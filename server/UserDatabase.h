#ifndef USERDATABASE_H_INCLUDED
#define USERDATABASE_H_INCLUDED

#include "Database.h"
#include "Hash.h"

#include <cstdint>
#include <memory>
#include <string>

/**
 * @file server/UserDatabase.h
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

	/**
	 * Thrown if a user database is not present yet but was requested
	 * by a call to UserDatabase::get_instance().
	 */
	struct UserDatabaseNotConstructedError
		: Failure, std::logic_error
	{
		/**
		 * Construct a new UserDatabaseNotConstructedError with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		UserDatabaseNotConstructedError(std::string const &message);
	};

	/**
	 * Thrown if a user database is present and the constructor is
	 * called again.
	 */
	struct UserDatabaseAlreadyConstructedError
		: Failure, std::logic_error
	{
		/**
		 * Construct a new UserDatabaseAlreadyConstructedError with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		UserDatabaseAlreadyConstructedError(std::string const &message);
	};
}

/**
 * Basically this is just an object that interacts with a
 * database and provides mechanisms to create and remove users
 * and to check the credentials for users.
 *
 * This is a singleton, because only one user database shall be active
 * at a time. Although this implementation cannot be default constructed,
 * hence make sure to call the constructor first.
 *
 * @startuml{UserDatabase_Class.svg}
 * class UserDatabase << singleton >> {
 * .. Construction ..
 * + UserDatabase(db: shared_ptr<Database>, ui: UserInterface &)
 * __
 * + check(name: string const &, password_hash: array<char, 20> const &): int32_t
 * + create(name: string const &, password_hash: array<char, 20> const &)
 * + create(name: string const &, password: string const &)
 * + remove(name: string const &)
 * + get_instance(): UserDatabase &
 * __ attributes __
 * - database_: shared_ptr<Database>
 * - user_interface_: UserInterface &
 * - {static} instance_: UserDatabase *
 * }
 * @enduml
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
	 * @throws UserDatabaseAlreadyConstructedError If the constructor was already
	 *                                             called once. Use get_instance() instead.
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

	/**
	 * Obtain a reference to the one and only instance of this singleton.
	 *
	 * @throws UserDatabaseNotConstructedError If the constructor wasn't called yet.
	 * @return A reference to the implementation.
	 */
	static UserDatabase &get_instance();

private:
	//! A pointer to the underlying database for users.
	std::shared_ptr<Database> database_;
	//! A reference to the user interface, must stay valid until destruction of this instance.
	UserInterface &user_interface_;
	//! Store a pointer to this implementation.
	static UserDatabase *instance_;
};

#endif
