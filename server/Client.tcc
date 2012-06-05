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
void Client::receive(T *destination, uint64_t size)
{
	uint64_t read_size = 0;
	while (read_size < size)
	{
		ssize_t read_tmp = recv(this->socket, destination + read_size, size - read_size, 0);
		if (read_tmp == -1)
		{ throw Exception::ErrnoError("message reception failed", "recv"); }
	}
}

#endif
