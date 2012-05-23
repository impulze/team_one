/**
	file: cte_server.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

#include <cstdlib>

int main(int argc, char **argv)
{
	MainNetworkEventHandler main_handler;
	NetworkInterface network_interface(1337);
	
	network_interface.add_event_handler(&main_handler);
	
	while (1)
	{ network_interface.listen(); }
	
	return EXIT_SUCCESS;
}