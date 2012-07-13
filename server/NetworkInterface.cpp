#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "exceptions.h"
#include "Client.h"
#include "NetworkInterface.h"

NetworkInterface *NetworkInterface::instance = NULL;

NetworkInterface &NetworkInterface::get_current_instance(void)
{
	// check if not yet instantiated
	if (instance == NULL)
	{ throw Exception::NotYetInstantiated("NetworkInterface not yet instantiated"); }

	return *instance;
}

NetworkInterface::NetworkInterface(int port, int backlog)
{
	// check if already instantiated
	if (instance != NULL)
	{ throw Exception::AlreadyInstantiated("only one NetworkInterface instance allowed"); }
	instance = this;

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

NetworkInterface::~NetworkInterface(void)
{ instance = NULL; }

void NetworkInterface::add_message_handler(const NetworkMessageHandler handler)
{
	message_handlers.push_front(handler);

	// send initialization message
	Message dummy_message;
	dummy_message.type = Message::MessageType::TYPE_INIT;
	handler(dummy_message);
}

void NetworkInterface::broadcast_message(const Message &message, int32_t document_id) const
{ message.send_to(clients, document_id); }

void NetworkInterface::disconnect_client(Client &client)
{
	Message dummy_message;
	dummy_message.type = Message::MessageType::TYPE_CLIENT_DISCONNECT;
	dummy_message.source = ClientSptr(&client);

	for (NetworkMessageHandler handler: message_handlers)
	{ handler(dummy_message); }

	clients.disconnect_client(client);
}

void NetworkInterface::remove_message_handler(const NetworkMessageHandler handler)
{
	message_handlers.remove(handler);

	// send exiting message
	Message dummy_message;
	dummy_message.type = Message::MessageType::TYPE_EXIT;
	handler(dummy_message);
}

void NetworkInterface::run(int ipc_socket)
{
	bool ipc_required = false;

	while (!ipc_required)
	{
		// generate fd_set
		fd_set set;
		FD_ZERO(&set);
		FD_SET(this->listener, &set);
		FD_SET(ipc_socket, &set);

		int end = std::max(this->listener, this->clients.fill_fd_set(&set));
		end = std::max(end, ipc_socket);
		end += 1;

		// select
		int selected_amount = select(end, &set, 0, 0, 0);
		if (selected_amount == -1)
		{
			throw Exception::ErrnoError("select failed", "select");
		}

		// check for incoming client connections
		if (selected_amount == 0)
		{ continue; }

		if (FD_ISSET(this->listener, &set))
		{
			this->clients.accept_client(this->listener);
			--selected_amount;
			/* Daniel -> Max:
			 * I added this here, because obviously you don't
			 * want to check for messages on the listening socket
			 * right?
			 */
			FD_CLR(this->listener, &set);
		}

		if (FD_ISSET(ipc_socket, &set))
		{
			/* the ipc socket doesn't care about messages, hence reduce
			 * the amount of messages required
			 * also clear it from the set so it won't be accessed
			 * by the message things below
			 */
			--selected_amount;
			FD_CLR(ipc_socket, &set);
			ipc_required = true;
		}

		// receive messages
		MessageList messages(selected_amount);
		this->clients.get_messages_by_fd_set(&set, end, messages);

		// process received messages
		for (const Message &message: messages)
		{
			// skip if message is empty or invalid
			if (message.is_empty() || message.type == Message::MessageType::TYPE_INVALID)
			{ continue; }

			// trigger events for all event handlers
			for (const NetworkMessageHandler &handler: message_handlers)
			{ handler(message); }
		}
	}
}

void NetworkInterface::update_client_cursors(int32_t start, int32_t addend,
	int32_t document_id)
{ clients.update_cursors(start, addend, document_id); }
