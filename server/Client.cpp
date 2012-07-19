/**
 * @file Client.cpp
 * @author Maximilian Lasser <max.lasser@online.de>
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 */

#include <sys/socket.h>
#include <unistd.h>

#include "Client.h"
#include "errno.h"
#include "exceptions.h"
#include "UserInterface.h"

extern UserInterface *g_user_interface;

Client::Client(int listener):
	active_document(0), cursor(0), socket(accept(listener, 0, 0)), user_id(0)
{
	g_user_interface->printf("new client\n");
	// check if a client was accepted
	if (this->socket == -1)
	{ throw Exception::ErrnoError("failed to accept a new client", errno, "accept"); }
}

Client::~Client(void)
{
	// close socket
	close(this->socket);
}

void Client::send(const std::vector<char> &bytes) const
{
	ssize_t sent = ::send(socket, bytes.data(), bytes.size(), 0);
	if (sent == -1)
	{ throw Exception::ErrnoError("message sending failed", "send"); }
	else if (static_cast<size_t>(sent) < bytes.size())
	{ throw Exception::SocketFailure("not all bytes have been sent", socket); }
}
