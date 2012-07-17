#include "Document.h"

#include <cassert>
#include <cerrno>
#include <sstream>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @file server/Document.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the document implementation.
 */

/**
 * Global variable storing the documents directory for the server.
 */
std::string const Document::directory_ = "./documents/";

/**
 * A global variable for the current document id. Wraps after
 * 2147483647
 */
std::int32_t Document::global_document_id_ = 1;

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

	DocumentClosedError::DocumentClosedError(std::string const &message)
		: DocumentError(message)
	{
	}
}

using namespace document_errors;

Document::Document(Document &&other)
	: contents_(std::move(other.contents_)),
	  fd_(other.fd_),
	  name_(std::move(other.name_)),
	  id_(other.id_),
	  document_closed_(other.document_closed_),
	  contents_fetched_(other.contents_fetched_)
{
	// prevent the other destructor to call close
	other.document_closed_ = true;
}

Document::~Document()
{
	close();
}

Document Document::create(std::string const &name, bool overwrite)
{

	int const fd = open_writable(name, overwrite);

	return Document(fd, name);
}

Document Document::open(std::string const &name)
{
	int const fd = open_readable(name);

	return Document(fd, name);
}

bool Document::is_empty(std::string const &name)
{
	int const fd = open_readable(name);

	off_t const now = ::lseek(fd, 0, SEEK_CUR);
	off_t const begin = ::lseek(fd, 0, SEEK_SET);
	off_t const end = ::lseek(fd, 0, SEEK_END);
	off_t const now_again = ::lseek(fd, now, SEEK_SET);

	::close(fd);

	// should never fail
	assert(now != static_cast<off_t>(-1));
	assert(begin != static_cast<off_t>(-1));
	assert(now_again != static_cast<off_t>(-1));

	if (end == static_cast<off_t>(-1))
	{
		// should only fail if file too large
		if (errno == EOVERFLOW)
		{
			throw DocumentError("file too big");
		}
	}

	return end == 0;
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
	if (document_closed_)
	{
		throw DocumentClosedError("trying to write contents from document that got closed");
	}

	ssize_t const write_result = ::write(fd_, &contents_[0], contents_.size());

	if (static_cast<std::vector<char>::size_type>(write_result) != contents_.size())
	{
		throw DocumentError("unable to write all data to file");
	}
}

void Document::close()
{
	if (!document_closed_)
	{
		::close(fd_);
		document_closed_ = true;
	}
}

Hash::hash_t Document::hash()
{
	if (!contents_fetched_)
	{
		get_contents();
	}

	if (contents_.size() >= std::numeric_limits<unsigned long>::max())
	{
		throw std::runtime_error("unable to create hash because document is too big");
	}

	return Hash::hash_bytes(contents_);
}

std::vector<char> &Document::get_contents()
{
	if (contents_fetched_)
	{
		return contents_;
	}

	if (document_closed_)
	{
		throw DocumentClosedError("trying to read contents in document that got closed");
	}

	off_t const end = ::lseek(fd_, 0, SEEK_END);

	if (end == static_cast<off_t>(-1))
	{
		// should only fail if file too large
		if (errno == EOVERFLOW)
		{
			throw DocumentError("file too big");
		}
	}

	// reserve space
	contents_.resize(end);

	ssize_t const read_result = ::read(fd_, &contents_[0], end);

	if (read_result != end)
	{
		throw DocumentError("unable to read all data from file");
	}

	contents_fetched_ = true;
	return contents_;
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

int Document::open_readable(std::string const &name)
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

	return fd;
}

int Document::open_writable(std::string const &name, bool overwrite)
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

			throw DocumentAlreadyExistsError(strm.str());
		}

		if (errno == EACCES || errno == EROFS)
		{
			strm << "insufficient permissions to create document";

			throw DocumentPermissionsError(strm.str());
		}

		strm << std::strerror(errno);

		throw DocumentError(strm.str());
	}

	return fd;
}

void Document::increment_global_document_id()
{
	if (global_document_id_ == std::numeric_limits<std::int32_t>::max())
	{
		global_document_id_ = 1;
	}
	else
	{
		global_document_id_++;
	}
}

Document::Document(int fd, std::string const &name)
	: fd_(fd),
	  name_(name),
	  id_(global_document_id_),
	  document_closed_(false),
	  contents_fetched_(false)
{
	get_contents();
	increment_global_document_id();
}
