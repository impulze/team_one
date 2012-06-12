#include "User.h"

#include <stdexcept>

/**
 * @file User.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the user implementation.
 */

void User::check(std::string const &, password_hash_t const &)
{
	throw std::runtime_error("STUB User::check");
}

void User::create(std::string const &, password_hash_t const &)
{
	throw std::runtime_error("STUB User::create");
}

void User::remove(std::string const &)
{
	throw std::runtime_error("STUB User::remove");
}

User::password_hash_t User::hash_bytes(std::vector<char> const &)
{
	throw std::runtime_error("STUB User::hash_bytes");
}
