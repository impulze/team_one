#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @file server/Hash.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for the SHA-1 hash implementation.
 */

namespace hash_errors
{
	/**
	 * An invalid hash error.
	 * This is always relevant when dealing with invalid
	 * hashes, for example if their sizes are bigger
	 * or their representation faulty.
	 */
	struct InvalidHashError
		: std::runtime_error
	{
		/**
		 * Construct a new invalid hash error with a specific error message.
		 *
		 * @param message The error message that describes the error.
		 */
		InvalidHashError(std::string const &message);
	};
}

/**
 * A class providing functionality of SHA-1 hashes.
 * One can hash some bytes or strings (which are
 * basically converted to bytes before working with
 * them).
 * SHA-1 hashes can also be represented by hexadecimal
 * strings, see hash_to_string().
 *
 * @startuml{Hash_Class.svg}
 * class Hash {
 * + {static} hash_bytes(bytes: vector<char> const &): array<char, 20>
 * + {static} hash_to_string(hash: array<char, 20>): string
 * + {static} string_to_hash(hash_string: string const &): array<char, 20>
 * }
 * @enduml
 */
class Hash
{
public:
	//! The underlying storage for a hash.
	typedef std::array<char, 20> hash_t;

	/**
	 * Create a hash for the specificied byte sequence.
	 *
	 * @param bytes A sequence of bytes as data input for the hash algorithm.
	 * @return The hash sequence for the specified bytes.
	 */
	static hash_t hash_bytes(std::vector<char> const &bytes);

	/**
	 * Convert a raw hash bytestream to hexadecimal represented string.
	 *
	 * @param hash The raw bytes of the hash.
	 * @return The hexadecimal string representation of the hash.
	 */
	static std::string hash_to_string(hash_t const &hash);

	/**
	 * Convert a hexadecimal represented string to a raw hash byte sequence.
	 *
	 * @param hash_string The raw bytes of the hash.
	 * @return The hexadecimal string representation of the hash.
	 */
	static hash_t string_to_hash(std::string const &hash_string);
};

#endif
