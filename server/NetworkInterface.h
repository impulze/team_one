/**
	file: NetworkInterface.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#ifndef _NETWORKINTERFACE_H_
#define _NETWORKINTERFACE_H_

#include "ClientCollection.h"

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
		
		void run(void);
	
	private:
		ClientCollection	clients;
		int					listener;
};

#endif