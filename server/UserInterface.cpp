#include "UserInterface.h"

#include <algorithm>
#include <sstream>

/**
 * @file server/UserInterface.cpp
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

	CommandFailedError::CommandFailedError(std::string const &message)
		: Failure(message)
	{
	}
}

UserInterface::UserInterface()
	: quit_requested_(false)
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
	quit_requested_ = true;
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
			command_arguments_t args;

			if (input_end != std::wstring::npos)
			{
				// there are arguments
				args = parse_arguments(current_line_.substr(input_end + 1));
			}

			{
				std::wostringstream strm;

				for (auto it = args.begin(); it != args.end(); ++it)
				{
					strm << '"' << *it << '"';

					if (it + 1 != args.end())
					{
						strm << ',';
					}
				}

				printf("command <%ls> with arguments <%ls> entered\n", command, strm.str());
			}

			try
			{
				processing.second(args);
			}
			catch (userinterface_errors::InvalidCommandError const &error)
			{
				printf("invalid command\n%s\n", error.what());
			}
			catch (userinterface_errors::CommandFailedError const &error)
			{
				printf("command failed\n%s\n", error.what());
			}
		}
	}
}

UserInterface::command_arguments_t
UserInterface::parse_arguments(std::wstring const &string)
{
	command_arguments_t command_arguments;
	std::wstring current_argument;

	if (string.empty())
	{
		return command_arguments;
	}

	for (auto it = string.begin(); it != string.end(); ++it)
	{
		if (*it == L' ')
		{
			if (it == string.begin())
			{
				command_arguments.push_back(L"");
			}
			else if (*(it - 1) == L'\\')
			{
				current_argument.back() = *it;
			}
			else
			{
				command_arguments.push_back(current_argument);
				current_argument.clear();
			}
		}
		else
		{
			current_argument.push_back(*it);
		}
	}

	command_arguments.push_back(current_argument);

	return command_arguments;
}
