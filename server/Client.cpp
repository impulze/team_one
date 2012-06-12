/**
	file: Client.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#include <unistd.h> // close
#include <sys/socket.h>
#include "Client.h"
#include "errno.h"
#include "exceptions.h"

Client::Client(int listener):
	active_document(0), cursor(0), socket(accept(listener, 0, 0)), user_id(0)
{
	// check if a client was accepted
	if (this->socket == -1)
	{ throw Exception::ErrnoError("failed to accept a new client", errno, "accept"); }
}

Client::~Client(void)
{
	// close socket
	close(this->socket);
}

void Client::send(const std::vector<char> &bytes) const
{}
