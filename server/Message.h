/**	@file Message.h
	
	@author Maximilian Lasser <max.lasser@online.de>
	@date Tuesday, 22nd May 2012
**/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "ClientCollection.h"

class Client;

/**
	@brief Encapsulates a network message.

	The Message class is an abstraction of a simple network message that may be sent from client
	to server or vice versa.
**/
class Message
{
	public:
		/**
			The MessageStatus is mainly used for messages from server to client that report the
			status of a requested action, such as success or failure, the latter often described
			more detailed.
		**/
		enum class MessageStatus
		{
			STATUS_OK, ///< success
			STATUS_OK_CONTENTS_FOLLOWING, ///< multibyte message with doc contents following
			STATUS_DOC_ALREADY_EXIST, ///< doc does already exist
			STATUS_DOC_NOT_EXIST, ///< doc does not exist
			STATUS_DOC_SAVED, ///< doc was saved by another user
			STATUS_DB_ERROR, ///< a DB error occured
			STATUS_IO_ERROR, ///< an IO error occured
			STATUS_USER_NOT_EXIST, ///< username does not exist
			STATUS_USER_WRONG_PASSWORD, ///< password is wrong
			STATUS_USER_NO_ACTIVE_DOC, ///< user has no active doc
			STATUS_USER_CURSOR_UNKNOWN, ///< user cursor position is unknown
			STATUS_USER_CURSOR_OUT_OF_BOUNDS, ///< user cursor position is out of bounds
			STATUS_USER_LENGTH_TOO_LONG, ///< specified length is too long
			STATUS_NOT_OK ///< anything but success
		};
		/**
			The MessageType specifies what kind of action is required or to what kind of action the
			Message is the answer.
		**/
		enum class MessageType
		{
			TYPE_INVALID, ///< invalid message type
			TYPE_DOC_ACTIVATE, ///< user activates/switches to doc (id, hash)
			TYPE_DOC_CREATE, ///< user creates doc (name)
			TYPE_DOC_DELETE, ///< user deletes doc (name)
			TYPE_DOC_OPEN, ///< user opens doc (name)
			TYPE_DOC_SAVE, ///< user saves doc (id)
			TYPE_STATUS, ///< server -> client only (general status announcement)
			TYPE_SYNC_BYTE, ///< user sends byte to insert at current pos (byte)
			TYPE_SYNC_CURSOR, ///< user sends new cursor position (position)
			TYPE_SYNC_DELETION, ///< user sends deletion (position, length)
			TYPE_SYNC_MULTIBYTE, ///< user sends byte sequence to insert (position, length, payload)
			TYPE_USER_LOGIN, ///< user sends login credentials (name, hash)
			TYPE_USER_LOGOUT, ///< user logs out
			TYPE_USER_JOIN, ///< server -> client only (a new user connected)
			TYPE_USER_QUIT, ///< server -> client only (a user disconnected)

			TYPE_INIT, ///< pseudo-type for handler initialization
			TYPE_EXIT ///< pseudo-type on server exit/quit
		};
		
		static const size_t
			FIELD_SIZE_BYTE = 1, ///< size of a byte
			FIELD_SIZE_ID = 4, ///< size of a document or user id
			FIELD_SIZE_DOC_NAME = 128, ///< size of a document name
			FIELD_SIZE_HASH = 20, ///< size of a password or document hash (sha-1)
			FIELD_SIZE_SIZE = 4, ///< size of a position or length
			FIELD_SIZE_STATUS = 1, ///< size of a MessageStatus
			FIELD_SIZE_TYPE = 1, ///< size of a MessageType
			FIELD_SIZE_USER_NAME = 64; ///< size of a user name
		
		std::vector<char>					bytes; ///< Message payload
		std::array<char, FIELD_SIZE_HASH>	hash; ///< password or document hash (sha-1)
		int32_t								length; ///< mostly payload length, depends on context
		int32_t								id; ///< document or user id
		std::vector<char>					name; ///< document or user name
		int32_t								position; ///< position within a document
		ClientSptr							source; ///< sender of the message
		MessageStatus						status; ///< status of the respective action/request
		MessageType	 	 					type; ///< type of the message

		/**
			Default constructor.
		**/
		Message(void);
		Message(const Message &) = delete; ///< No copy constructor.

		Message operator=(const Message &) = delete; ///< No copying via assignment operator.
		
		/**
			Converts the stored name field to a string and returns it.

			@return the stored name as string
		**/
		inline std::string get_name_string(void) const;
		/**
			Checks whether this is an empty Message.

			@return whether this Message is empty

			@deprecated
			@note Use at own risk. This method is not good.
		**/
		inline bool is_empty() const;
		/**
			Attempts to parse a bytestream sent by the given client to this Message object.
			This is kind of a named constructor, but the object has to be constructed already.

			@param client a shared pointer to the Client to receive the Message from

			@exception Exception::InvalidMessageType if the message has an invalid type

			@note This calls Client::receive(T*, size_t) without catching any exceptions.
			@see Client::receive(T*, size_t)
		**/
		void receive_from(ClientSptr client);
		/**
			Sends a raw byte sequence representation of this Message to the specified Client.
			
			@param client a reference to the Client to send this Message to

			@note Calls Client::send(const std::vector<char>&) without catching any exceptions.
			@see Client::send(const std::vector<char>&)
		**/
		void send_to(const Client &client) const;
		/**
			Like send_to(ClientSptr), but sends to all Clients in the given ClientCollection.
			
			@param clients a reference to the ClientCollection to broadcast the this Message in
			@param document_id an optional constraint causing the bytestream to be sent only to all
				clients whose active document id is equal to the valueof this argument; the default
				is 0, which means that the bytestream should be sent to all clients

			@note Calls ClientCollection::broadcast(const std::vector<char>&, int32_t) without
				catching any exceptions.
			@see ClientCollection::broadcast(const std::vector<char>&, int32_t)
		**/
		void send_to(const ClientCollection &clients, int32_t document_id = 0) const;
	
	private:
		/**
			Auxiliary function that appends a byte sequence to the given vector.

			@param dest a reference to the vector<char> to append the bytes to
			@param src a pointer to the source of the bytestream to append
			@param length the number of bytes to append; if discarded or 0 the length will be
				determined using the sizeof operator on *src

			@return a reference to the vector<char> the bytes have been appended to
		**/
		template<typename T>
		static inline std::vector<char> &append_bytes(std::vector<char> &dest, const T *src,
			size_t length = 0);
		/**
			Like append_bytes(std::vector<char>&, const T*, size_t), but takes a value as src, not a
			pointer.

			@see append_bytes(std::vector<char>&, const T*, size_t)
		**/
		template<typename T>
		static inline std::vector<char> &append_bytes(std::vector<char> &dest, const T src,
			size_t length = 0);
		// static inline uint64_t htonll(uint64_t hostlonglong);
		// static inline uint64_t ntohll(uint64_t netlonglong);

		/**
			Auxiliary function that generates a bytesteam from this Message that can be sent to one
			or more Clients.

			@param dest a reference to a vector<char> to store the bytestream in
			@return a referenct to the vector<char> the bytestream has been stored in

			@exception Exception::InvalidMessageType if the MessageType is invalid
		**/
		std::vector<char> &generate_bytestream(std::vector<char> &dest) const;
};

#include "Message.tcc"

#endif
