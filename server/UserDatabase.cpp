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

namespace
{
	std::string const g_sql_queries[] = {
		"CREATE TABLE IF NOT EXISTS UserDatabase ("
			"u_id INTEGER NOT NULL,"
			"u_name VARCHAR(64) NOT NULL,"
			"u_pwhash VARCHAR(20) NOT NULL,"
			"PRIMARY KEY(u_id),"
			"UNIQUE(u_name)"
		");",
		"SELECT u_pwhash FROM UserDatabase WHERE u_name = %Q;",
		"INSERT INTO UserDatabase VALUES (%Q, %Q);",
		"DELETE FROM UserDatabase WHERE p_name = %Q;",
	};
}

UserDatabase::UserDatabase(std::shared_ptr<Database> database,
                           UserInterface &user_interface)
	: database_(database),
	  user_interface_(user_interface)
{
	database_->execute_sql(g_sql_queries[0]);
	user_interface.printf("user database loaded\n");
}

void UserDatabase::check(std::string const &name, password_hash_t const &password_hash)
{
	Database::results_t const result = database_->execute_sql(g_sql_queries[1], name.c_str());

	user_interface_.printf("user check\n");

	for (auto const &row: result)
	{
		for (auto const &key_value: row)
		{
			user_interface_.printf("%s: %s\n", key_value.first.c_str(), key_value.second.c_str());
		}
	}

	throw std::runtime_error("STUB UserDatabase::check");
}

void UserDatabase::create(std::string const &name, password_hash_t const &password_hash)
{
	std::string const password_hash_string(password_hash.begin(), password_hash.end());

	Database::results_t const result = database_->execute_sql(g_sql_queries[2], name.c_str(), password_hash[0]);

	user_interface_.printf("user create\n");

	for (auto const &row: result)
	{
		for (auto const &key_value: row)
		{
			user_interface_.printf("%s: %s\n", key_value.first.c_str(), key_value.second.c_str());
		}
	}

	throw std::runtime_error("STUB UserDatabase::create");
}

void UserDatabase::remove(std::string const &name)
{
	Database::results_t const result = database_->execute_sql(g_sql_queries[3], name.c_str());

	user_interface_.printf("user delete\n");

	for (auto const &row: result)
	{
		for (auto const &key_value: row)
		{
			user_interface_.printf("%s: %s\n", key_value.first.c_str(), key_value.second.c_str());
		}
	}

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
