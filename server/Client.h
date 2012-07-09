/**	@file Client.h

	@author Maximilian Lasser <max.lasser@online.de>
	@date Friday, 11th May 2012
**/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <cstdint>
#include <vector>

/**
	@brief The Client class wraps a connected client.
**/
class Client
{
	public:
		int32_t		active_document; ///< the client's active document's id
		int32_t		cursor; ///< the client's current cursor position in the active document
		const int	socket; ///< the client's socket
		int32_t		user_id; ///< the client's user id, if logged in

		/**
			Uses the specified listening socket to accept a new incoming client connection.
			Therefore it uses the low-level function accept.
			
			@param listener the listening socket
			
			@exception Exception::ErrnoException if accept (sys/socket.h) failed
		**/
		Client(int listener);
		/**
			Closes the client's socket.
		**/
		~Client(void);

		/**
			Receives the specified amount of bytes from the client and stores them at the given
			destination.
			The method will wait until the requested amount of bytes got received.

			@param destination a pointer to allocated memory big enough for the amount of requested
				bytes
			@param size the amount of bytes to receive

			@exception Exception::ErrnoError if recv (sys/socket.h) failed
			@exception Exception::SocketFailure if less bytes than requested were received
		**/
		template<typename T>
		void receive(T *destination, size_t size) const;
		/**
			Sends the given bytestream to the client.

			@param bytes a reference to a vector containing the bytes to send

			@exception Exception::ErrnoError if send (sys/socket.h) failed
			@exception Exception::SocketFailure if less bytes than required were sent
		**/
		void send(const std::vector<char> &bytes) const;
};

#include "Client.tcc"

#endif
