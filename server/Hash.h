#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

namespace hash_errors
{
	struct InvalidHashError
		: std::runtime_error
	{
		InvalidHashError(std::string const &message);
	};
}

class Hash
{
public:
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
