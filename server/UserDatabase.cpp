#include "UserDatabase.h"

#include "UserInterface.h"

#include <openssl/sha.h>

#include <sstream>
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
			"u_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
			"u_name VARCHAR(64) NOT NULL,"
			"u_pwhash BLOB NOT NULL,"
			"UNIQUE(u_name)"
		");",
		"SELECT u_pwhash FROM UserDatabase WHERE u_name = %Q;",
		"INSERT INTO UserDatabase (u_name, u_pwhash) VALUES (%Q, %Q);",
		"DELETE FROM UserDatabase WHERE p_name = %Q;",
	};
}

namespace userdatabase_errors
{
	UserAlreadyPresentError::UserAlreadyPresentError(std::string const &message)
		: std::runtime_error(message)
	{
	}
}

UserDatabase::UserDatabase(std::shared_ptr<Database> database,
                           UserInterface &user_interface)
	: database_(database),
	  user_interface_(user_interface)
{
	user_interface.printf("user database loading: loading\n");

	database_->execute_sql(g_sql_queries[0]);

	user_interface.printf("user database loading: success\n");
}

void UserDatabase::check(std::string const &name, password_hash_t const &password_hash)
{
	user_interface_.printf("checking credentials: \"%s\"\n");

	Database::results_t const result = database_->execute_sql(g_sql_queries[1], name);

	user_interface_.printf("checking credentials: succes\n");

	// debugging
	for (auto const &row: result)
	{
		for (auto const &key_value: row)
		{
			user_interface_.printf("%s: %s\n", key_value.first, key_value.second);
		}
	}
}

void UserDatabase::create(std::string const &name, password_hash_t const &password_hash)
{
	std::ostringstream password_hash_readable;

	for (auto const &ub: password_hash)
	{
		password_hash_readable
			<< std::hex
			<< ((ub & 0xf0) >> 4)
			<< ((ub & 0x0f) >> 0);
	}

	user_interface_.printf("adding user: \"%s\", [password_hash: \"%s\"\n", name, password_hash_readable.str());

	try
	{
		Database::results_t const result = database_->execute_sql(g_sql_queries[2], name, password_hash);

		// debugging
		for (auto const &row: result)
		{
			for (auto const &key_value: row)
			{
				user_interface_.printf("%s: %s\n", key_value.first, key_value.second);
			}
		}
	}
	catch (...)
	{
		std::ostringstream strm;

		strm << "user \"" << name << "\" already present in database.";

		user_interface_.printf("adding user: failed (%s)\n", strm.str());

		throw userdatabase_errors::UserAlreadyPresentError(strm.str());
	}

	user_interface_.printf("adding user: success\n");

}

void UserDatabase::create(std::string const &name, std::string const &password)
{
	std::vector<char> const password_bytes(password.begin(), password.end());
	auto const password_hash = hash_bytes(password_bytes);

	create(name, password_hash);
}

void UserDatabase::remove(std::string const &name)
{
	user_interface_.printf("remove user: \"%s\"\n", name);

	Database::results_t const result = database_->execute_sql(g_sql_queries[3], name);

	user_interface_.printf("remove user: success\n");

	// debugging
	for (auto const &row: result)
	{
		for (auto const &key_value: row)
		{
			user_interface_.printf("%s: %s\n", key_value.first, key_value.second);
		}
	}
}

UserDatabase::password_hash_t UserDatabase::hash_bytes(std::vector<char> const &bytes)
{
	if (bytes.size() >= std::numeric_limits<unsigned long>::max())
	{
		throw std::runtime_error("unable to create hash because byte container is too big");
	}

	password_hash_t sha1_hash;

	// should never fail
	::SHA1(
		reinterpret_cast<unsigned char const *>(&bytes[0]),
		bytes.size(),
		reinterpret_cast<unsigned char *>(&sha1_hash[0]));

	return sha1_hash;
}
