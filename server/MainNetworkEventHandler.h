/**
	file: MainNetworkEventHandler.h
	author: Maximilian Lasser [max.lasser@online.de]
	created: Thursday, 24th May 2012
**/

#ifndef _MAINNETWORKHANDLER_H_
#define _MAINNETWORKHANDLER_H_

#include "NetworkEventHandler.h"

class MainNetworkEventHandler: public NetworkEventHandler
{
	void on_document_activate(Client &client, uint32_t document_id) const;
	void on_document_create(Client &client, const char *document_name) const;
	void on_document_delete(Client &client, const char *document_name) const;
	void on_document_open(Client &client, const char *document_name) const;
	void on_document_save(Client &client, uint32_t document_id) const;
	void on_sync_byte(Client &client, char byte) const;
	void on_sync_cursor(Client &client, uint64_t position) const;
	void on_sync_deletion(Client &client, uint64_t position, uint64_t length) const;
	void on_sync_multibyte(Client &client, const char *bytes) const;
	void on_user_login(Client &client, const char *user_name, const char *password_hash) const;
	void on_user_logout(Client &client) const;
};

#endif