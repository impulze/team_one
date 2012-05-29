/**
	file: Message.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#include <cstdlib> // malloc
#include "exceptions.h"
#include "Message.h"

Message::Message(Client &client):
	byte('\0'), payload(NULL), id(0), position(0), size(0)
{
	// get message type
	// FIXME: receive does not have any parameter
#if 0
	client.receive(&this->type, MSGF_SIZE_TYPE);
	
	// get first data
	switch (this->type)
	{
		case MSGT_DOC_ACTIVATE:
		case MSGT_DOC_SAVE:
			client.receive(&this->id, MSGF_SIZE_ID);
			break;
		case MSGT_DOC_CREATE:
		case MSGT_DOC_DELETE:
		case MSGT_DOC_OPEN:
			this->payload = (char *)malloc(MSGF_SIZE_DOC_NAME);
			client.receive(&this->payload, MSGF_SIZE_DOC_NAME);
			break;
		case MSGT_SYNC_BYTE:
			client.receive(&this->byte, MSGF_SIZE_BYTE);
			break;
		case MSGT_SYNC_CURSOR:
		case MSGT_SYNC_DELETION:
			client.receive(&this->position, MSGF_SIZE_SIZE);
			break;
		case MSGT_SYNC_MULTIBYTE:
			client.receive(&this->size, MSGF_SIZE_SIZE);
			break;
		case MSGT_USER_LOGIN:
			this->payload = (char *)malloc(MSGF_SIZE_USER_NAME + MSGF_SIZE_PASSWORD_HASH + 1);
			client.receive(&this->payload, MSGF_SIZE_USER_NAME);
			break;
		case MSGT_USER_LOGOUT: break;
		default:
			throw Exception::InvalidMessageType(this->type);
	}
	
	// get second data
	switch (this->type)
	{
		case MSGT_USER_LOGIN:
			client.receive(&this->payload + MSGF_SIZE_USER_NAME, MSGF_SIZE_PASSWORD_HASH);
			this->payload[MSGF_SIZE_USER_NAME + MSGF_SIZE_PASSWORD_HASH] = '\0';
			break;
		case MSGT_SYNC_DELETION:
			client.receive(&this->position, MSGF_SIZE_SIZE);
			break;
		case MSGT_SYNC_MULTIBYTE:
			this->payload = (char *)malloc(this->size);
			client.receive(&this->payload, this->size);
			break;
	}
#endif
}

Message::~Message(void)
{
	switch (this->type)
	{
		case MSGT_DOC_CREATE:
		case MSGT_DOC_DELETE:
		case MSGT_DOC_OPEN:
		case MSGT_SYNC_DELETION:
		case MSGT_SYNC_MULTIBYTE:
		case MSGT_USER_LOGIN:
			free(this->payload);
	}
}

const char &Message::get_byte(void) const
{ return this->byte; }

const uint32_t &Message::get_id(void) const
{ return this->id; }

const char *Message::get_name(void) const
{ return this->payload; }

const char *Message::get_password(void) const
{ return this->payload + MSGF_SIZE_USER_NAME; }

const char *Message::get_payload(void) const
{ return this->payload; }

const uint64_t &Message::get_position(void) const
{ return this->position; }

const uint64_t &Message::get_size(void) const
{ return this->size; }
