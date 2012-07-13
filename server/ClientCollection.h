/** @file ClientCollection.h

	@author Maximilian Lasser <max.lasser@online.de>
	@date Thursday, 24th May 2012
**/

#ifndef _CLIENTCOLLECTION_H_
#define _CLIENTCOLLECTION_H_

#include <forward_list>
#include <memory>
#include <sys/select.h>
#include <unordered_map>

class Client;
class Message;

typedef std::shared_ptr<Client> ClientSptr; ///< abbreviation for a Client shared_ptr
typedef std::forward_list<Message> MessageList; ///< abbreviation for a Message forward_list

/**
	@brief Loose collection of Client objects with various useful methods.

	A ClientCollection may hold an arbitrary number of Client objects. It provides methods for
	accepting a new client from a listener socket, broadcasting messages, disconnecting single
	clients and various auxiliary functions.
**/
class ClientCollection
{
	public:
		/**
			Creates a new Client object and adds it to the map.

			@param listener the (listener) socket to accept the client connection on
			@return a reference to the newly created Client object

			@note Calls Client::Client(int) without catching any exceptions.
			@see Client::Client(int)
		**/
		Client &accept_client(int listener);

		/**
			Sends the given bytestream to all clients in this ClientCollection.
			
			@param bytestream a reference to the byte vector containing the bytes to send
			@param document_id an optional constraint causing the bytestream to be sent only to all
				clients whose active document id is equal to the valueof this argument; the default
				is 0, which means that the bytestream should be sent to all clients

			@note Calls Client::send(const std::vector<char>) without catching any exceptions.
			@see Client::send(const std::vector<char>)
		**/
		void broadcast(const std::vector<char> &bytestream, int32_t document_id = 0) const;

		/**
			Disconnects a client and removes the respective Client object from this ClientCollection
			and thus the whole memory.

			@param client a reference to the Client object to disconnect and remove
		**/
		void disconnect_client(Client &client);

		/**
			Adds all clients' sockets to the given fd_set using the makro FD_SET.
			
			@param set a pointer to the fd_set to add the sockets to
			@return the socket id with the highest integral value of all added ones
		**/
		int fill_fd_set(fd_set *set) const;
		/**
			Collects the oldest unread message in the queue from each socket that's set as readable
			in the fd_set. Each of those has to be one of a currently connected Client. Stores all
			received messages in the given MessageList.

			@param set a pointer to the fd_set containing the readable sockets
			@param fd_max highest of all sockets' integral values plus 1
			@param dest a reference to the MessageList to fill
			@return a reference to the filled MessageList

			@note Calls Message::receive_from(ClientSptr) without catching any exceptions.
			@see Message::receive_from(ClientSptr)
		**/
		MessageList &get_messages_by_fd_set(fd_set *set, int fd_max, MessageList &dest);
		/**
			Updates the cursor positions of all clients with the specified document as current
			active one by adding the addend to them, but only if their cursor position is greater
			than or equal to start.
			@param start smallest affected cursor position; all cursors smaller than this value
				won't be affected
			@param addend value to add to the cursor positions; may be negative
			@param document_id the document id of the affected document
		**/
		void update_cursors(int32_t start, int32_t addend, int32_t document_id);
		
	private:
		std::unordered_map<int, ClientSptr> clients; ///< maps sockets onto Client object pointers
};

#endif
