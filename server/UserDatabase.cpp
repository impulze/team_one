#include "UserDatabase.h"

#include "UserInterface.h"

#include <openssl/sha.h>

#include <stdexcept>

/**
 * @file UserDatabaseDatabase.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the user database implementation.
 */

UserDatabase::UserDatabase(std::shared_ptr<Database> database,
                           UserInterface &user_interface)
	: database_(database),
	  user_interface_(user_interface)
{
}

void UserDatabase::check(std::string const &, password_hash_t const &)
{
	throw std::runtime_error("STUB UserDatabase::check");
}

void UserDatabase::create(std::string const &, password_hash_t const &)
{
	throw std::runtime_error("STUB UserDatabase::create");
}

void UserDatabase::remove(std::string const &)
{
	throw std::runtime_error("STUB UserDatabase::remove");
}

UserDatabase::password_hash_t UserDatabase::hash_bytes(std::vector<char> const &bytes)
{
	if (bytes.size() >= std::numeric_limits<unsigned long>::max())
	{
		throw std::runtime_error("unable to create hash because byte container is too big");
	}

	password_hash_t sha1_hash;

	// should never fail
	::SHA1(reinterpret_cast<unsigned char const *>(&bytes[0]), bytes.size(), &sha1_hash[0]);

	return sha1_hash;
}
