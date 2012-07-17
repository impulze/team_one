#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include "Hash.h"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @file server/Document.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for the document implementation.
 */

namespace document_errors
{
	/**
	 * A generic document exception.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct DocumentError
		: std::runtime_error
	{
		/**
		 * Construct a new document error with a specific error message.
		 *
		 * @param message The error message that describes the error.
		 */
		DocumentError(std::string const &message);
	};

	/**
	 * This exception occurs while trying to open a document for writing
	 * without the intention of overwriting even though it already
	 * exists.
	 */
	struct DocumentAlreadyExistsError
		: DocumentError
	{
		/**
		 * Construct the error with a specific error message.
		 *
		 * @param message The error message that describes the error.
		 */
		DocumentAlreadyExistsError(std::string const &message);
	};

	/**
	 * This exception occurs while trying to open/remove documents that
	 * no longer exist or never existed in the first place.
	 * It can also occur while listing directory contents while the directory
	 * is not present.
	 */
	struct DocumentDoesntExistError
		: DocumentError
	{
		/**
		 * Construct the error with a specific error message.
		 *
		 * @param message The error message that describes the error.
		 */
		DocumentDoesntExistError(std::string const &message);
	};

	/**
	 * This exception occurs whenever trying to open/access the documents/directories
	 * without having the proper permissions to do so.
	 */
	struct DocumentPermissionsError
		: DocumentError
	{
		/**
		 * Construct the error with a specific error message.
		 *
		 * @param message The error message that describes the error.
		 */
		DocumentPermissionsError(std::string const &message);
	};

	/**
	 * This is typically a user error which happens whenever reading/saving
	 * documents although they're already closed.
	 */
	struct DocumentClosedError
		: DocumentError
	{
		/**
		 * Construct the error with a specific error message.
		 *
		 * @param message The error message that describes the error.
		 */
		DocumentClosedError(std::string const &message);
	};
}

/**
 * Representation of a document.
 * The document can only be created by the named constructors
 * Document::create() and Document::open().
 * To copy a document to another one can Document::open() one
 * document and Document::create() another empty document.
 * The the contents of the 2nd document can be modified by
 * modifying the byte container returned by Document::get_contents().
 * The resulting document can then be saved to the disk by calling
 * Document::save().
 */
class Document
{
public:
	/**
	 * Move a document.
	 *
	 * The resources are moved and you can no longer expect
	 * any useful state of this object after moving it.
	 */
	Document(Document &&);

	/**
	* Release all resources allocated by the document.
	* Also calls close()
	*/
	~Document();

	/**
	 * Create a document by name.
	 *
	 * Refer to open_writable() and Document() to see possible Exceptions.
	 *
	 * @param name The name the document is referenced by.
	 * @param overwrite Allow overwriting if the document exists.
	 * @return The Document instance.
	 */
	static Document create(std::string const &name, bool overwrite = false);

	/**
	 * Open a document by name.
	 *
	 * Refer to open_readable() and Document() to see possible Exceptions.
	 *
	 * @param name The name the document is referenced by.
	 * @return The Document instance.
	 */
	static Document open(std::string const &name);

	/**
	 * Check if a document is empty.
	 *
	 * Refer to open_readable() which additional exceptions can occur.
	 *
	 * @throws document_errors::DocumentError If the document size exceeds
	 *                                        the upper limit of the size of
	 *                                        the local off_t type.
	 * @return true if empty, false otherwise.
	 */
	static bool is_empty(std::string const &name);

	/**
	 * Remove the document physically.
	 *
	 * @throws document_errors::DocumentDoesntExistError If the document doesn't exist. Consider the
	 *                                                   situation where 2 instances of this document
	 *                                                   are present and one is already removed.
	 * @throws document_errors::DocumentPermissionsError If the remover lacks sufficient permissions to
	 *                                                   remove the file.
	 * @throws document_errors::DocumentError If removing fails for other reasons.
	 */
	void remove();

	/**
	 * Save the document physically.
	 *
	 * @throws document_errors::DocumentClosedError If the document was closed by a
	 *                                              call to close() prior to this call.
	 * @throws document_errors::DocumentError If not all data could be copied.
	 */
	void save();

	/**
	 * Close the document.
	 *
	 * Further reading/saving will result in a document_errors::DocumentClosedError
	 * being thrown.
	 * Calling close() more than once will result in a NO-OP.
	 */
	void close();

	/**
	 * Create a SHA-1 hash for this document and return it.
	 *
	 * Refer to get_contents() to see other possible exceptions that can get thrown.
	 * Those are thrown if get_contents() wasn't called prior to this call.
	 *
	 * @throws std::runtime_error If the document size exceeds the upper limit of the
	 *                            size of the local unsigned long type.
	 * @return The SHA-1 hash (20 bytes) of the contents.
	 */
	Hash::hash_t hash();

	/**
	 * Obtain the bytes of the document.
	 *
	 * @throws document_errors::DocumentClosedError If the document was closed by a
	 *                                              call to close() prior to this call.
	 * @throws document_errors::DocumentError If the document size exceeds
	 *                                        the upper limit of the size of
	 *                                        the local off_t type.
	 * @throws document_errors::DocumentError If reading the internal file descriptor
	 *                                        fails.
	 * @return A reference to the vector with all the bytes of the document.
	 */
	std::vector<char> &get_contents();

	/**
	 * Obtain a list of documents that can be opened.
	 *
	 * @throws document_errors::DocumentDoesntExistError If the directory cannot
	 *                                                   be accessed.
	 * @throws document_errors::DocumentPermissionsError If the caller lacks
	 *                                                   permissions to obtain
	 *                                                   directory entries.
	 * @throws document_errors::DocumentError If any other error occured during
	 *                                        directory listing.
	 * @return A list of documents that can be opened. This does not include the
	 *         standard unix directories (links) '.' and '..'.
	 */
	static std::vector<std::string> list_documents();

	/**
	 * Get the name the document was created with.
	 *
	 * @return The name passed during construction.
	 */
	std::string const &get_name() const
	{
		return name_;
	}

	/**
	 * A global id for the document.
	 *
	 * After opening 2147483647 documents, the id restarts
	 * at 0.
	 *
	 * @return id for the document between [0, 2147483647]
	 */
	std::int32_t get_id() const
	{
		return id_;
	}

private:
	/**
	 * Open a document by name and return the file descriptor.
	 *
	 * @param name The name the document is referenced by.
	 * @return The UNIX file descriptor.
	 * @throws document_errors::DocumentDoesntExistError If the document doesn't exist.
	 * @throws document_errors::DocumentPermissionsError If the opener lacks sufficient permissions to
	 *                                                   open the file.
	 * @throws document_errors::DocumentError If opening fails for other reasons.
	 */
	static int open_readable(std::string const &name);

	/**
	 * Open a document by name and return the file descriptor.
	 *
	 * @param name The name the document is referenced by.
	 * @param overwrite Use to determine if a file can be overwritten if it does
	 *                  already exist.
	 * @return The UNIX file descriptor.
	 * @throws document_errors::DocumentAlreadyExistsError If the document doesn't exist.
	 * @throws document_errors::DocumentPermissionsError If the opener lacks sufficient permissions to
	 *                                                   open the file.
	 * @throws document_errors::DocumentError If opening fails for other reasons.
	 */
	static int open_writable(std::string const &name, bool overwrite);

	/**
	 * Increment the global document id and consider wrap around.
	 */
	static void increment_global_document_id();

	/**
	 * Create a document with a linux specific file descriptor.
	 *
	 * See get_contents() to see which exceptions can occur.
	 * The constructor initially reads all the contents.
	 *
	 * @param fd The descriptor for this document. The descriptor must
	 *           be a valid descriptor as returned by open(2). Otherwise
	 *           the behaviour is undefined.
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

	//! byte container for the document
	std::vector<char> contents_;
	//! unix file descriptor valid until close() was called
	int fd_;
	//! the name which was passed from create() or open()
	std::string const name_;
	//! the directory in which all server documents can be found
	static std::string const directory_;
	//! the global document id, wraps after 2147483647
	static std::int32_t global_document_id_;
	//! the id for this particular document instance
	std::int32_t id_;
	//! indicator for closed documents, true after close()
	bool document_closed_;
	//! indicator for fetched contents, true after get_contents()
	bool contents_fetched_;
};

#endif
