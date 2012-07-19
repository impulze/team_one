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

//! TODO: quite dirty way to share the user interface
UserInterface *g_user_interface;

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
 * == Communication ==
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
 * UserInterface -> User: << print quit message, ignore input >>
 * destroy UserInterface
 *
 * @enduml
 *
 * @startuml{Database_Interaction.svg}
 *
 * class Database << abstract >> {
 * .. Construction ..
 * + Database()
 * + ~Database()
 * .. Deleted ..
 * + Database(db: Database const &)
 * + operator=(db: Database const &): Database &
 * __ implemented __
 * << templated >>
 * + execute_sql(statement: string const &, args: ...): vector<unordered_map<string, string>>
 * __ abstract __
 * + complete_sql(statement: string const &) const: bool
 * + execute_sqlv(statement: char const *, args: ...): vector<unordered_map<string, string>>
 * }
 *
 * Database <|-- SQLiteDatabase
 * class SQLiteDatabase {
 * .. Construction ..
 * + SQLiteDatabase(SQLiteDatabase &&)
 * + ~SQLiteDatabase()
 * - SQLiteDatabase()
 * + {static} from_path(path: string const &): SQLiteDatabase
 * + {static} temporary(): SQLiteDatabase
 * __
 * + complete_sql(statement: string const &) const: bool
 * + execute_sqlv(statement: char const *): vector<unordered_map<string, string>>
 * }
 *
 * Database "1" -- "1" UserDatabase: > provides implementation
 * class UserDatabase << singleton >> {
 * .. Construction ..
 * + UserDatabase(db: shared_ptr<Database>, ui: UserInterface &)
 * __
 * + check(name: string const &, password_hash: array<char, 20> const &): int32_t
 * + create(name: string const &, password_hash: array<char, 20> const &)
 * + create(name: string const &, password: string const &)
 * + remove(name: string const &)
 * + get_instance(): UserDatabase &
 * __ attributes __
 * - database_: shared_ptr<Database>
 * - user_interface_: UserInterface &
 * - {static} instance_: UserDatabase *
 * }
i * @enduml
 *
 * @startuml{UserInterface_Interaction.svg}
 * note as N1
 *  typedef function<void(vector<wstring> const &)> command_processor_t
 *  typedef unordered_multimap<wstring, command_processor_t> command_processors_t
 * end note
 *
 * UserInterface <|-- NCursesUserInterface
 * class NCursesUserInterface << abstract >> {
 * .. Construction ..
 * + NCursesUserInterface()
 * + ~NCursesUserInterface()
 * __
 * + run()
 * + deinitialize();
 * - printfv(format: char const *, ...)
 * __ attributes __
 * deinitialized_: bool
 * screen_: SCREEN *
 * input_window_: WINDOW *
 * current_position_: wstring::size_type
 * printf_mutex_: mutex
 * }
 *
 * class UserInterface << abstract >> {
 * .. Construction ..
 * + UserInterface()
 * + ~UserInterface()
 * .. Deleted ..
 * + UserInterface(ui: UserInterface const &)
 * + operator=(ui: UserInterface const &): Database &
 * __ implemented __
 * + register_processor(command: wstring const &, function: command_processor_t): iterator
 * + unregister_processor(iterator)
 * << templated >>
 * + printf(format: string, args: ...)
 * + quit()
 * ~ process_line()
 * __ abstract __
 * + run()
 * - printfv(format: char const *, args: ...)
 * - {static} parse_arguments(string: wstring): vector<wstring>
 * __ attributes __
 * ~ quit_requested_: bool
 * ~ current_line_: wstring
 * - command_processors_: command_processors_t
 * - quit_mutex_: mutex
 * }
 *
 * CommandProcessor "1..*" -- "1" UserInterface
 * class CommandProcessor {
 * .. Construction ..
 * + CommandProcessor(ui: UserInterface &, db: UserDatabase &)
 * + ~CommandProcessor()
 * + adduser(vector<wstring> const &)
 * + deluser(vector<wstring> const &)
 * + check_password(vector<wstring> const &)
 * + check_password_hash(vector<wstring> const &)
 * + quit(vector<wstring> const &)
 * __ attributes __
 * user_interface_: UserInterface &
 * user_database_: UserDatabase &
 * registered_processors_: vector<command_processors_t>
 * }
 *
 * N1 .. UserInterface
 * N1 .. CommandProcessor
 * @enduml
 *
 * @startuml{Document_Interaction.svg}
 * class Document {
 * .. Construction ..
 * + Document(Document &&)
 * + ~Document()
 * - Document(fd: int, name: string const &)
 * .. Deleted ..
 * + Document(Document const &)
 * + operator=(Document const &): Document &
 * __
 * + {static} create(name: string, overwrite: bool): Document
 * + {static} open(name: string): Document
 * + {static} is_empty(name: string): bool
 * + {static} list_documents(): vector<string>
 * + remove()
 * + save()
 * + close()
 * + hash(): array<char, 20>
 * + get_contents(): vector<char>
 * + get_name(): string
 * + get_id(): int32_t
 * .. helpers ..
 * - {static} open_readable(name: string): int
 * - {static} open_writable(name: string, overwrite: bool): int
 * - {static} increment_global_document_id()
 * __ attributes __
 * - contents_: vector<char>
 * - fd_: int
 * - name_: string const
 * - {static} directory_: string const
 * - {static} global_document_id_: int32_t
 * - id_: int32_t
 * - document_closed_: bool
 * - contents_fetched_: bool
 * }
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
	g_user_interface = &ui;
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
