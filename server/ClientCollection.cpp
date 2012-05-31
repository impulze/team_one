/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#include "exceptions.h"
#include "Client.h"
#include "ClientCollection.h"

Client &ClientCollection::add_client(int listener)
{
	ClientSptr client(new Client(listener));
	this->clients[client->socket] = client;
	
	return *client;
}

int ClientCollection::fill_fd_set(fd_set *set) const
{
	int end = 0;
	
	for (const std::pair<int, ClientSptr> &client: this->clients)
	{
		FD_SET(client.first, set);
		end = std::max(end, client.first);
	}
	
	return end;
}