/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

// TODO write getters and setters for Message data

#include "NetworkInterface.h"
#include "SQLiteDatabase.h"

#include <cstdlib>

class Message;
extern void main_network_message_handler(const Message &);

int main(int argc, char **argv)
{
	NetworkInterface network_interface(1337);
	
	network_interface.add_message_handler(&main_network_message_handler);
	
	while (1)
	{ network_interface.run(); }

	SQLiteDatabase sqlite_db = SQLiteDatabase::from_path("db.sql");

	return EXIT_SUCCESS;
}
