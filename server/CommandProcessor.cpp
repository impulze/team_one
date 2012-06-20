#include "CommandProcessor.h"
#include "UserDatabase.h"

#include <functional>
#include <sstream>

CommandProcessor::CommandProcessor(UserInterface &user_interface,
                                   UserDatabase &user_database)
	: user_interface_(user_interface),
	  user_database_(user_database)
{
	using std::placeholders::_1;

	registered_processors_.push_back(
		user_interface_.register_processor(
			L"adduser",
			std::bind(&CommandProcessor::adduser, this, _1)));
	registered_processors_.push_back(
		user_interface_.register_processor(
			L"deluser",
			std::bind(&CommandProcessor::deluser, this, _1)));
	registered_processors_.push_back(
		user_interface_.register_processor(
			L"quit",
			std::bind(&CommandProcessor::quit, this, _1)));
}

void CommandProcessor::adduser(command_arguments_t const &parameters)
{
	std::wostringstream strm;

	for (auto const &parameter: parameters)
	{
		strm << parameter << ' ';
	}

	user_interface_.printf("adding user: <%ls>\n", strm.str());
}

void CommandProcessor::deluser(command_arguments_t const &parameters)
{
	std::wostringstream strm;

	for (auto const &parameter: parameters)
	{
		strm << parameter << ' ';
	}

	user_interface_.printf("adding user: <%ls>\n", strm.str());
}

void CommandProcessor::quit(command_arguments_t const &)
{
	// ignore parameters
	user_interface_.quit();
}
