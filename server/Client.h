/**
	file: Client.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <cstdint>
#include <vector>

class Client
{
	public:
		int32_t		active_document;
		int32_t		cursor;
		const int	socket;
		int32_t		user_id;

		/*
			Uses the specified listening socket to accept a new incoming client connection.
			Therefore it uses the low-level function accept (sys/socket.h).
				listener - listening socket
			=#	Exception::ErrnoException - if accept fails
		*/
		Client(int listener);
		/*
			Closes the client's socket.
		*/
		~Client(void);

		template<typename T>
		void receive(T *destination, size_t size) const;
		void send(const std::vector<char> &bytes) const;
};

#include "Client.tcc"

#endif
