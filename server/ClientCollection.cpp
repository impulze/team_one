/**
 * @file ClientCollection.cpp
 * @author Maximilian Lasser <max.lasser@online.de>
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 */

#include <sys/select.h>

#include "exceptions.h"
#include "Client.h"
#include "ClientCollection.h"
#include "UserInterface.h"

extern UserInterface *g_user_interface;

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
		if (document_id == 0 || client.second->active_document == document_id)
		{
			try
			{ client.second->send(bytestream); }
			catch (...)
			{}
		}
	}
}

void ClientCollection::disconnect_client(Client &client)
{
	g_user_interface->printf("[client %d] disconnecting\n", client.user_id);
	clients.erase(client.socket);
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
		// added by Daniel: this was formerly IN the client, causing a huge memory
		// corruption
		try
		{
			message->receive_from(clients[fd]);
		}
		catch (Exception::SocketDisconnected const &ex)
		{
			disconnect_client(*clients[fd]);
		}

		// break if the list is full
		if (++message == list.end())
		{ break; }
	}

	return list;
}

void ClientCollection::update_cursors(int32_t start, int32_t addend, int32_t document_id)
{
	// iterate through all clients
	for (std::pair<int, ClientSptr> pair: clients)
	{
		ClientSptr &client = pair.second;

		// continue if client is not affected
		if (client->active_document != document_id || client->cursor < start)
		{ continue; }

		// update cursor
		client->cursor += addend;
	}
}
