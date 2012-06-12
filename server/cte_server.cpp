/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

#include "NetworkInterface.h"
#include "SQLiteDatabase.h"
#include "NCursesUserInterface.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{
	NetworkInterface network_interface(1337);
	
	// FIXME: add_event_handler doesn't exist
	//network_interface.add_event_handler(&main_handler);
	
	// FIXME: no listen() in NetworkInterface
#if 0
	while (1)
	{ network_interface.listen(); }
#endif

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
