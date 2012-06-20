#include "Document.h"

#include <cassert>
#include <cerrno>
#include <sstream>

#include <dirent.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @file Document.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the document implementation.
 */

std::string const Document::directory_ = "./documents/";

namespace document_errors
{
	DocumentError::DocumentError(std::string const &message)
		: std::runtime_error(message)
	{
	}

	DocumentAlreadyExistsError::DocumentAlreadyExistsError(std::string const &message)
		: DocumentError(message)
	{
	}

	DocumentDoesntExistError::DocumentDoesntExistError(std::string const &message)
		: DocumentError(message)
	{
	}

	DocumentPermissionsError::DocumentPermissionsError(std::string const &message)
		: DocumentError(message)
	{
	}
}

using namespace document_errors;

Document::Document(Document &&other)
	: contents_(std::move(other.contents_))
{
}

Document Document::create(std::string const &name, bool overwrite)
{
	// using Linux API here because of error checking functionality
	int flags = O_CREAT | O_WRONLY | O_TRUNC;

	if (!overwrite)
	{
		flags |= O_EXCL;
	}

	int const fd = ::open(name.c_str(), flags, 0644);

	if (fd < 0)
	{
		std::ostringstream strm;

		strm << "while creating document <" << name << ">: ";

		if (errno == EEXIST)
		{
			strm << "document already exists";

			throw std::runtime_error(strm.str()); //DocumentAlreadyExistsError(strm.str());
		}

		if (errno == EACCES || errno == EROFS)
		{
			strm << "insufficient permissions to create document";

			throw DocumentPermissionsError(strm.str());
		}

		strm << std::strerror(errno);

		throw DocumentError(strm.str());
	}

	return Document(fd, name);
}

Document Document::open(std::string const &name)
{
	// using Linux API here because of error checking functionality
	int const fd = ::open(name.c_str(), O_RDONLY);

	if (fd < 0)
	{
		std::ostringstream strm;

		strm << "while opening document <" << name << ">: ";

		if (errno == ENOENT)
		{
			strm << "document does not exist";

			throw DocumentDoesntExistError(strm.str());
		}

		if (errno == EACCES)
		{
			strm << "insufficient permissions to open document";

			throw DocumentPermissionsError(strm.str());
		}

		strm << std::strerror(errno);

		throw DocumentError(strm.str());
	}

	return Document(fd, name);
}

void Document::remove()
{
	int const result = ::unlink(name_.c_str());

	if (result)
	{
		std::ostringstream strm;

		strm << "while removing document <" << name_ << ">: ";

		if (errno == ENOENT)
		{
			strm << "document does not exist";

			throw DocumentDoesntExistError(strm.str());
		}

		if (errno == EACCES || errno == EROFS)
		{
			strm << "insufficient permissions to delete document";

			throw DocumentPermissionsError(strm.str());
		}

		strm << std::strerror(errno);

		throw DocumentError(strm.str());
	}
}

void Document::save()
{
	off_t const begin = ::lseek(fd_, 0, SEEK_SET);

	// should never fail
	assert(begin != static_cast<off_t>(-1));

	ssize_t const write_result = ::write(fd_, &contents_[0], contents_.size());

	if (static_cast<std::vector<char>::size_type>(write_result) != contents_.size())
	{
		throw DocumentError("unable to write all data to file");
	}
}

Document::hash_t Document::hash() const
{
	if (contents_.size() >= std::numeric_limits<unsigned long>::max())
	{
		throw std::runtime_error("unable to create hash because document is too big");
	}

	hash_t sha1_hash;

	// should never fail
	::SHA1(reinterpret_cast<unsigned char const *>(&contents_[0]), contents_.size(), &sha1_hash[0]);

	return sha1_hash;
}

std::vector<std::string> Document::list_documents()
{
	DIR *dir = ::opendir(Document::directory_.c_str());
	std::vector<std::string> list;

	if (!dir)
	{
		std::ostringstream strm;

		strm << "while listing documents <" << Document::directory_ << ">: ";

		if (errno == ENOENT || errno == ENOTDIR)
		{
			strm << "document directory does not exist";

			throw DocumentDoesntExistError(strm.str());
		}

		if (errno == EACCES)
		{
			strm << "insufficient permissions to open document directory";

			throw DocumentPermissionsError(strm.str());
		}

		strm << std::strerror(errno);

		throw DocumentError(strm.str());
	}

	::dirent *entry;

	try
	{
		while ((entry = ::readdir(dir)))
		{
			std::string const name = entry->d_name;

			if (name != "." && name != "..")
			{
				list.push_back(name);
			}
		}

		::closedir(dir);
	}
	catch (...)
	{
		::closedir(dir);
		throw;
	}

	return list;
}

Document::Document(int fd, std::string const &name)
try
	: fd_(fd),
	  name_(name)
{
	off_t const end = ::lseek(fd, 0, SEEK_END);

	if (end == static_cast<off_t>(-1))
	{
		// should only fail if file too large
		if (errno == EOVERFLOW)
		{
			throw DocumentError("file too big");
		}
	}

	// should never fail
	off_t const seek_result = ::lseek(fd, 0, SEEK_SET);

	assert(seek_result != static_cast<off_t>(-1));

	// reserve space
	contents_.resize(end);

	ssize_t const read_result = ::read(fd, &contents_[0], end);

	if (read_result != end)
	{
		throw DocumentError("unable to read all data from file");
	}
}
catch (...)
{
	close(fd);
	throw;
}
