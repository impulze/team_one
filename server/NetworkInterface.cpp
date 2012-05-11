/**
 * file: NetworkInterface.cpp
 * author: Maximilian Lasser [max.lasser@online.de]
 * created: Friday, 11th May 2012
 **/

#include "NetworkInterface.h"

NetworkInterface::NetworkInterface(int port)
{
	// create a socket for listening
	this.listener = socket(AF_INET, SOCK_STREAM, 0);
	// TODO: check for -1
	
	// generate listening socket address structure and bind
	struct sockaddr_in socket_address =
	{
		.sin_family = AF_INET,
		.sin_port = port
	};
	inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr);
	// TODO: check for errors
	bind(this.listener, &socket_address, sizeof(socket_address));
	// TODO: check for errors
}