/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

// TODO write getters and setters for Message data

#include "NetworkInterface.h"
#include "SQLiteDatabase.h"
#include "NCursesUserInterface.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

class Message;
extern void main_network_message_handler(const Message &);

int main(int argc, char **argv)
{
	NetworkInterface network_interface(1337);
	
	network_interface.add_message_handler(&main_network_message_handler);
	
	while (1)
	{ network_interface.run(); }

	SQLiteDatabase sqlite_db = SQLiteDatabase::from_path("./db.sql");

	NCursesUserInterface &ncurses_ui = NCursesUserInterface::get_instance();

	typedef NCursesUserInterface::command_arguments_t command_arguments_t;

	struct Processor
	{
		void add(command_arguments_t const &parameters)
		{
			std::wostringstream strm;

			for (auto const &parameter: parameters)
			{
				strm << parameter << ' ';
			}

			ncurses_ui.printf("adding user: <%ls>\n", strm.str().c_str());
		}

		void del(command_arguments_t const &parameters)
		{
			std::wostringstream strm;

			for (auto const &parameter: parameters)
			{
				strm << parameter << ' ';
			}

			ncurses_ui.printf("adding user: <%ls>\n", strm.str().c_str());
		}

		void quit(command_arguments_t const &)
		{
			// ignore parameters
			wants_running = false;
		}

		NCursesUserInterface &ncurses_ui;
		bool wants_running;
	} processor = { ncurses_ui, true };

	using std::placeholders::_1;

	ncurses_ui.register_processor(
		L"adduser",
		std::bind(&Processor::add, &processor, _1));
	ncurses_ui.register_processor(
		L"deluser",
		std::bind(&Processor::del, &processor, _1));
	ncurses_ui.register_processor(
		L"quit",
		std::bind(&Processor::quit, &processor, _1));

	while (processor.wants_running)
	{
		ncurses_ui.run();
	}

	return EXIT_SUCCESS;
}
