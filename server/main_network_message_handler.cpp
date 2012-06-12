/**
	file: main_network_message_handler.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Monday, 11th June 2012
**/

#include "Client.h"
#include "Message.h"

void main_network_message_handler(const Message &message)
{
	switch (message.type)
	{
		case Message::TYPE_DOC_ACTIVATE:
			/* TODO
				if doc not exists
					respond: not existing
					return
				
				get current doc hash
				compare hash to received hash

				if hashs not equal
					respond: ok, contents following
					send contents via multibyte package
					return

				respond ok
			*/
			break;
		
		case Message::TYPE_DOC_CREATE:
			/* TODO
				if doc already exists
					respond: already existing
					return

				create doc
				respond ok
			*/
			break;

		case Message::TYPE_DOC_DELETE:
			/* TODO
				if doc not exists
					respond: not existing
					return

				delete doc
				respond ok
			*/
			break;

		case Message::TYPE_DOC_OPEN:
			/* TODO
				if doc not exists
					respond: not existing
					return

				if doc not empty
					respond: ok, contents following
					return

				respond: ok
			*/
			break;

		case Message::TYPE_DOC_SAVE:
			/* TODO
				if doc not exists
					respond: not existing
					return
				
				save doc
				respond: ok
				UNCLEAR: add new general message type so all clients get the information that the
					doc has been saved?
			*/
			break;

		case Message::TYPE_SYNC_BYTE:
			/* TODO
				if client has no active remote doc
					discard (UNCLEAR: add new message type for general errors so the client gets an
						error message saying they have no document activated?)
					return

				if client cursor position unclear/unknown (if that's even possible)
					discard (see above)
					return

				if client cursor position out of bounds
					discard (see above)
					return

				add byte at current client cursor position in current client active doc
				sync byte to all clients that have this doc active
			*/
			break;

		case Message::TYPE_SYNC_CURSOR:
			message.source->cursor = message.position;
			break;

		case Message::TYPE_SYNC_DELETION:
			/* TODO
				if client has no active remote doc
					discard (see first UNCLEAR in byte sync)
					return

				if start is out of bounds
					discard (see above)
					return
				
				if length too big
					discard (see above)
					return

				delete specified area in client active doc
				sync deletion to all clients that have this doc active
			*/
			break;

		case Message::TYPE_SYNC_MULTIBYTE:
			/* TODO
				[same first three checks as in byte sync]

				add byted to current client cursor position in current client active doc
				sync bytes to all clients that have this doc active
			*/
			break;

		case Message::TYPE_USER_LOGIN:
			/* TODO
				check user credentials
				if username unknown
					respond: unknown username
					return
				if password wrong
					respond: wrong password
					return

				get user id
				respond: ok
				sync user join to all users
			*/
			break;

		case Message::TYPE_USER_LOGOUT:
			/* TODO
				clear userdata
				close connection
				sync user quit to all users
			*/
			break;

		default: break;
	}
}
