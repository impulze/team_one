/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _CLIENTCOLLECTION_H_
#define _CLIENTCOLLECTION_H_

#include <forward_list>
#include <memory>
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
			Adds all clients' sockets to the given fd_set using the makro FD_SET.
				set
			=>	client socket with the highest integral value of all added sockets
		**/
		int fill_fd_set(fd_set *set) const;
		MessageList &get_messages_by_fd_set(fd_set *set, int fd_max, MessageList &dest);
		
	private:
		/// maps socket => Client
		std::unordered_map<int, ClientSptr> clients;
};

#endif
