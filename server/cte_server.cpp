/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

// TODO write getters and setters for Message data

#include "CommandProcessor.h"
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

namespace
{
	// this function is called as soon as the user interface is initialized
	int main_ui(int argc, char **argv, UserInterface &ui);
}

int main(int argc, char **argv)
{
	UserInterface &ui = NCursesUserInterface::get_instance();

	try
	{
		return main_ui(argc, argv, ui);
	}

	catch (std::exception const &exception)
	{
		ui.printf("exception during execution: %s\n", exception.what());
	}
	catch (...)
	{
		ui.printf("exception during execution");
	}

	ui.quit();

	return 1;
}

namespace
{
	int main_ui(int argc, char **argv, UserInterface &ui)
	{
		auto db = std::make_shared<SQLiteDatabase>(SQLiteDatabase::from_path("./db.sql"));
		UserDatabase user_db(db, ui);
		CommandProcessor command_processor(ui, user_db);
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
				ui.printf("network thread finished\n");
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
				ui.printf("exception in network thread\n");
			}

			ui.quit();
		};


		std::thread network_thread(network_thread_function);

		while (ui.still_running())
		{
			try
			{
				ui.run();
			}
			catch (std::exception const &exception)
			{
				ui.printf("exception in main thread: %s\n", exception.what());
				break;
			}
			catch (...)
			{
				ui.printf("exception in main thread\n");
				break;
			}
		}

		if (::send(ipc_sockets[0], "quit", 5, 0) != 5)
		{
			throw std::runtime_error("unable to exit network thread gracefully");
		}

		network_thread.join();

		return 0;
	}
}
