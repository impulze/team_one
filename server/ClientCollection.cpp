/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#include "exceptions.h"
#include "Client.h"
#include "ClientCollection.h"

Client &ClientCollection::add_client(int socket)
{
	// check if there is already a client assigned to the given socket
	if (this->clients.find(socket) != this->clients.end())
	{ throw Exception::ClientAlreadyAdded("client already added"); }
	
	// create the client object and assign it to the given socket
	// FIXME: get user_id
	ClientSptr client(new Client(socket));
	this->clients[socket] = const_cast<ClientSptr&>(client);
	
	return *client;
}
