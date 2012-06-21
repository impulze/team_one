#include "UserDatabase.h"

#include "UserInterface.h"

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
			"u_pwhash VARCHAR(40) NOT NULL,"
			"UNIQUE(u_name)"
		");",
		"SELECT u_id, u_pwhash FROM UserDatabase WHERE u_name = %Q;",
		"INSERT INTO UserDatabase (u_name, u_pwhash) VALUES (%Q, %Q);",
		"DELETE FROM UserDatabase WHERE p_name = %Q;",
		"SELECT MAX(u_id) FROM UserDatabase",
	};
}

namespace userdatabase_errors
{
	Failure::Failure(std::string const &message)
		: database_errors::Failure(message)
	{
	}

	UserAlreadyPresentError::UserAlreadyPresentError(std::string const &message)
		: database_errors::ConstraintError(message)
	{
	}

	UserDoesntExistError::UserDoesntExistError(std::string const &message)
		: Failure(message)
	{
	}

	InvalidPasswordError::InvalidPasswordError(std::string const &message)
		: Failure(message)
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

std::int32_t UserDatabase::check(std::string const &name, Hash::hash_t const &password_hash)
{
	user_interface_.printf("checking credentials: \"%s\"\n", name);

	{
		Database::results_t result = database_->execute_sql(g_sql_queries[4]);

		if (result.size() != 1)
		{
			throw userdatabase_errors::Failure("no user in database yet");
		}

		if (result[0].find("MAX(u_id)") == result[0].end())
		{
			throw userdatabase_errors::Failure("no maximum user id");
		}

		std::int32_t maximum_user_id = std::stoi(result[0]["MAX(u_id)"]);

		if (maximum_user_id == std::numeric_limits<std::int32_t>::max())
		{
			throw userdatabase_errors::Failure("too many users");
		}
	}

	Database::results_t result = database_->execute_sql(g_sql_queries[1], name);

	if (result.size() != 1)
	{
		throw userdatabase_errors::UserDoesntExistError("user not in database");
	}

	if (result[0].find("u_pwhash") == result[0].end() || result[0].find("u_id") == result[0].end())
	{
		throw userdatabase_errors::Failure("no password hash or user id for user");
	}

	if (Hash::string_to_hash(result[0]["u_pwhash"]) != password_hash)
	{
		throw userdatabase_errors::InvalidPasswordError("invalid password");
	}

	user_interface_.printf("checking credentials: succes\n");

	return std::stoi(result[0]["u_id"]);
}

void UserDatabase::create(std::string const &name, Hash::hash_t const &password_hash)
{
	std::string const password_hash_readable = Hash::hash_to_string(password_hash);

	user_interface_.printf("adding user: \"%s\", [password_hash: \"%s\"\n", name, password_hash_readable);

	try
	{
		Database::results_t const result = database_->execute_sql(g_sql_queries[2], name, password_hash_readable);

		// debugging
		for (auto const &row: result)
		{
			for (auto const &key_value: row)
			{
				user_interface_.printf("%s: %s\n", key_value.first, key_value.second);
			}
		}
	}
	catch (database_errors::ConstraintError const &)
	{
		std::ostringstream strm;

		strm << "user \"" << name << "\" already present in database.";

		throw userdatabase_errors::UserAlreadyPresentError(strm.str());
	}

	user_interface_.printf("adding user: success\n");

}

void UserDatabase::create(std::string const &name, std::string const &password)
{
	std::vector<char> const password_bytes(password.begin(), password.end());
	auto const password_hash = Hash::hash_bytes(password_bytes);

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
