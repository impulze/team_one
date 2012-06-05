/**
	file: NetworkInterface.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "exceptions.h"
#include "NetworkInterface.h"

NetworkInterface::NetworkInterface(int port, int backlog)
{
	// create a socket for listening
	this->listener = socket(AF_INET, SOCK_STREAM, 0);
	if (this->listener == -1)
	{ throw Exception::ErrnoError("failed to create listening socket", "socket"); }
	
	// generate listening socket address structure and bind
	struct sockaddr_in socket_address =
	{
		.sin_family = AF_INET,
		.sin_port = htons(port)
	};
	if (inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr) == -1)
	{ throw Exception::ErrnoError("failed to generate network address structure", "inet_pton"); }
	
	// bind listener
	if (bind(this->listener, reinterpret_cast<sockaddr *>(&socket_address), sizeof(socket_address)) == -1)
	{ throw Exception::ErrnoError("failed to bind listening socket", "bind"); }
	
	// listen
	if (listen(this->listener, backlog) == -1)
	{ throw Exception::ErrnoError("failed to listen", "listen"); }
}

void NetworkInterface::run(void)
{
	// generate fd_set
	fd_set set;
	FD_ZERO(&set);
	FD_SET(this->listener, &set);
	int end = std::max(this->listener, this->clients.fill_fd_set(&set)) + 1;
	
	// select
	int selected_amount = select(end, &set, 0, 0, 0);
	if (selected_amount == -1)
	{ throw Exception::ErrnoError("select failed", "select"); }
	
	// check for incoming client connections
	if (selected_amount == 0)
	{ return; }
	if (FD_ISSET(this->listener, &set))
	{
		this->clients.accept_client(this->listener);
		--selected_amount;
	}
	
	// receive messages
	MessageList messages(selected_amount);
	this->clients.get_messages_by_fd_set(&set, end, messages);

	// process received messages
	for (const Message &message: messages)
	{
		// skip if message is empty or invalid
		if (message.source == 0 || message.type == Message::TYPE_INVALID)
		{ continue; }
		
		// trigger events for all event handlers
		for (const NetworkMessageHandler &handler: message_handlers)
		{ handler(message); }
	}
}
