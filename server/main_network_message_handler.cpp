/**	@file main_network_message_handler.cpp

	@author Maximilian Lasser <max.lasser@online.de>
	@date Monday, 11th June 2012
**/

#include <unordered_map>
#include <unordered_set>

#include "Client.h"
#include "Document.h"
#include "Message.h"
#include "NetworkInterface.h"
#include "UserDatabase.h"

// auxiliary functions
namespace
{
	typedef std::shared_ptr<Document> DocumentSptr;

	std::unordered_map<int32_t, DocumentSptr> doc_by_id; // doc_id -> doc
	std::unordered_map<int32_t, size_t> doc_counter; // doc_id -> doc_opened_count
	std::unordered_map<std::string, DocumentSptr> doc_by_name; // doc_name -> doc
	std::unordered_map<int32_t, std::unordered_set<int32_t>> open_docs; // client_id -> doc_id...

	void close_document(int32_t doc_id, int32_t client_id = 0);

	/**
		Invokes document_close for all documents opened by the given client.
		@param client_id - id of the respective client
	**/
	void close_client_documents(int32_t client_id)
	{
		// get the list of opened documents, if existing
		auto docs = open_docs.find(client_id);
		if (docs == open_docs.end())
		{ return; }

		// close all docs opened by this client
		std::forward_list<int32_t> doc_list;
		for (const auto &doc_id: docs->second)
		{ doc_list.push_front(doc_id); }
		for (const auto &doc_id: doc_list)
		{ close_document(doc_id, client_id); }

		open_docs.erase(client_id);
	}

	/**
		Closes an opened document if it's not needed anymore, otherwise just decrements the document
		counter and deassigns it from the given client.
		@param doc_id - document id
		@param client_id - id of the client that closed this document, 0 if no client involved
	**/
	void close_document(int32_t doc_id, int32_t client_id)
	{
		// remove from client opened documents
		if (client_id != 0)
		{
			auto docs = open_docs.find(client_id);
			if (docs != open_docs.end())
			{ docs->second.erase(doc_id); }
		}
		--doc_counter[doc_id];

		// close document if not needed anymore
		if (doc_counter[doc_id] == 0)
		{
			DocumentSptr doc = doc_by_id[doc_id];
			doc_by_id.erase(doc_id);
			doc_by_name.erase(doc->get_name());
			doc_counter.erase(doc_id);
			doc->close();
		}
	}

	/**
		Creates a new document, if it doesn't exist yet.
			name - document name
		=#	Message::MessageStatus::STATUS_DOC_NOT_EXIST - document doesn't exist
		=#	Message::MessageStatus::STATUS_IO_ERROR - an IO error occured
	**/
	void create_document(const std::string &name)
	{
		try
		{
			Document doc = Document::create(name);
			doc.close();
		}
		catch (document_errors::DocumentDoesntExistError)
		{ throw Message::MessageStatus::STATUS_DOC_NOT_EXIST; }
		catch (document_errors::DocumentError)
		{ throw Message::MessageStatus::STATUS_IO_ERROR; }
	}

	/**
		Deletes an existing document.
			name - document name
		=#	Message::MessageStatus::STATUS_DOC_NOT_EXIST - document doesn't exist
		=#	Message::MessageStatus::STATUS_IO_ERROR - an IO error occured
	**/
	void delete_document(const std::string &name)
	{
		try
		{
			Document doc = Document::open(name);
			doc.remove();
			doc.close();
		}
		catch (document_errors::DocumentDoesntExistError)
		{ throw Message::MessageStatus::STATUS_DOC_NOT_EXIST; }
		catch (document_errors::DocumentError)
		{ throw Message::MessageStatus::STATUS_IO_ERROR; }
	}

	/**
		Gets the document with the specified id from the auxiliary cache. It must already have been
		opened to make this action succeed.
			id - document id
		=>	#
		=#	Message::MessageStatus::STATUS_DOC_NOT_EXIST - document isn't opened
	**/
	DocumentSptr get_document(int32_t id)
	{
		try
		{ return doc_by_id.at(id); }
		catch (std::out_of_range)
		{ throw Message::MessageStatus::STATUS_DOC_NOT_EXIST; }
	}

	std::vector<char> get_document_list(void)
	{
		// variable initialization
		const std::vector<std::string> string_list = Document::list_documents();
		std::vector<char> result(string_list.size() * Message::FIELD_SIZE_DOC_NAME);

		auto pos = result.begin();

		// iterate through document names
		for (const std::string document_name: string_list)
		{
			// skip if document name is empty
			if (document_name.empty())
			{ continue; }

			// check if document name has to be padded
			if (document_name.length() < Message::FIELD_SIZE_DOC_NAME)
			{
				result.insert(pos, document_name.front(), document_name.back());
				std::fill(pos + document_name.length(), pos + Message::FIELD_SIZE_DOC_NAME, '\0');
			}
			else
			{
				result.insert(pos, document_name.front(), document_name.front() +
					Message::FIELD_SIZE_DOC_NAME);
			}

			pos += Message::FIELD_SIZE_DOC_NAME;
		}

		return result;
	}

	/**
		Checks if a document is empty.
			name - document name
		=>	whether the document is empty
		=#	Message::MessageStatus::STATUS_DOC_NOT_EXIST - document doesn't exist
		=#	Message::MessageStatus::STATUS_IO_ERROR - an IO error occured
	**/
	bool is_document_empty(const std::string &name)
	{
		try
		{ return Document::is_empty(name); }
		catch (document_errors::DocumentDoesntExistError)
		{ throw Message::MessageStatus::STATUS_DOC_NOT_EXIST; }
		catch (document_errors::DocumentError)
		{ throw Message::MessageStatus::STATUS_IO_ERROR; }
	}

	/**
		Attempts to open a document by name or get it from the auxiliary cache. If it's opened it
		automatically gets added to the auxiliary cache for further use.
			name - document name
		=>	#
		=#	Message::MessageStatus::STATUS_DOC_NOT_EXIST - document doesn't exist
		=#	Message::MessageStatus::STATUS_IO_ERROR - an IO error occured
	**/
	DocumentSptr open_document(const std::string &name, uint32_t client_id = 0)
	{
		DocumentSptr result;

		auto iter = doc_by_name.find(name);
		if (iter == doc_by_name.end())
		{
			try
			{
				// open document and retrieve id
				result = DocumentSptr(new Document(Document::open(name)));
				int32_t doc_id = result->get_id();

				// save DocumentSptr in several hashes
				doc_by_id[doc_id] = doc_by_name[name] = result;
				doc_counter[doc_id] = 0;

				// add to client opened documents if a client id is provided
				if (client_id != 0)
				{ open_docs[client_id].insert(doc_id); }

				return result;
			}
			catch (document_errors::DocumentDoesntExistError)
			{ throw Message::MessageStatus::STATUS_DOC_NOT_EXIST; }
			catch (document_errors::DocumentError)
			{ throw Message::MessageStatus::STATUS_IO_ERROR; }
		}
		else
		{ result = iter->second; }

		// increment document counter
		++doc_counter[result->get_id()];
		
		return result;
	}

	/**
		Sends a whole document to a client, assuming that it's already cleared to 0 Bytes on the
		clientside, thus starting at position 0.
			doc - document to send
			client - client to send the document to
		=#	Message::send_to
	**/
	void send_document(Document &doc, const Client &client)
	{
		// initialize message
		Message message;
		message.type = Message::MessageType::TYPE_SYNC_MULTIBYTE;
		message.position = 0;

		std::vector<char> contents = doc.get_contents();
		auto start = contents.begin();
		while (start != contents.end())
		{
			// get remaining amount of bytes, trim to int32_t max if necessary
			size_t rem_length = contents.end() - start;
			if (rem_length > std::numeric_limits<int32_t>::max())
			{ message.length = std::numeric_limits<int32_t>::max(); }
			else
			{ message.length = rem_length; }

			// prepare bytes vector of message
			message.bytes.clear();
			message.bytes.insert(message.bytes.begin(), start, start + message.length);

			// send message and update variables
			message.send_to(client);
			start += message.length;
			message.position += message.length;
		}
	}

	void sync_bytes(const Client &client, int32_t position, const std::vector<char> &bytes,
		bool multibyte = true)
	{
		DocumentSptr doc;

		// check if client has an active document at all and it's opened
		if (client.active_document < 1)
		{ throw Message::MessageStatus::STATUS_USER_NO_ACTIVE_DOC; }

		try
		{
			// check if cursor position is known
			if (position < 0)
			{ throw Message::MessageStatus::STATUS_USER_CURSOR_UNKNOWN; }

			// get document
			doc = get_document(client.active_document);

			// get contents and check if cursor position is in bounds
			auto contents = doc->get_contents();
			if (static_cast<size_t>(position) >= contents.size())
			{ throw Message::MessageStatus::STATUS_USER_CURSOR_OUT_OF_BOUNDS; }

			// create synchronization message
			Message sync;
			sync.type = multibyte ? Message::MessageType::TYPE_SYNC_MULTIBYTE :
				Message::MessageType::TYPE_SYNC_BYTE;
			sync.bytes = multibyte ? bytes : std::vector<char>(bytes.begin(),
				bytes.begin() + 1);
			sync.length = multibyte ? bytes.size() : 1;
			sync.position = position;

			// broadcast synchronization message
			NetworkInterface::get_current_instance().broadcast_message(sync,
				client.active_document);

			// apply change to document
			contents.insert(contents.begin() + sync.position, sync.bytes.begin(),
				sync.bytes.end());
		}
		catch (Message::MessageStatus)
		{ throw Message::MessageStatus::STATUS_USER_NO_ACTIVE_DOC; }
	}
};

void main_network_message_handler(const Message &message)
{
	// check if user is logged in
	if (message.source->user_id == 0 && message.type != Message::MessageType::TYPE_USER_LOGIN)
	{ return; }
	
	// initialize response Message
	Message response;
	response.status = Message::MessageStatus::STATUS_OK;
	response.type = message.type;

	switch (message.type)
	{
		case Message::MessageType::TYPE_DOC_ACTIVATE:
		{
			DocumentSptr doc;

			try
			{
				// open document
				doc = open_document(message.get_name_string());
				response.id = message.source->active_document = doc->get_id();

				// compare hash
				if (doc->hash() != message.hash)
				{ response.status = Message::MessageStatus::STATUS_OK_CONTENTS_FOLLOWING; }
			}
			catch (Message::MessageStatus status)
			{ response.status = status; }

			// send response
			response.send_to(*message.source);

			// send contents if necessary
			if (response.status == Message::MessageStatus::STATUS_OK_CONTENTS_FOLLOWING)
			{ send_document(*doc, *message.source); }

			break;
		}
		case Message::MessageType::TYPE_DOC_CREATE:
		{
			response.name = message.name;

			// try to create the document
			try
			{ create_document(message.get_name_string()); }
			catch (Message::MessageStatus status)
			{ response.status = status; }

			response.send_to(*message.source);

			break;
		}
		case Message::MessageType::TYPE_DOC_DELETE:
		{
			response.name = message.name;

			// try to delete the document
			try
			{ delete_document(message.get_name_string()); }
			catch (Message::MessageStatus status)
			{ response.status = status; }

			response.send_to(*message.source);

			break;
		}
		case Message::MessageType::TYPE_DOC_LIST:
		{
			response.bytes = get_document_list();
			response.length = response.bytes.size() / Message::FIELD_SIZE_DOC_NAME;

			response.send_to(*message.source);

			break;
		}
		case Message::MessageType::TYPE_DOC_OPEN:
		{
			response.name = message.name;
			DocumentSptr doc;

			try
			{
				const std::string name = message.get_name_string();

				// open document and get id
				doc = open_document(name);
				response.id = message.source->active_document = doc->get_id();

				// check if document is empty
				if (!doc->get_contents().empty())
				{ response.status = Message::MessageStatus::STATUS_OK_CONTENTS_FOLLOWING; }
			}
			catch (Message::MessageStatus status)
			{ response.status = status; }

			// send response
			response.send_to(*message.source);

			// send contents if necessary
			if (response.status == Message::MessageStatus::STATUS_OK_CONTENTS_FOLLOWING)
			{ send_document(*doc, *message.source); }

			break;
		}
		case Message::MessageType::TYPE_DOC_SAVE:
		{
			response.id = message.id;

			// save the document
			try
			{
				DocumentSptr doc = get_document(message.id);
				doc->save();
			}
			catch (Message::MessageStatus status)
			{ response.status = status; }

			// send response
			response.send_to(*message.source);

			// inform all clients that have this document active about saving
			if (response.status == Message::MessageStatus::STATUS_OK)
			{
				Message announcement;
				announcement.type = Message::MessageType::TYPE_STATUS;
				announcement.status = Message::MessageStatus::STATUS_DOC_SAVED;
			
				NetworkInterface::get_current_instance().broadcast_message(announcement, message.id);
			}

			break;
		}
		case Message::MessageType::TYPE_SYNC_BYTE:
		case Message::MessageType::TYPE_SYNC_MULTIBYTE:
		{
			// sync byte(s)
			try
			{
				bool multibyte = (message.type == Message::MessageType::TYPE_SYNC_MULTIBYTE);
				int32_t position = multibyte ? message.position : message.source->cursor;
				sync_bytes(*message.source, position, message.bytes, multibyte);
				NetworkInterface::get_current_instance().update_client_cursors(position,
					multibyte ? message.length : 1, message.source->active_document);
			}
			catch (Message::MessageStatus status)
			{
				response.type = Message::MessageType::TYPE_STATUS;
				response.status = status;
				response.send_to(*message.source);
			}

			break;
		}
		case Message::MessageType::TYPE_SYNC_CURSOR:
		{
			message.source->cursor = message.position;
			break;
		}
		case Message::MessageType::TYPE_SYNC_DELETION:
		{
			try
			{
				DocumentSptr doc = get_document(message.source->active_document);
				auto contents = doc->get_contents();
				
				// check if start position is out of bounds
				if (message.position < 0 ||
					static_cast<size_t>(message.position) >= contents.size())
				{ throw Message::MessageStatus::STATUS_USER_CURSOR_OUT_OF_BOUNDS; }

				// check if length too big
				if (static_cast<size_t>(message.position + message.length) > contents.size())
				{ throw Message::MessageStatus::STATUS_USER_LENGTH_TOO_LONG; }

				// sync deletion
				Message sync;
				sync.type = Message::MessageType::TYPE_SYNC_DELETION;
				sync.position = message.position;
				sync.length = message.length;

				NetworkInterface::get_current_instance().broadcast_message(sync,
					message.source->active_document);
				NetworkInterface::get_current_instance().update_client_cursors(sync.position,
					-sync.length, message.source->active_document);

				// perform deletion
				auto start = contents.begin() + sync.position;
				contents.erase(start, start + sync.length);
			}
			catch (Message::MessageStatus status)
			{
				response.type = Message::MessageType::TYPE_STATUS;
				response.status = status == Message::MessageStatus::STATUS_DOC_NOT_EXIST ?
					Message::MessageStatus::STATUS_USER_NO_ACTIVE_DOC : status;
				response.send_to(*message.source);
			}

			break;
		}
		case Message::MessageType::TYPE_USER_LOGIN:
		{
			UserDatabase database = UserDatabase::get_instance();

			try
			{
				// log in / get the user id
				message.source->user_id = database.check(message.get_name_string(), message.hash);
			}
			catch (userdatabase_errors::UserDoesntExistError)
			{ response.status = Message::MessageStatus::STATUS_USER_NOT_EXIST; }
			catch (userdatabase_errors::InvalidPasswordError)
			{ response.status = Message::MessageStatus::STATUS_USER_WRONG_PASSWORD; }
			catch (userdatabase_errors::Failure)
			{ response.status = Message::MessageStatus::STATUS_DB_ERROR; }

			// send response message
			response.send_to(*message.source);

			// broadcast user join notification if login was successful
			if (response.status == Message::MessageStatus::STATUS_OK)
			{
				Message announcement;
				announcement.type = Message::MessageType::TYPE_USER_JOIN;
				announcement.id = message.source->user_id;
				announcement.name = message.name;

				NetworkInterface::get_current_instance().broadcast_message(announcement);
			}

			break;
		}
		case Message::MessageType::TYPE_USER_LOGOUT:
		{
			// send simple response
			response.send_to(*message.source);

			// close the connection
			NetworkInterface::get_current_instance().disconnect_client(*message.source);
			
			break;
		}
		case Message::MessageType::TYPE_CLIENT_DISCONNECT:
		{
			// close the user's documents
			close_client_documents(message.source->user_id);

			// broadcase user quit notification
			Message announcement;
			announcement.type = Message::MessageType::TYPE_USER_QUIT;
			announcement.id = message.source->user_id;
			NetworkInterface::get_current_instance().broadcast_message(announcement);

			break;
		}
		default: break;
	}
}
