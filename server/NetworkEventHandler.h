/**
	file: NetworkInterface.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

#ifndef _NETWORKEVENTHANDLER_H_
#define _NETWORKEVENTHANDLER_H_

#include <cstdint>

class Client;

class NetworkEventHandler
{
	public:
		virtual void on_document_activate(Client &client, uint32_t document_id) const = 0;
		virtual void on_document_create(Client &client, const char *document_name) const = 0;
		virtual void on_document_delete(Client &client, const char *document_name) const = 0;
		virtual void on_document_open(Client &client, const char *document_name) const = 0;
		virtual void on_document_save(Client &client, uint32_t document_id) const = 0;
		virtual void on_sync_byte(Client &client, char byte) const = 0;
		virtual void on_sync_cursor(Client &client, uint64_t position) const = 0;
		virtual void on_sync_deletion(Client &client, uint64_t position, uint64_t length) const = 0;
		virtual void on_sync_multibyte(Client &client, const char *bytes) const = 0;
		virtual void on_user_login(Client &client, const char *user_name, const char *password_hash) const = 0;
		virtual void on_user_logout(Client &client) const = 0;
};

#endif
