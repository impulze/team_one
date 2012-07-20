/**
 * @file Message.cpp
 * @author Maximilian Lasser <max.lasser@online.de>
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 */

#include "Client.h"
#include "exceptions.h"
#include "Message.h"

Message::Message(void):
	length(0), id(0), position(0), source(NULL), status(MessageStatus::STATUS_NOT_OK), type(MessageType::TYPE_INVALID)
{}

void Message::receive_from(ClientSptr client)
{
	char buffer;

	// save source
	source = client;

	// get message type
	// added by Daniel: graceful client disconnects
	try
	{
		client->receive(&buffer, FIELD_SIZE_TYPE);
	}
	catch (Exception::SocketDisconnected const &error)
	{
		// first message bytes -> disconnect is ok -> empty message
		source = 0;
		throw;
	}

	type = static_cast<MessageType>(buffer);
	
	// get first data
	switch (type)
	{
		case MessageType::TYPE_DOC_ACTIVATE:
		case MessageType::TYPE_DOC_SAVE:
			client->receive(&id, FIELD_SIZE_ID);
			id = ntohl(id);
			break;
		case MessageType::TYPE_DOC_CREATE:
		case MessageType::TYPE_DOC_DELETE:
		case MessageType::TYPE_DOC_OPEN:
			name.resize(FIELD_SIZE_DOC_NAME);
			client->receive(&name[0], FIELD_SIZE_DOC_NAME);
			break;
		case MessageType::TYPE_SYNC_BYTE:
			bytes.resize(FIELD_SIZE_BYTE);
			client->receive(&bytes, FIELD_SIZE_BYTE);
			break;
		case MessageType::TYPE_SYNC_CURSOR:
		case MessageType::TYPE_SYNC_DELETION:
			client->receive(&position, FIELD_SIZE_SIZE);
			position = ntohl(position);
			break;
		case MessageType::TYPE_SYNC_MULTIBYTE:
			client->receive(&length, FIELD_SIZE_SIZE);
			length = ntohl(length);
			break;
		case MessageType::TYPE_USER_LOGIN:
			name.resize(FIELD_SIZE_USER_NAME);
			client->receive(&name[0], FIELD_SIZE_USER_NAME);
			break;
		case MessageType::TYPE_DOC_LIST:
		case MessageType::TYPE_USER_LOGOUT:
			break;
		default:
			throw Exception::InvalidMessageType("invalid message type", type, client->socket);
	}
	
	// get second data
	switch (type)
	{
		case MessageType::TYPE_DOC_ACTIVATE:
		case MessageType::TYPE_USER_LOGIN:
			client->receive(&hash[0], FIELD_SIZE_HASH);
			break;
		case MessageType::TYPE_SYNC_DELETION:
			client->receive(&length, FIELD_SIZE_SIZE);
			length = ntohl(length);
			break;
		case MessageType::TYPE_SYNC_MULTIBYTE:
			bytes.resize(length);
			client->receive(&bytes, length);
			break;
		default: break;
	}
}

std::vector<char> &Message::generate_bytestream(std::vector<char> &dest) const
{
	// append message type
	append_bytes(dest, static_cast<char>(type));

	// append first data
	switch (type)
	{
		case MessageType::TYPE_DOC_ACTIVATE:
		case MessageType::TYPE_DOC_CREATE:
		case MessageType::TYPE_DOC_DELETE:
		case MessageType::TYPE_DOC_OPEN:
		case MessageType::TYPE_DOC_SAVE:
		case MessageType::TYPE_USER_LOGIN:
		case MessageType::TYPE_STATUS:
			append_bytes(dest, static_cast<char>(status));
			break;
		case MessageType::TYPE_DOC_LIST:
			append_bytes(dest, htonl(length));
			break;
		case MessageType::TYPE_SYNC_BYTE:
		case MessageType::TYPE_SYNC_DELETION:
		case MessageType::TYPE_SYNC_MULTIBYTE:
			append_bytes(dest, htonl(position));
			break;
		case MessageType::TYPE_USER_JOIN:
		case MessageType::TYPE_USER_QUIT:
			append_bytes(dest, htonl(id));
			break;
		default:
			throw Exception::InvalidMessageType("invalid message type", type, 0);
	}

	// append second data
	switch (type)
	{
		case MessageType::TYPE_DOC_ACTIVATE:
		case MessageType::TYPE_DOC_OPEN:
		case MessageType::TYPE_DOC_SAVE:
			append_bytes(dest, htonl(id));
			break;
		case MessageType::TYPE_DOC_CREATE:
		case MessageType::TYPE_DOC_DELETE:
			append_bytes(dest, &name, FIELD_SIZE_DOC_NAME);
			break;
		case MessageType::TYPE_DOC_LIST:
		{
			std::vector<char> buffer = bytes;
			size_t target_size = FIELD_SIZE_DOC_NAME * length;
			if (buffer.size() < target_size)
			{ buffer.resize(target_size, '\0'); }

			append_bytes(dest, &buffer, target_size);
			break;
		}
		case MessageType::TYPE_SYNC_BYTE:
			append_bytes(dest, &bytes, FIELD_SIZE_BYTE);
			break;
		case MessageType::TYPE_SYNC_DELETION:
		case MessageType::TYPE_SYNC_MULTIBYTE:
			append_bytes(dest, htonl(length));
			break;
		case MessageType::TYPE_USER_JOIN:
			append_bytes(dest, &name, FIELD_SIZE_USER_NAME);
			break;
		default: break;
	}

	// append third data
	switch (type)
	{
		case MessageType::TYPE_DOC_OPEN:
			append_bytes(dest, &name, FIELD_SIZE_DOC_NAME);
			break;
		case MessageType::TYPE_SYNC_MULTIBYTE:
			append_bytes(dest, &bytes, length);
			break;
		default: break;
	}

	return dest;
}

void Message::send_to(const Client &client) const
{
	// generate bytestream to send
	std::vector<char> bytestream;
	generate_bytestream(bytestream);

	// send
	client.send(bytestream);
}

void Message::send_to(const ClientCollection &clients, int32_t document_id) const
{
	// generate bytestream to send
	std::vector<char> bytestream;
	generate_bytestream(bytestream);

	// send
	clients.broadcast(bytestream, document_id);
}
