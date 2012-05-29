/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _CLIENTCOLLECTION_H_
#define _CLIENTCOLLECTION_H_

#include <memory>
#include <vector>
#include <unordered_map>

class Client;

typedef std::shared_ptr<Client> ClientSptr;

class ClientCollection
{
	public:
		/**
			Creates a new Client object and assigns it to the given socket.
				socket
			 =>	reference to the newly created Client
			 =#	Exception::ClientAlreadyAdded
					there is already a Client assigned to the given socket
		**/
		Client &add_client(int socket);
		
	private:
		std::unordered_map<int, ClientSptr> clients;
};

#endif