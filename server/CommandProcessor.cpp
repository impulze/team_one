#include "CommandProcessor.h"
#include "UserDatabase.h"

#include <codecvt>
#include <functional>
#include <locale>
#include <sstream>
#include <vector>

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
	if (parameters.size() != 2)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: adduser <name> <password>");
	}

	user_interface_.printf("adding user: \"%ls\"\n", parameters[0]);

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string const nonwide_password = converter.to_bytes(parameters[1]);
	std::vector<char> const nonwide_password_bytes(nonwide_password.begin(), nonwide_password.end());
	auto const password_hash = user_database_.hash_bytes(nonwide_password_bytes);
	std::ostringstream password_hash_readable;

	for (auto const &ub: password_hash)
	{
		password_hash_readable
			<< std::hex
			<< ((ub & 0xf0) >> 4)
			<< ((ub & 0x0f) >> 0);
	}

	user_interface_.printf("added user: \"%ls\" [password hash: %s]\n", parameters[0], password_hash_readable.str());
}

void CommandProcessor::deluser(command_arguments_t const &parameters)
{
	if (parameters.size() != 1)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: deluser <name>");
	}

	user_interface_.printf("deleting user: \"%ls\"\n", parameters[0]);
}

void CommandProcessor::quit(command_arguments_t const &)
{
	// ignore parameters
	user_interface_.quit();
}
