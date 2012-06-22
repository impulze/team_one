#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include "Hash.h"

#include <array>
#include <cstdint>
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
	 * @param name The name the document is referenced by.
	 * @param overwrite Allow overwriting if the document exists.
	 * @throws DocumentAlreadyExistsError If the document does exists and overwrite is
	 *                                    false.
	 * @throws DocumentPermissionsError If the file would have to be created but the
	 *                                  creater lacks sufficient permissions.
	 * @throws DocumentError If creating fails for other reasons.
	 */
	static Document create(std::string const &name, bool overwrite = false);

	/**
	 * Open a document by name.
	 *
	 * @param name The name the document is referenced by.
	 * @return The Document instance.
	 * @throws DocumentDoesntExistError If the document doesn't exist.
	 * @throws DocumentPermissionsError If the opener lacks sufficient permissions to
	 *                                  open the file.
	 * @throws DocumentError If opening fails for other reasons.
	 */
	static Document open(std::string const &name);

	/**
	 * Check if a document is empty.
	 *
	 * @param name The name the document is referenced by.
	 * @return true if empty, false otherwise.
	 * @throws DocumentDoesntExistError If the document doesn't exist.
	 * @throws DocumentPermissionsError If the opener lacks sufficient permissions to
	 *                                  open the file.
	 * @throws DocumentError If opening fails for other reasons.
	 */
	static bool is_empty(std::string const &name);

	/**
	 * Remove the document physically.
	 *
	 * @throws DocumentDoesntExistError If the document doesn't exist.
	 * @throws DocumentPermissionsError If the remover lacks sufficient permissions to
	 *                                  remove the file.
	 * @throws DocumentError If removing fails for other reasons.
	 */
	void remove();

	/**
	 * Save the document physically.
	 *
	 * @throws DocumentError If not all data could be copied.
	 */
	void save();

	/**
	 * Close the document.
	 */
	void close();

	/**
	 * Create a SHA-1 hash for this document and return it.
	 *
	 * @return The SHA-1 hash (20 bytes) of the contents.
	 */
	Hash::hash_t hash() const;

	/**
	 * Obtain the bytes of the document.
	 *
	 * @return A reference to the vector with all the bytes of the document.
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

	/**
	 * Obtain the id.
	 *
	 * @return The id passed in the constructor.
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
	 * @throws DocumentDoesntExistError If the document doesn't exist.
	 * @throws DocumentPermissionsError If the opener lacks sufficient permissions to
	 *                                  open the file.
	 * @throws DocumentError If opening fails for other reasons.
	 */
	static int open_readable(std::string const &name);

	/**
	 * Create a document with a linux specific file descriptor.
	 *
	 * @param fd The descriptor for this document.
	 * @param name The name the document is referenced by.
	 * @param id The id for this document. Keep in mind that once the
	 *           maximum id is reached, it starts at 0 again.
	 */
	explicit Document(int fd, std::string const &name, std::int32_t id);

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
	std::int32_t id_;
	bool document_closed_;
};

#endif
