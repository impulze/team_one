/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

// TODO write getters and setters for Message data

#include "exceptions.h"
#include "NetworkInterface.h"
#include "NCursesUserInterface.h"
#include "SQLiteDatabase.h"
#include "UserDatabase.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>

class Message;
extern void main_network_message_handler(const Message &);

int main(int argc, char **argv)
{
	auto db = std::make_shared<SQLiteDatabase>(SQLiteDatabase::from_path("./db.sql"));
	UserInterface &ui = NCursesUserInterface::get_instance();
	UserDatabase user_db = UserDatabase(db, ui);

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

			ui.printf("adding user: <%ls>\n", strm.str().c_str());
		}

		void del(command_arguments_t const &parameters)
		{
			std::wostringstream strm;

			for (auto const &parameter: parameters)
			{
				strm << parameter << ' ';
			}

			ui.printf("adding user: <%ls>\n", strm.str().c_str());
		}

		void quit(command_arguments_t const &)
		{
			// ignore parameters
			wants_running = false;
		}

		UserInterface &ui;
		bool wants_running;
	} processor = { ui, true };

	using std::placeholders::_1;

	ui.register_processor(
		L"adduser",
		std::bind(&Processor::add, &processor, _1));
	ui.register_processor(
		L"deluser",
		std::bind(&Processor::del, &processor, _1));
	ui.register_processor(
		L"quit",
		std::bind(&Processor::quit, &processor, _1));

	int ipc_sockets[2];

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, ipc_sockets) == -1)
	{
		throw std::runtime_error("unable to create local communication sockets");
	}

	auto const network_thread_function = [&ui, &ipc_sockets]() {
		try
		{
			NetworkInterface network_interface(1337);

			network_interface.add_message_handler(&main_network_message_handler);
			network_interface.run(ipc_sockets[1]);
			ui.printf("network thread shutdown\n");
			return;
		}

		/* catch everything that doesn't require the main thread
		 * to exit, for now everything does :)
		 */
		catch (std::exception const &exception)
		{
			ui.printf("exception in network thread: %s\n", exception.what());
		}
		catch (...)
		{
			ui.printf("network exception: a network error occured\n");
		}

		ui.printf("restart process to get networking working again\n");
	};


	std::thread network_thread(network_thread_function);

	while (processor.wants_running)
	{
		try
		{
			ui.run();
		}
		catch (std::exception const &exception)
		{
			ui.printf("exception in main thread: %s\n", exception.what());
			ui.printf("exiting\n");
			break;
		}
	}

	if (::send(ipc_sockets[0], "quit", 5, 0) != 5)
	{
		throw std::runtime_error("unable to exit network thread gracefully");
	}

	network_thread.join();

	return EXIT_SUCCESS;
}
