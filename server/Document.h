#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @file Document.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for the document implementation.
 */

namespace document_errors
{
	struct DocumentError
		: std::runtime_error
	{
		DocumentError(std::string const &message);
	};

	struct DocumentAlreadyExistsError
		: DocumentError
	{
		DocumentAlreadyExistsError(std::string const &message);
	};

	struct DocumentDoesntExistError
		: DocumentError
	{
		DocumentDoesntExistError(std::string const &message);
	};

	struct DocumentPermissionsError
		: DocumentError
	{
		DocumentPermissionsError(std::string const &message);
	};
}

class Document
{
public:
	typedef std::array<char, 20> hash_t;

	/**
	 * Move a ddocument.
	 *
	 * The resources are moved and you can no longer expect
	 * any useful state of this object after moving it.
	 */
	Document(Document &&);

	/**
	 * Create a document by name.
	 *
	 * @param name The name the document is referenced by.
	 * @param overwrite Allow overwriting if the document exists.
	 */
	static Document create(std::string const &name, bool overwrite = false);

	/**
	 * Open a document by name.
	 *
	 * @param name The name the document is referenced by.
	 */
	static Document open(std::string const &name);

	/**
	 * Remove the document physically.
	 */
	void remove();

	/**
	 * Save the document physically.
	 */
	void save();

	/**
	 * Create a SHA-1 hash for this document and return it.
	 *
	 * @return The SHA-1 hash (20 bytes) of the contents.
	 */
	hash_t hash() const;

	/**
	 * Obtain the bytes of the document.
	 *
	 * @returnA reference to the vector with all the bytes of the document.
	 */
	std::vector<char> &get_contents()
	{
		return contents_;
	}

	/**
	 * Obtain a list of documents that can be opened.
	 *
	 * @return A list of documents that can be opened.
	 */
	static std::vector<std::string> list_documents();

private:
	/**
	 * Create a document with a linux specific file descriptor.
	 *
	 * @param fd The descriptor for this document.
	 * @param name The name the document is referenced by.
	 */
	explicit Document(int fd, std::string const &name);

	/**
	 * Delete the default copy constructor, making copying a document object
	 * impossible.
	 */
	Document(Document const &) = delete;

	/**
	 * Delete the default assignment operator, making assigning a document object
	 * impossible.
	 */
	Document &operator=(Document const &) = delete;

	std::vector<char> contents_;
	int fd_;
	std::string const name_;
	static std::string const directory_;
};

#endif
