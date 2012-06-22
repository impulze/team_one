/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _CLIENTCOLLECTION_H_
#define _CLIENTCOLLECTION_H_

#include <forward_list>
#include <memory>
#include <sys/select.h>
#include <unordered_map>

class Client;
class Message;

typedef std::shared_ptr<Client> ClientSptr;
typedef std::forward_list<Message> MessageList;

class ClientCollection
{
	public:
		/**
			Creates a new Client object and adds it to the map.
				listener
			=>	reference to the newly created Client
			=#	Client::Client
		**/
		Client &accept_client(int listener);
		/**
			Sends the given bytestream to all clients of this ClientCollection.
				bytestream
				document_id [#]
					if this is not 0 the bytestream will only be sent to all clients that have the
					document with this id activated
			=#	Client::send(std::vector<char>)
		**/
		void broadcast(const std::vector<char> &bytestream, int32_t document_id = 0) const;
		/**
			Adds all clients' sockets to the given fd_set using the makro FD_SET.
				set
			=>	client socket with the highest integral value of all added sockets
		**/
		int fill_fd_set(fd_set *set) const;
		/**
			Collects the oldest unread message in the queue from each socket that's set as readable
			in the fd_set. Each of those has to be one of a currently connected Client. Stores all
			received messages in the given MessageList.
				set
				fd_max -> <sys/select.h> select(nfds)
				dest
			=>	`dest`
		**/
		MessageList &get_messages_by_fd_set(fd_set *set, int fd_max, MessageList &dest);
		
	private:
		/// maps socket => Client
		std::unordered_map<int, ClientSptr> clients;
};

#endif
