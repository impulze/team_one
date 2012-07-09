/**	@file NetworkInterface.h

	@author Maximilian Lasser <max.lasser@online.de>
	@date Friday, 11th May 2012
**/

#ifndef _NETWORKINTERFACE_H_
#define _NETWORKINTERFACE_H_

#include <forward_list>
#include <vector>

#include "ClientCollection.h"

typedef void (*NetworkMessageHandler)(const Message &); ///< NetworkMessageHandler type

/**
	@brief Main network interface class. All the fancy stuff happens here.

	The NetworkInterface is a singleton class that builds the interface between the server software
	and the network.
	It's run method should be run in an own thread as it's there for accepting new connections and
	incoming messages.

	@note Currently it's only allowed to instantiate this class once.
**/
class NetworkInterface
{
	public:
		/**
			Retrieves the current instance.

			@result a reference to this' current instance

			@exception Exception::NotYetInstantiated if no NetworkInterface has been instantiated
				yet
		**/
		static NetworkInterface &get_current_instance(void);

		/**
			
			Standard constructor.
			Creates and binds a listening socket and sets it to listening state.

			@param port port to bind to
			@param backlog backlog argument to pass to  the listen function defined in sys/socket.h

			@exception Exception::AlreadyInstantiated if an instance of this class already exists
			@exception Exception::ErrnoError if the listening socket creation failed
			@exception Exception::ErrnoError if the network address structure generation failed
			@exception Exception::ErrnoError if the listening socket binding failed
			@exception Exception::ErrnoError if the listening failed
		**/
		NetworkInterface(int port, int backlog = 4);
		~NetworkInterface(void); //< Standard destructor.
		
		/**
			Adds a message handler to this NetworkInterface. Each added handler will get called for
			each received Message.
			The given handler will be added to the list regardless of whether it's already there or
			not.
			The invocation order is the addition order reversed, i.e. the first added handler will
			be called last, vice versa and for the handlers in between analogously.

			@param handler the NetworkMessageHandler to add
		**/
		void add_message_handler(const NetworkMessageHandler handler);
		/**
			Broadcasts a Message to all connected Clients.

			@param message a reference to the Message to broadcast
			@param document_id an optional constraint causing the bytestream to be sent only to all
				clients whose active document id is equal to the valueof this argument; the default
				is 0, which means that the bytestream should be sent to all clients
			
			@note Calls Message::send_to(const ClientCollection&, int32_t) without catching any
				exceptions.
			@see Message::send_to(const ClientCollection&, int32_t)
		**/
		void broadcast_message(const Message &message, int32_t document_id = 0) const;
		/**
			Disconnects a client.
			@param client a reference to the Client to disconnect
		**/
		void disconnect_client(Client &client);
		/**
			Removes all occurrences of the specified handler from this' handler list.
			
			@param handler the handler to remove
		**/
		void remove_message_handler(const NetworkMessageHandler handler);		
		/**
			Main routine that looks for incoming client connections and messages and processes the
			latter as necessary.

			@param ipc_socket TODO

			@exception Exception::ErrnoError if select failed
			
			@note Calls ClientCollection::accept_client(int) without catching any exceptions.
			@see ClientCollection::accept_client(int)
		**/
		void run(int ipc_socket);
	
	private:
		static NetworkInterface						*instance; ///< holds this' current instance

		ClientCollection							 clients; ///< connected clients
		int											 listener; ///< listener socket
		std::forward_list<NetworkMessageHandler>	 message_handlers; ///< message handlers
};

#endif
