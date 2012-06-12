#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include <array>
#include <cstdint>
#include <vector>

/**
 * @file Document.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for the document implementation.
 */

class Document
{
public:
	typedef std::array<char, 20> hash_t;

	/**
	 * Create a document by name.
	 *
	 * @param name The name the document is referenced by.
	 * @param overwrite Allow overwriting if the document exists.
	 */
	explicit Document(std::string const &name, bool overwrite = false);

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
	std::vector<char> contents_;
};

#endif
