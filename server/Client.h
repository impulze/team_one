/**
	file: Client.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <cstdint> // uint32_t
#include <vector>

class Client
{
	public:
		uint32_t	active_document;
		uint64_t	cursor;
		const int	socket;
		uint32_t	user_id;

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
		void receive(T *destination, uint64_t size) const;
		void send(const std::vector<char> &bytes) const;
};

#include "Client.tcc"

#endif
