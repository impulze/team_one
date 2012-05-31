/**
	file: Message.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <cstdint> // uint*_t
#include <vector>

class Client;

class Message
{
	public:
		enum MessageStatus
		{
			STATUS_OK
		};
		enum MessageType
		{
			TYPE_INVALID,
			TYPE_DOC_ACTIVATE,
			TYPE_DOC_CREATE,
			TYPE_DOC_DELETE,
			TYPE_DOC_OPEN,
			TYPE_DOC_SAVE,
			TYPE_SYNC_BYTE,
			TYPE_SYNC_CURSOR,
			TYPE_SYNC_DELETION,
			TYPE_SYNC_MULTIBYTE,
			TYPE_USER_LOGIN,
			TYPE_USER_LOGOUT,
			TYPE_USER_JOIN,
			TYPE_USER_QUIT
		};
		
		const uint64_t
			FIELD_SIZE_BYTE = 1,
			FIELD_SIZE_ID = 4,
			FIELD_SIZE_DOC_NAME = 128,
			FIELD_SIZE_HASH = 20,
			FIELD_SIZE_SIZE = 8,
			FIELD_SIZE_TYPE = 1,
			FIELD_SIZE_USER_NAME = 64;
		
		std::vector<char>	bytes;
		std::vector<char>	hash;
		uint64_t			length;
		uint32_t		 	id;
		std::vector<char>	name;
		uint64_t			position;
		MessageType	 	 	type;
		
		/**
			Attempts to generate a new Message by reading a bytestream sent by the given client.
				client
			=#	client.receive
			=#	Exception::InvalidMessageType - if the message has an invalid type
		**/
		Message(Client &client);
};

#endif