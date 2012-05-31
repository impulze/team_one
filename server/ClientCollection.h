/**
	file: ClientCollection.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _CLIENTCOLLECTION_H_
#define _CLIENTCOLLECTION_H_

#include <memory>
#include <vector>
#include <unordered_map>

class Client;

typedef std::shared_ptr<Client> ClientSptr;

class ClientCollection
{
	public:
		/**
			Creates a new Client object and adds it to the map.
				listener
			=>	reference to the newly created Client
			=#	Client::Client
		**/
		Client &add_client(int listener);
		
		/**
			Adds all clients' sockets to the given fd_set using the makro FD_SET.
				set
			=>	client socket with the highest integral value of all added plus 1 (for use in select)
		**/
		int fill_fd_set(fd_set *set) const;
		
	private:
		/// maps socket => Client
		std::unordered_map<int, ClientSptr> clients;
};

#endif

12:44 gruppengesp
13:10 teamgesp: fachmod
13:18 kundengesp: netzwerk
13:48