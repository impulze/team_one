/**
	file: Client.tcc
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 31st May 2012
**/

#ifndef _CLIENT_TCC_
#define _CLIENT_TCC_

#include <sys/socket.h>
#include "exceptions.h"

template<typename T>
void Client::receive(T *destination, size_t size) const
{
	ssize_t received = recv(socket, destination, size, MSG_WAITALL);
	if (received == -1)
	{ throw Exception::ErrnoError("message reception failed", "recv"); }
	else if (static_cast<size_t>(received) < size)
	{ throw Exception::SocketFailure("too less bytes received", socket); }
}

#endif
