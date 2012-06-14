package de.teamone.cte;
import java.io.IOError;
import java.io.IOException;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
public class Message
{
	enum MessageStatus
	{
		STATUS_OK, // success
		STATUS_OK_CONTENTS_FOLLOWING, // multibyte message with doc contents following
		STATUS_DOC_ALREADY_EXIST, // doc does already exist
		STATUS_DOC_NOT_EXIST, // doc does not exist
		STATUS_DOC_SAVED, // doc was saved by another user
		STATUS_USER_NOT_EXIST, // username does not exist
		STATUS_USER_WRONG_PASSWORD, // password is wrong
		STATUS_USER_NO_ACTIVE_DOC, // user has no active doc
		STATUS_USER_CURSOR_UNKNOWN, // user cursor position is unknown
		STATUS_USER_CURSOR_OUT_OF_BOUNDS, // user cursor position is out of bounds
		STATUS_USER_LENGTH_TOO_LONG, // specified length is too long
		STATUS_NOT_OK, // anything but success
		STATUS_UNKNOWN // unknown/invalid status
	};
	enum MessageType
	{
		TYPE_INVALID, // invalid message type
		TYPE_DOC_ACTIVATE, // user activates/switches to doc (id, hash)
		TYPE_DOC_CREATE, // user creates doc (name)
		TYPE_DOC_DELETE, // user deletes doc (name)
		TYPE_DOC_OPEN, // user opens doc (name)
		TYPE_DOC_SAVE, // user saves doc (id)
		TYPE_STATUS, // server -> client only (general status announcement)
		TYPE_SYNC_BYTE, // user sends byte to insert at current pos (byte)
		TYPE_SYNC_CURSOR, // user sends new cursor position (position)
		TYPE_SYNC_DELETION, // user sends deletion (position, length)
		TYPE_SYNC_MULTIBYTE, // user sends byte sequence to insert at current
							 // position (length, payload)
		TYPE_USER_LOGIN, // user sends login credentials (name, hash)
		TYPE_USER_LOGOUT, // user logs out
		TYPE_USER_JOIN, // server -> client only (a new user connected)
		TYPE_USER_QUIT // server -> client only (a user disconnected)
	}
	final int
		FIELD_SIZE_BYTE = 1,
		FIELD_SIZE_ID = 4,
		FIELD_SIZE_DOC_NAME = 128,
		FIELD_SIZE_HASH = 20,
		FIELD_SIZE_SIZE = 8,
		FIELD_SIZE_STATUS = 1,
		FIELD_SIZE_TYPE = 1,
		FIELD_SIZE_USER_NAME = 64;
	
	public BigInteger position;
	public MessageStatus status;
	public MessageType type;
	
	public Message(MessageType type)
	{ this.type = type; }
	
	public void receiveFrom(ReadableByteChannel channel)
	throws CTEException, IOException
	{
		// read Message type
		ByteBuffer buffer = ByteBuffer.allocate(FIELD_SIZE_TYPE);
		channel.read(buffer);
		try
		{ type = MessageType.values()[buffer.get()]; }
		catch (IndexOutOfBoundsException e)
		{
			throw new CTEException("invalid message type",
					CTEException.ExceptionType.INVALID_TYPE);
		}
		
		// read first data
		switch (type)
		{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
		case TYPE_DOC_OPEN:
		case TYPE_DOC_SAVE:
		case TYPE_USER_LOGIN:
		case TYPE_USER_LOGOUT:
		case TYPE_STATUS:
			buffer = ByteBuffer.allocate(FIELD_SIZE_STATUS);
			channel.read(buffer);
			status = null;
			try
			{ status = MessageStatus.values()[buffer.get()]; }
			catch (IndexOutOfBoundsException e)
			{ status = MessageStatus.STATUS_UNKNOWN; }
			break;
		case TYPE_SYNC_BYTE:
			buffer = ByteBuffer.allocate(FIELD_SIZE_SIZE + 1);
			buffer.put((byte)0x00);
			channel.read(buffer);
			position = new BigInteger(buffer.array());
		}
	}
}