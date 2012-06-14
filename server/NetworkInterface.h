/**
	file: NetworkInterface.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#ifndef _NETWORKINTERFACE_H_
#define _NETWORKINTERFACE_H_

#include <forward_list>
#include <vector>

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
			Adds a message handler to this NetworkInterface. Each added handler will get called for
			each received Message.
			The given handler will be added to the list regardless of whether it's already there or
			not.
			The invocation order is the addition order reversed, i.e. the first added handler will
			be called last, vice versa and for the handlers in between analogously.
				handler
		**/
		void add_message_handler(const NetworkMessageHandler handler);
		/**
			Removes all occurrences of the specified handler from this' handler list.
				handler
		**/
		void remove_message_handler(const NetworkMessageHandler handler);		
		/**
			Main routine that looks for incoming client connections and messages and processes the
			latter as necessary.
			=#	Exception::ErrnoError - select failed
			=#	ClientCollection::add_client
		**/
		void run(int ipc_socket);
	
	private:
		ClientCollection							clients;
		int											listener;
		std::forward_list<NetworkMessageHandler>	message_handlers;
};

#endif
