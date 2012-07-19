#ifndef _CLIENT_TCC_
#define _CLIENT_TCC_

/**
 * @file Client.tcc
 * @author Maximilian Lasser <max.lasser@online.de>
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 */

#include <sys/socket.h>

#include "exceptions.h"
#include "NetworkInterface.h"

template<typename T>
void Client::receive(T *destination, size_t size)
{
	ssize_t received = recv(socket, destination, size, MSG_WAITALL);
	if (received == -1)
	{ throw Exception::ErrnoError("message reception failed", "recv"); }
	else if (static_cast<size_t>(received) < size)
	{
		if (received == 0)
		{
			throw Exception::SocketDisconnected("client disconnected", socket);
		}
		else
		{ throw Exception::SocketFailure("too less bytes received", socket); }
	}
}

#endif
