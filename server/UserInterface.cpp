#include "UserInterface.h"

#include <algorithm>
#include <sstream>

/**
 * @file UserInterface.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for common abstractions of user interface things.
 */

namespace userinterface_errors
{
	Failure::Failure(std::string const &message)
		: std::runtime_error("user interface error: " + message)
	{
	}

	InvalidCommandError::InvalidCommandError(std::string const &message)
		: Failure(message)
	{
	}
}

UserInterface::UserInterface()
	: still_running_(true)
{
}

UserInterface::~UserInterface()
{
}

UserInterface::command_processors_t::iterator
UserInterface::register_processor(std::wstring const &command,
                                  command_processor_t const &function)
{
	// commands are non spaced strings
	if (command.find_first_of(L" \v\t\n\r") != std::wstring::npos)
	{
		std::ostringstream strm;
		std::string command_nonwide;
		std::copy(command.begin(), command.end(), std::back_inserter(command_nonwide));

		strm << '<' << command_nonwide << "> contains whitespace";

		throw userinterface_errors::InvalidCommandError(strm.str());
	}

	return command_processors_.insert(command_processors_.end(), {command, function});
}

void UserInterface::unregister_processor(command_processors_t::iterator iterator)
{
	command_processors_.erase(iterator);
}

void UserInterface::quit()
{
	quit_mutex_.lock();

	try
	{
		printf("Press any key to quit.");
		wait_for_key();
		still_running_ = false;
	}
	catch (...)
	{
		quit_mutex_.unlock();
		throw;
	}

	quit_mutex_.unlock();
}

void UserInterface::process_line()
{
	for (auto const &processing: command_processors_)
	{
		// check if the command matches, commands are non spaced strings
		auto const &command = processing.first;

		auto const input_end = current_line_.find(L' ');

		if (command == current_line_.substr(0, input_end))
		{
			printf("command <%ls> entered\n", command.c_str());
			command_arguments_t args;
			processing.second(args);
		}
	}
}
