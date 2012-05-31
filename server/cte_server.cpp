/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

#include "NetworkInterface.h"
#include "SQLiteDatabase.h"

#include <cstdlib>

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

	SQLiteDatabase sqlite_db = SQLiteDatabase::from_path("db.sql");

	return EXIT_SUCCESS;
}
