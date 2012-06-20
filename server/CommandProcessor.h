#ifndef COMMANDPROCESSOR_H_INCLUDED
#define COMMANDPROCESSOR_H_INCLUDED

#include "UserInterface.h"

/**
 * @file CommandProcessor.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * This is an ecapsulation of the processed commands.
 * It's possible to add functions which get called if a
 * specific command is entered.
 * Remember to register those in the constructor or whereever
 * you please.
 */

class UserDatabase;

class CommandProcessor
{
public:
	typedef UserInterface::command_arguments_t command_arguments_t;

	CommandProcessor(UserInterface &user_interface,
	                 UserDatabase &user_database);

	void adduser(command_arguments_t const &);
	void deluser(command_arguments_t const &);
	void quit(command_arguments_t const &);

	bool still_running() const
	{
		return still_running_;
	}

private:
	UserInterface &user_interface_;
	UserDatabase &user_database_;
	bool still_running_;
	std::vector<UserInterface::command_processors_t::iterator> registered_processors_;
};

#endif
