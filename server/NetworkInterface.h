/**
	file: NetworkInterface.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#ifndef _NETWORKINTERFACE_H_
#define _NETWORKINTERFACE_H_

#include <forward_list>

#include "ClientCollection.h"

typedef void (*NetworkMessageHandler)(const Message &);

class NetworkInterface
{
	public:
		/**
			Standard constructor.
			Creates and binds a listening socket and sets it to listening state.
				 port - port to bind on
				*backlog -> <sys/socket.h> listen(backlog)
			=#	Exception::ErrnoError - listening socket creation failed
			=#	Exception::ErrnoError - network address structure generation failed
			=#	Exception::ErrnoError - listening socket binding failed
			=#	Exception::ErrnoError - listening failed
		**/
		NetworkInterface(int port, int backlog = 4);
		
		/**
			Main routine that looks for incoming client connections and messages and processes the
			latter as necessary.
			=#	Exception::ErrnoError - select failed
			=#	ClientCollection::add_client
		**/
		void run(void);
	
	private:
		ClientCollection							clients;
		int											listener;
		std::forward_list<NetworkMessageHandler>	message_handlers;
};

#endif
