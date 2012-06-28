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
	registered_processors_.push_back(
		user_interface_.register_processor(
			L"check_password",
			std::bind(&CommandProcessor::check_password, this, _1)));
	registered_processors_.push_back(
		user_interface_.register_processor(
			L"check_password_hash",
			std::bind(&CommandProcessor::check_password_hash, this, _1)));
}

void CommandProcessor::adduser(command_arguments_t const &arguments)
{
	if (arguments.size() != 2)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: adduser <name> <password>");
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string const nonwide_name = converter.to_bytes(arguments[0]);
	std::string const nonwide_password = converter.to_bytes(arguments[1]);

	try
	{
		user_database_.create(nonwide_name, nonwide_password);
	}
	catch (database_errors::Failure const &error)
	{
		throw userinterface_errors::CommandFailedError(error.what());
	}
}

void CommandProcessor::deluser(command_arguments_t const &arguments)
{
	if (arguments.size() != 1)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: deluser <name>");
	}


	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string const nonwide_name = converter.to_bytes(arguments[0]);

	try
	{
		user_database_.remove(nonwide_name);
	}
	catch (database_errors::Failure const &error)
	{
		throw userinterface_errors::CommandFailedError(error.what());
	}
}

void CommandProcessor::check_password(command_arguments_t const &arguments)
{
	if (arguments.size() != 2)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: check <name> <password>");
	}


	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string const nonwide_name = converter.to_bytes(arguments[0]);
	std::string const nonwide_password = converter.to_bytes(arguments[1]);

	try
	{
		std::vector<char> hash_bytes(nonwide_password.begin(), nonwide_password.end());
		Hash::hash_t const hash = Hash::hash_bytes(hash_bytes);
		user_database_.check(nonwide_name, hash);
	}
	catch (database_errors::Failure const &error)
	{
		throw userinterface_errors::CommandFailedError(error.what());
	}
}

void CommandProcessor::check_password_hash(command_arguments_t const &arguments)
{
	if (arguments.size() != 2)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: check <name> <password_hash>");
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string const nonwide_name = converter.to_bytes(arguments[0]);
	std::string const nonwide_password_hash = converter.to_bytes(arguments[1]);

	try
	{
		Hash::hash_t const hash = Hash::string_to_hash(nonwide_password_hash);
		user_database_.check(nonwide_name, hash);
	}
	catch (database_errors::Failure const &error)
	{
		throw userinterface_errors::CommandFailedError(error.what());
	}
}

void CommandProcessor::quit(command_arguments_t const &arguments)
{
	if (arguments.size() != 0)
	{
		throw userinterface_errors::InvalidCommandError("Syntax: quit");
	}

	user_interface_.quit();
}
