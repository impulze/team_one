#ifndef COMMANDPROCESSOR_H_INCLUDED
#define COMMANDPROCESSOR_H_INCLUDED

#include "UserInterface.h"

/**
 * @file CommandProcessor.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Interface and common symbols for command processing.
 */

class UserDatabase;

/**
 * This is an ecapsulation of the processed commands.
 * It's possible to add functions which get called if a
 * specific command is entered.
 * Remember to register those in the constructor or whereever
 * you please.
 */
class CommandProcessor
{
public:
	//! forward typedef
	typedef UserInterface::command_arguments_t command_arguments_t;

	/**
	 * Construct a command processor, storing a reference to the
	 * user interface and database.
	 *
	 * After construction the handler the callbacks are registered with the user
	 * interface input.
	 *
	 * If "adduser" is entered, adduser() is called.
	 * If "deluser" is entered, deluser() is called.
	 * If "quit" is entered, quit() is called.
	 * If "check_password" is entered, check_password() is called.
	 * If "check_password_hash" is entered, check_password_hash() is called.
	 *
	 * @param user_interface A reference to the user interface, make
	 *                       sure it stay valid until the command processor
	 *                       is destructor, otherwise behaviour is undefined.
	 * @param user_database A reference to the user interface, make
	 *                      sure it stay valid until the command processor
	 *                      is destructor, otherwise behaviour is undefined.
	 */
	CommandProcessor(UserInterface &user_interface,
	                 UserDatabase &user_database);

	/**
	 * A handler for adding users.
	 *
	 * @throws userinterface_errors::InvalidCommandError If arguments.size() != 2.
	 * @throws userinterface_errors::CommandFailedError If UserDatabase::create()
	 *                                                  throws a DatabaseFailure.
	 * @param arguments The arguments as parsed by UserInterface::process_line().
	 */
	void adduser(command_arguments_t const &arguments);

	/**
	 * A handler for deleting users.
	 *
	 * @throws userinterface_errors::InvalidCommandError If arguments.size() != 1.
	 * @throws userinterface_errors::CommandFailedError If UserDatabase::remove()
	 *                                                  throws a DatabaseFailure.
	 * @param arguments The arguments as parsed by UserInterface::process_line().
	 */
	void deluser(command_arguments_t const &arguments);

	/**
	 * A handler for checking passwords.
	 *
	 * @throws userinterface_errors::InvalidCommandError If arguments.size() != 2.
	 * @throws userinterface_errors::CommandFailedError If UserDatabase::check()
	 *                                                  throws a DatabaseFailure.
	 * @param arguments The arguments as parsed by UserInterface::process_line().
	 */
	void check_password(command_arguments_t const &arguments);

	/**
	 * A handler for checking password hashes.
	 *
	 * The hash should contain the 20 hexadecimal characters representing
	 * the SHA-1 bytes.
	 *
	 * @throws userinterface_errors::InvalidCommandError If arguments.size() != 2.
	 * @throws userinterface_errors::CommandFailedError If UserDatabase::check()
	 *                                                  throws a DatabaseFailure.
	 * @param arguments The arguments as parsed by UserInterface::process_line().
	 */
	void check_password_hash(command_arguments_t const &arguments);

	/**
	 * A handler for quitting the whole processing/interface.
	 *
	 * This calls UserInterface::quit() so check what exceptions are thrown
	 * by this method.
	 *
	 * @throws userinterface_errors::InvalidCommandError If arguments.size() != 0.
	 * @param arguments The arguments as parsed by UserInterface::process_line().
	 */
	void quit(command_arguments_t const &arguments);

private:
	//! A reference to the user interface, must stay valid until destruction of this instance.
	UserInterface &user_interface_;
	//! A reference to the user interface, must stay valid until destruction of this instance.
	UserDatabase &user_database_;
	/**
	 * The registered processors.
	 *
	 * In destruction or failed construction those processors are removed
	 * by calling UserInterface::unregister_processor().
	 */
	std::vector<UserInterface::command_processors_t::iterator> registered_processors_;
};

#endif
