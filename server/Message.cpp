/**
	file: Message.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#include "Client.h"
#include "exceptions.h"
#include "Message.h"

Message::Message(void):
	length(0), id(0), position(0), source(NULL), type(TYPE_INVALID)
{}

void Message::receive_from(ClientSptr client)
{
	// save source
	this->source = client;

	// get message type
	client->receive(&this->type, FIELD_SIZE_TYPE);
	
	// get first data
	switch (this->type)
	{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_SAVE:
			client->receive(&this->id, FIELD_SIZE_ID);
			break;
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
		case TYPE_DOC_OPEN:
			this->name.resize(FIELD_SIZE_DOC_NAME);
			client->receive(&this->name, FIELD_SIZE_DOC_NAME);
			break;
		case TYPE_SYNC_BYTE:
			this->bytes.resize(FIELD_SIZE_BYTE);
			client->receive(&this->bytes, FIELD_SIZE_BYTE);
			break;
		case TYPE_SYNC_CURSOR:
		case TYPE_SYNC_DELETION:
			client->receive(&this->position, FIELD_SIZE_SIZE);
			break;
		case TYPE_SYNC_MULTIBYTE:
			client->receive(&this->length, FIELD_SIZE_SIZE);
			break;
		case TYPE_USER_LOGIN:
			this->name.resize(FIELD_SIZE_USER_NAME);
			client->receive(&this->name, FIELD_SIZE_USER_NAME);
			break;
		case TYPE_USER_LOGOUT: break;
		default:
			throw Exception::InvalidMessageType("invalid message type", this->type, client->socket);
	}
	
	// get second data
	switch (this->type)
	{
		case TYPE_DOC_ACTIVATE:
			this->hash.resize(FIELD_SIZE_HASH);
			client->receive(&this->hash, FIELD_SIZE_HASH);
		case TYPE_USER_LOGIN:
			this->hash.resize(FIELD_SIZE_HASH);
			client->receive(&this->hash, FIELD_SIZE_HASH);
			break;
		case TYPE_SYNC_DELETION:
			client->receive(&this->length, FIELD_SIZE_SIZE);
			break;
		case TYPE_SYNC_MULTIBYTE:
			this->bytes.resize(this->length);
			client->receive(&this->bytes, this->length);
			break;
		default: break;
	}
}
