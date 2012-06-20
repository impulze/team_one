#include "UserDatabase.h"

#include "UserInterface.h"

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

UserDatabase::password_hash_t UserDatabase::hash_bytes(std::vector<char> const &)
{
	throw std::runtime_error("STUB UserDatabase::hash_bytes");
}
