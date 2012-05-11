/**
	file: Client.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#include <unistd.h> // close
#include "Client.h"

Client::Client(int socket, uint32_t user_id)
{
	this->socket = socket;
	this->user_id = user_id;
}

Client::~Client(void)
{
	// close socket
	close(this->socket);
}
