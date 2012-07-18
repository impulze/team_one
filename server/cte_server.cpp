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

/**
 * @file server/cte_server.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 * @author Maximilian Lasser <max.lasser@online.de>
 *
 * The application main loop and instantiation code.
 */

class Message;
extern void main_network_message_handler(const Message &);

/**
 * The entry point for the server application binary.
 *
 * @startuml{main_Communication.svg}
 *
 * actor User
 *
 * == Initialisation ==
 *
 * User -> Application: main()
 * activate Application
 *
 * Application -> UserInterface: <b>NCurses</b>UserInterface()
 * activate UserInterface
 * UserInterface --> Application: << <font color="green">ui</font> >>
 *
 * Application -> Database: <b>SQLite</b>Database("users.sql")
 * activate Database
 * Database --> Application: << <font color="red">db</font> >>
 *
 * Application -> UserDatabase: UserDatabase(<font color="green">ui</font>, <font color="red">db</font>)
 * activate UserDatabase
 * UserDatabase --> Application: << <font color="#FF6600">user_db</font> >>
 *
 * Application -> CommandProcessor: CommandProcessor(<font color="green">ui</font>, <font color="#FF6600">user_db</font>)
 * activate CommandProcessor
 *
 * Application -> CommunicationPipe
 * activate CommunicationPipe
 *
 * CommunicationPipe --> Application: << <font color="#3333FF">read_pipe</font>, <font color="#FF0099">write_pipe</font> >>
 *
 * Application -> NetworkThread: NetworkThread(<font color="#3333FF">read_pipe</font>)
 * activate NetworkThread
 *
 * NetworkThread -> NetworkInterface: NetworkInterface(port, message_handler)
 * activate NetworkInterface
 *
 * UserInterface -> User: wait()
 * User -> UserInterface: << some input >>
 *
 * NetworkThread -> NetworkInterface: run(<font color="#3333FF">read_pipe</font>)
 *
 * NetworkInterface -> OS: select(sockets..., <font color="#3333FF">read_pipe</font>)
 * NetworkInterface --> NetworkThread
 * destroy NetworkInterface
 *
 * NetworkThread --> Application
 * destroy NetworkThread
 *
 * Application -> UserInterface: quit()
 * note right
 *  this will set an
 *  internal flag, so the
 *  user interface can quit
 * end note
 *
 * UserInterface -> User: wait()
 * destroy UserInterface
 *
 * @enduml
 *
 * @param argc The amount of arguments passed to the program + 1.
 * @param argv An array of argument strings passed to the program. The first
 *             argument is the name of the binary which was executed.
 * @returns 0 On success, non-zero otherwise.
 */
int main(int argc, char **argv)
{
	typedef UserInterfaceSingleton<NCursesUserInterface> NCursesUserInterfaceSingleton;

	auto &ui = NCursesUserInterfaceSingleton::get_instance();
	auto db = std::make_shared<SQLiteDatabase>(SQLiteDatabase::from_path("./user.sql"));
	UserDatabase user_db(db, ui);
	CommandProcessor command_processor(ui, user_db);
	int ipc_sockets[2];

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, ipc_sockets) == -1)
	{
		throw std::runtime_error("unable to create local communication sockets");
	}

	auto const network_thread_function = [&ui, &ipc_sockets]()
	{
		try
		{
			NetworkInterface network_interface(1337);

			network_interface.add_message_handler(&main_network_message_handler);
			network_interface.run(ipc_sockets[1]);
			ui.printf("network thread finished\n");
			return;
		}
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

	auto const network_thread_canceler = [&network_thread, &ipc_sockets]()
	{
		if (::send(ipc_sockets[0], "quit", 5, 0) != 5)
		{
			throw std::runtime_error("unable to exit network thread gracefully");
		}

		network_thread.join();
	};

	try
	{
		ui.run();
		network_thread_canceler();
		return 0;
	}
	catch (std::exception const &exception)
	{
		ui.printf("exception in main thread: %s\n", exception.what());
	}
	catch (...)
	{
		ui.printf("exception in main thread\n");
	}

	ui.quit();
	network_thread_canceler();

	return 1;
}
