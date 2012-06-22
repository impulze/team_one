/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#include <sys/select.h>
#include "exceptions.h"
#include "Client.h"
#include "ClientCollection.h"

Client &ClientCollection::accept_client(int listener)
{
	ClientSptr client(new Client(listener));
	this->clients[client->socket] = client;
	
	return *client;
}

void ClientCollection::broadcast(const std::vector<char> &bytestream, int32_t document_id) const
{
	for (const std::pair<int, ClientSptr> &client: clients)
	{
		// TODO: add try-catch
		if (document_id == 0 || client.second->active_document == document_id)
		{ client.second->send(bytestream); }
	}
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

MessageList &ClientCollection::get_messages_by_fd_set(fd_set *set, int fd_max, MessageList &list)
{
	MessageList::iterator message = list.begin();

	// iterate through all fds in the set and handle the set ones
	for (int fd = 0; fd < fd_max; ++fd)
	{
		// ignore unset
		if (!FD_ISSET(fd, set))
		{ continue; }

		// read message from set
		message->receive_from(clients[fd]);

		// break if the list is full
		if (++message == list.end())
		{ break; }
	}

	return list;
}
