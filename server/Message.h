/**
	file: Message.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <cstdint> // uint*_t
#include "Client.h"

#define MSGF_SIZE_BYTE 1
#define MSGF_SIZE_ID 4
#define MSGF_SIZE_DOC_NAME 128
#define MSGF_SIZE_PASSWORD_HASH 20
#define MSGF_SIZE_SIZE 8
#define MSGF_SIZE_TYPE 1
#define MSGF_SIZE_USER_NAME 64

enum MessageStatus{
	MSGS_OK
};

enum MessageType {
	MSGT_DOC_ACTIVATE,
	MSGT_DOC_CREATE,
	MSGT_DOC_DELETE,
	MSGT_DOC_OPEN,
	MSGT_DOC_SAVE,
	MSGT_SYNC_BYTE,
	MSGT_SYNC_CURSOR,
	MSGT_SYNC_DELETION,
	MSGT_SYNC_MULTIBYTE,
	MSGT_USER_LOGIN,
	MSGT_USER_LOGOUT,
	MSGT_USER_JOIN,
	MSGT_USER_QUIT
};

class Message
{
	public:
		/**
			Attempts to generate a new Message by reading a bytestream sent by the given client.
				client
		**/
		Message(Client &client);
		~Message(void);
		
		const char &get_byte(void) const;
		const uint32_t &get_id(void) const;
		const char *get_name(void) const;
		const char *get_password(void) const;
		const char *get_payload(void) const;
		const uint64_t &get_position(void) const;
		const uint64_t &get_size(void) const;
		const MessageType &get_type(void) const;
		
	private:
		char			 byte,
						*payload;
		uint32_t		 id;
		uint64_t		 position;
		uint64_t	 	 size;
		MessageType	 	 type;
};

#endif