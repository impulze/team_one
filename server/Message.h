/**
	file: Message.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <array>
#include <cstdint> // uint*_t
#include <string>
#include <vector>

#include "ClientCollection.h"

class Client;

class Message
{
	public:
		enum MessageStatus
		{
			STATUS_OK, // success
			STATUS_OK_CONTENTS_FOLLOWING, // multibyte message with doc contents following
			STATUS_DOC_ALREADY_EXIST, // doc does already exist
			STATUS_DOC_NOT_EXIST, // doc does not exist
			STATUS_DOC_SAVED, // doc was saved by another user
			STATUS_IO_ERROR, // an IO error occurred
			STATUS_USER_NOT_EXIST, // username does not exist
			STATUS_USER_WRONG_PASSWORD, // password is wrong
			STATUS_USER_NO_ACTIVE_DOC, // user has no active doc
			STATUS_USER_CURSOR_UNKNOWN, // user cursor position is unknown
			STATUS_USER_CURSOR_OUT_OF_BOUNDS, // user cursor position is out of bounds
			STATUS_USER_LENGTH_TOO_LONG, // specified length is too long
			STATUS_NOT_OK // anything but success
		};
		enum MessageType
		{
			TYPE_INVALID, // invalid message type
			TYPE_DOC_ACTIVATE, // user activates/switches to doc (id, hash)
			TYPE_DOC_CREATE, // user creates doc (name)
			TYPE_DOC_DELETE, // user deletes doc (name)
			TYPE_DOC_OPEN, // user opens doc (name)
			TYPE_DOC_SAVE, // user saves doc (id)
			TYPE_STATUS, // server -> client only (general status announcement)
			TYPE_SYNC_BYTE, // user sends byte to insert at current pos (byte)
			TYPE_SYNC_CURSOR, // user sends new cursor position (position)
			TYPE_SYNC_DELETION, // user sends deletion (position, length)
			TYPE_SYNC_MULTIBYTE, // user sends byte sequence to insert at current position (length,
								 // payload)
			TYPE_USER_LOGIN, // user sends login credentials (name, hash)
			TYPE_USER_LOGOUT, // user logs out
			TYPE_USER_JOIN, // server -> client only (a new user connected)
			TYPE_USER_QUIT, // server -> client only (a user disconnected)

			TYPE_INIT, // pseudo-type for handler initialization
			TYPE_EXIT // pseudo-type on server exit/quit
		};
		
		static const size_t
			FIELD_SIZE_BYTE = 1,
			FIELD_SIZE_ID = 4,
			FIELD_SIZE_DOC_NAME = 128,
			FIELD_SIZE_HASH = 20,
			FIELD_SIZE_SIZE = 4,
			FIELD_SIZE_STATUS = 1,
			FIELD_SIZE_TYPE = 1,
			FIELD_SIZE_USER_NAME = 64;
		
		std::vector<char>					bytes;
		std::array<char, FIELD_SIZE_HASH>	hash;
		int32_t								length;
		int32_t								id;
		std::vector<char>					name;
		int32_t								position;
		ClientSptr							source;
		MessageStatus						status;
		MessageType	 	 					type;

		Message(void);
		Message(const Message &) = delete;

		Message operator=(const Message &) = delete;
		
		/**
			Converts the stored name field to a std::string and returns it.
		**/
		inline std::string get_name_string(void) const;
		/**
			Checks whether this is an empty message.
		**/
		inline bool is_empty() const;
		/**
			Attempts to parse a bytestream sent by the given client to this Message object.
				client
			=#	Exception::InvalidMessageType - if the message has an invalid type
			=#	client.receive
		**/
		void receive_from(ClientSptr client);
		/**
			Attempts to send a raw byte sequence representation of this Message to the specified
			Client.
				client
			=#	Client::send(std::vector<char> &)
		**/
		void send_to(const Client &client) const;
		/**
			Like send_to(ClientSptr), but sends to all Clients in the given ClientCollection.
				clients
				document_id [#] -> ClientCollection::broadcast(std::vector<char> &, uint32_t)
					.document_id
			=#	ClientCollection::broadcast(std::vector<char> &, int32_t)
		**/
		void send_to(const ClientCollection &clients, int32_t document_id = 0) const;
	
	private:
		/**
			Auxiliary function that appends a byte sequence to the given vector.
				 dest - char(/byte) vector to append the bytes to
				 src
					source, i.e. either to pointer to the first byte or the raw value whose bytes
					shall be appended
				*length [#] - number of bytes to append
			=>	`dest`
		**/
		template<typename T>
		static inline std::vector<char> &append_bytes(std::vector<char> &dest, const T *src,
			size_t length = 0);
		template<typename T>
		static inline std::vector<char> &append_bytes(std::vector<char> &dest, const T src,
			size_t length = 0);
		// static inline uint64_t htonll(uint64_t hostlonglong);
		// static inline uint64_t ntohll(uint64_t netlonglong);

		/**
			Generates a bytesteam from this Message that can be sent to one or more Clients.
				dest - vector to store the bytestream in
			=>	`dest`
		**/
		std::vector<char> &generate_bytestream(std::vector<char> &dest) const;
};

#include "Message.tcc"

#endif
