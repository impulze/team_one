#include "Hash.h"

#include <openssl/sha.h>

#include <sstream>

namespace hash_errors
{
	InvalidHashError::InvalidHashError(std::string const &message)
		: std::runtime_error(message)
	{
	}
}

Hash::hash_t Hash::hash_bytes(std::vector<char> const &bytes)
{
	hash_t sha1_hash;

	// should never fail
	::SHA1(
		reinterpret_cast<unsigned char const *>(&bytes[0]),
		bytes.size(),
		reinterpret_cast<unsigned char *>(&sha1_hash[0]));

	return sha1_hash;
}

std::string Hash::hash_to_string(hash_t const &hash)
{
	if (hash.size() != 20)
	{
		throw hash_errors::InvalidHashError("invalid length for SHA-1 hash");
	}

	std::ostringstream hash_hex;

	for (auto const &byte: hash)
	{
		hash_hex
			<< std::hex
			<< ((byte & 0xf0) >> 4)
			<< ((byte & 0x0f) >> 0);
	}

	return hash_hex.str();
}

Hash::hash_t Hash::string_to_hash(std::string const &hash_string)
{
	if (hash_string.size() != 40)
	{
		throw hash_errors::InvalidHashError("invalid length for SHA-1 hash");
	}

	hash_t hash;

	for (std::string::size_type i = 0; i < 20; i++)
	{
		std::string tuple;

		tuple.push_back(hash_string[2 * i]);
		tuple.push_back(hash_string[2 * i + 1]);

		hash[i] = std::stoi(tuple, 0, 16);
	}

	return hash;
}
