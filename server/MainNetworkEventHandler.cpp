/**
	file: MainNetworkEventHandler.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Wednesday, 23rd May 2012
**/

#ifndef _MAINNETWORKHANDLER_H_
#define _MAINNETWORKHANDLER_H_

class MainNetworkEventHandler : NetworkEventHandler
{};

void MainNetworkEventHandler::on_document_activate(Client &client, uint32_t document_id) const
{ client.active_document = document_id; }

void MainNetworkEventHandler::on_document_create(Client &client, const char *document_name) const
{
	// TODO
}

void MainNetworkEventHandler::on_document_delete(Client &client, const char *document_name) const
{
	// TODO
}

void MainNetworkEventHandler::on_document_open(Client &client, const char *document_name) const
{
	// TODO
}

void MainNetworkEventHandler::on_document_save(Client &client, uint32_t document_id) const
{
	// TODO
}

void MainNetworkEventHandler::on_sync_byte(Client &client, char byte) const
{
	// TODO: sync bytes to all clients that have this document open
}

void MainNetworkEventHandler::on_sync_cursor(Client &client, uint64_t position) const
{ client.cursor = position; }

void MainNetworkEventHandler::on_sync_deletion(Client &client, uint64_t position, uint64_t length) const
{
	// TODO: sync deletion to all clients that have this document open
}

void MainNetworkEventHandler::on_sync_multibyte(Client &client, const char *bytes) const
{
	// TODO: sync everyting
}

void MainNetworkEventHandler::on_user_login(Client &client, const char *user_name, const char *password_hash) const
{
	// TODO: check username and password etc. and add user to client map
}

void MainNetworkEventHandler::on_user_logout(Client &client) const
{
	// TODO: delete everything from client!!!
}

#endif