/**
	file: main_network_message_handler.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Monday, 11th June 2012
**/

#include <unordered_map>

#include "Client.h"
#include "Document.h"
#include "Message.h"
#include "NetworkInterface.h"

// auxiliary functions
namespace
{
	typedef std::shared_ptr<Document> DocumentSptr;

	std::unordered_map<int32_t, DocumentSptr> doc_by_id;
	std::unordered_map<std::string, DocumentSptr> doc_by_name;

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
	DocumentSptr open_document(const std::string &name)
	{
		auto iter = doc_by_name.find(name);
		if (iter == doc_by_name.end())
		{
			try
			{
				DocumentSptr result = DocumentSptr(new Document(Document::open(name)));
				doc_by_id[result->get_id()] = doc_by_name[name] = result;
				return result;
			}
			catch (document_errors::DocumentDoesntExistError)
			{ throw Message::MessageStatus::STATUS_DOC_NOT_EXIST; }
			catch (document_errors::DocumentError)
			{ throw Message::MessageStatus::STATUS_IO_ERROR; }
		}
		
		return iter->second;
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
};

void main_network_message_handler(const Message &message)
{
	
	// initialize response Message
	Message response;
	response.status = Message::MessageStatus::STATUS_OK;
	response.type = message.type;

	switch (message.type)
	{
		case Message::TYPE_DOC_ACTIVATE:
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
		case Message::TYPE_DOC_CREATE:
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
		case Message::TYPE_DOC_DELETE:
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
		case Message::TYPE_DOC_OPEN:
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
		case Message::TYPE_DOC_SAVE:
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
				NetworkInterface *net = NetworkInterface::get_current_instance();

				if (net != NULL)
				{
					Message announcement;
					announcement.type = Message::MessageType::TYPE_STATUS;
					announcement.status = Message::MessageStatus::STATUS_DOC_SAVED;
				
					net->broadcast_message(announcement, message.id);
				}
			}

			break;
		}
		case Message::TYPE_SYNC_BYTE:
			// TODO put this in a function!
			DocumentSptr doc;
			response.type = Message::MessageType::TYPE_STATUS;

			// check if client has an active document at all and it's opened
			if (message.source->active_document < 1)
			{ response.status = Message::MessageStatus::STATUS_USER_NO_ACTIVE_DOC; }
			else
			{
				try
				{
					// get document
					doc = get_document(message.source->active_document);

					// check if cursor position is known
					if (message.source->cursor < 0)
					{ response.status = Message::MessageStatus::STATUS_USER_CURSOR_UNKNOWN; }
					else
					{
						// get contents and check if cursor position is in bounds
						auto contents = doc->get_contents();
						if (message.source->cursor >= contents.size())
						{
							response.status =
								Message::MessageStatus::STATUS_USER_CURSOR_OUT_OF_BOUNDS;
						}
						else
						{
							// sync change to all clients
							Message sync;
							sync.type = Message::MessageType::TYPE_SYNC_BYTE;
							sync.bytes = message.bytes;
							sync.position = message.source->cursor;

							NetworkInterface::get_current_instance()->broadcast_message(sync,
								message.source->active_document);

							// apply change to document
							contents.insert(contents.begin() + sync.position, bytes[0]);

							break;
						}
					}
				}
				catch (Message::MessageStatus)
				{ response.status = Message::MessageStatus::STATUS_USER_NO_ACTIVE_DOC; }
			}

			response.send_to(*message.source);

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
