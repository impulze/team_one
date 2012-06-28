#ifndef USERDATABASE_H_INCLUDED
#define USERDATABASE_H_INCLUDED

#include "Database.h"
#include "Hash.h"

#include <cstdint>
#include <memory>
#include <string>

/**
 * @file UserDatabase.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for the user implementation.
 */

class UserInterface;

namespace userdatabase_errors
{
	/**
	 * A generic user database exception.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct Failure
		: database_errors::Failure
	{
		/**
		 * Construct a new user database failure with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		Failure(std::string const &message);
	};

	/**
	 * Thrown if a user is already in the database and is to be
	 * newly added.
	 */
	struct UserAlreadyPresentError
		: database_errors::ConstraintError
	{
		/**
		 * Construct a new UserAlreadyPresentError with a specific
		 * error message.
		 *
		 * @param name The username.
		 */
		UserAlreadyPresentError(std::string const &name);
	};

	/**
	 * Thrown if a user does not exists although it should be.
	 */
	struct UserDoesntExistError
		: Failure
	{
		/**
		 * Construct a new UserDoesntExistError with a specific
		 * error message.
		 *
		 * @param name The username.
		 */
		UserDoesntExistError(std::string const &name);
	};

	/**
	 * Thrown if a password is invalid for a certain user.
	 */
	struct InvalidPasswordError
		: Failure
	{
		/**
		 * Construct a new InvalidPasswordError with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		InvalidPasswordError(std::string const &message);
	};
}

/**
 * Basically this is just an object that interacts with a
 * database and provides mechanisms to create and remove users
 * and to check the credentials for users.
 */
class UserDatabase
{
public:
	/**
	 * Construct the user database.
	 *
	 * Refer to Database::execute_sql() to see which additional
	 * constraints apply.
	 *
	 * @param database A shared database handle.
	 * @param user_interface The user interface instance.
	 */
	UserDatabase(std::shared_ptr<Database> database,
	             UserInterface &user_interface);

	/**
	 * Check credentials of a user.
	 *
	 * @param name The name the user is referenced by.
	 * @param password_hash The password SHA-1 hash that was generated.
	 * @throws userdatabase_errors::UserDoesntExistError If the user doesn't exist.
	 * @throws userdatabase_errors::Failure If the maximum user id can't be
	 *                                      determined.
	 * @throws userdatabase_errors::Failure If the stored amount of stored users exceeds the
	 *                                      limit of std::int32_t.
	 * @throws userdatabase_errors::Failure If the password hash or user id is missing from the
	 *                                      database entry.
	 * @throws userdatabase_errors::InvalidPasswordError If the password doesn't match.
	 * @returns The user id if the user is found and the password hash matches.
	 */
	std::int32_t check(std::string const &name, Hash::hash_t const &password_hash);

	/**
	 * Create a user in the database with the specified name and password hash.
	 * The password hash can be generated by interacting with the Hash class.
	 *
	 * @param name The name the user is referenced by.
	 * @param password_hash The password SHA-1 hash that was generated.
	 * @throws userdatabase_errors::UserAlreadyPresentError If the user is already in
	 *                                                      the database.
	 */
	void create(std::string const &name, Hash::hash_t const &password_hash);

	/**
	 * Create a user in the database with the specified name and plain password.
	 *
	 * \overload
	 * Refer to create(std::string const &, std::string const &) to see which additional exceptions can occur.
	 *
	 * @param name The name the user is referenced by.
	 * @param password The plain text password.
	 */
	void create(std::string const &name, std::string const &password);

	/**
	 * Delete a user in the database with the specified name.
	 *
	 * @param name The name the user is referenced by.
	 * @throws userdatabase_errors::UserDoesntExistError If the user doesn't exist yet
	 *                                                   in the database.
	 * @throws userdatabase_errors::Failure If the user is stored without a user_id.
	 */
	void remove(std::string const &name);

private:
	//! A pointer to the underlying database for users.
	std::shared_ptr<Database> database_;
	//! A reference to the user interface, must stay valid until destruction of this instance.
	UserInterface &user_interface_;
};

#endif
