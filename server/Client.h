/**
	file: Client.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <cstdint> // uint32_t

class Client
{
	public:
		uint32_t	active_document;
		int			socket;
		uint32_t	user_id;

		Client(int socket, uint32_t user_id);
		~Client(void);

		char *receive(void);
		void send(char *bytes);
};

#endif
