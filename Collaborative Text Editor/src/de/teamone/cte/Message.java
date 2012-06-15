package de.teamone.cte;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.util.Arrays;
public class Message
{
	final static int
	FIELD_SIZE_BYTE = 1,
	FIELD_SIZE_ID = 4,
	FIELD_SIZE_DOC_NAME = 128,
	FIELD_SIZE_HASH = 20,
	FIELD_SIZE_SIZE = 4,
	FIELD_SIZE_STATUS = 1,
	FIELD_SIZE_TYPE = 1,
	FIELD_SIZE_USER_NAME = 64;
	static enum MessageStatus
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
	static enum MessageType
	{
		TYPE_INVALID, // invalid message type
		TYPE_DOC_ACTIVATE, // user activates/switches to doc (id, hash)
		TYPE_DOC_CREATE, // user creates doc (name)
		TYPE_DOC_DELETE, // user deletes doc (name)
		TYPE_DOC_LIST, // document list
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
		TYPE_USER_QUIT; // server -> client only (a user disconnected)
		
		/**
		 * Calculates the total message size for a message of this type.
		 * @param fromServer - whether the message is from the server or the client
		 * @param dynamic - used in only two cases:
		 * 		TYPE_DOC_LIST: amount of document names in the list (from server)
		 * 		TYPE_SYNC_MULTIBYTE: payload size in Bytes
		 * 		Needs to be set to 1 if unneeded!
		 * @return total message size for the respective type and direction in Bytes
		 */
		public int getMessageSize(boolean fromServer, int dynamic)
		{
			// type is always there
			int result = FIELD_SIZE_TYPE;
			
			// byte field
			if (this == TYPE_SYNC_BYTE ||
				this == TYPE_SYNC_MULTIBYTE)
			{ result += dynamic * FIELD_SIZE_BYTE; }
			
			// id field (user id or document id)
			if (this == TYPE_DOC_ACTIVATE ||
				this == TYPE_DOC_SAVE ||
				(fromServer && (
					this == TYPE_DOC_OPEN ||
					this == TYPE_USER_JOIN ||
					this == TYPE_USER_QUIT)
				))
			{ result += FIELD_SIZE_ID; }
			
			// document name field
			if (this == TYPE_DOC_CREATE ||
				this == TYPE_DOC_DELETE ||
				this == TYPE_DOC_OPEN ||
				(fromServer && (
					this == TYPE_DOC_LIST)
				))
			{ result += dynamic * FIELD_SIZE_DOC_NAME; }
			
			// hash field
			if (!fromServer && (
				this == TYPE_DOC_ACTIVATE ||
				this == TYPE_USER_LOGIN))
			{ result += FIELD_SIZE_HASH; }
			
			// size field
			if (this == TYPE_SYNC_DELETION ||
				this == TYPE_SYNC_MULTIBYTE ||
				(fromServer && (
					this == TYPE_DOC_LIST ||
					this == TYPE_SYNC_BYTE)
				) ||
				(!fromServer && (
					this == TYPE_SYNC_CURSOR)
				))
			{ result += FIELD_SIZE_SIZE; }
			
			// size field (for messages with two size fields)
			if (this == TYPE_SYNC_DELETION ||
				(fromServer && (
					this == TYPE_SYNC_MULTIBYTE)
				))
			{ result += FIELD_SIZE_SIZE; }
			
			// status field
			if (fromServer && !(
				this == TYPE_DOC_LIST ||
				this == TYPE_SYNC_BYTE ||
				this == TYPE_SYNC_DELETION ||
				this == TYPE_SYNC_MULTIBYTE ||
				this == TYPE_USER_JOIN ||
				this == TYPE_USER_QUIT))
			{ result += FIELD_SIZE_STATUS; }
			
			// user name field
			if ((fromServer && (
					this == TYPE_USER_JOIN)
				) ||
				(!fromServer && (
					this == TYPE_USER_LOGIN)
				))
			{ result += FIELD_SIZE_USER_NAME; }
			
			return result;
		}
		
		public int getMessageSize(boolean fromServer)
		{ return getMessageSize(fromServer, 1); }
	}
	
	public byte[] bytes;
	public int id;
	public int length;
	public String name;
	public int position;
	public MessageStatus status;
	public final MessageType type;
	
	public Message(MessageType type)
	{ this.type = type; }
	
	/**
	 * Receives a message from the server. The given channel should be readable at
	 * all, i.e. there should be a message in the reception queue.
	 * @param channel
	 * @throws CTEException - if the type of the received message is invalid
	 * @throws IOException - if an error occurred while reading from the channel
	 */
	public static Message receiveFrom(ReadableByteChannel channel)
	throws CTEException, IOException
	{
		byte[] nameBuffer;
		
		// initialize buffer for reading message type
		ByteBuffer buffer = ByteBuffer.allocate(FIELD_SIZE_TYPE);
		channel.read(buffer);
		//ByteBuffer buffer = readField(channel, FIELD_SIZE_TYPE);
		
		// read Message type
		MessageType type;
		try
		{ type = MessageType.values()[buffer.get()]; }
		catch (IndexOutOfBoundsException e)
		{
			throw new CTEException("invalid message type",
					CTEException.ExceptionType.INVALID_TYPE);
		}
		Message result = new Message(type);
		
		// initialize buffer for reading the rest of the message
		if (type == MessageType.TYPE_SYNC_MULTIBYTE)
		{ buffer = ByteBuffer.allocate(type.getMessageSize(true, 0)); }
		else
		{ buffer = ByteBuffer.allocate(type.getMessageSize(true)); }
		
		// read the message
		channel.read(buffer);
		
		// read first field
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
			try
			{ result.status = MessageStatus.values()[buffer.get()]; }
			catch (IndexOutOfBoundsException e)
			{ result.status = MessageStatus.STATUS_UNKNOWN; }
			break;
		case TYPE_DOC_LIST:
			result.length = buffer.getInt();
			break;
		case TYPE_SYNC_BYTE:
		case TYPE_SYNC_DELETION:
		case TYPE_SYNC_MULTIBYTE:
			result.position = buffer.getInt();
			break;
		case TYPE_USER_JOIN:
		case TYPE_USER_QUIT:
			result.id = buffer.getInt();
			break;
		}

		// read second field
		switch (type)
		{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_OPEN:
		case TYPE_DOC_SAVE:
			result.id = buffer.getInt();
			break;
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
			buffer.get(nameBuffer = new byte[FIELD_SIZE_DOC_NAME]);
			result.name = new String(nameBuffer);
			break;
		case TYPE_DOC_LIST:
			result.bytes = new byte[result.length * FIELD_SIZE_DOC_NAME];
			for (int i = 0; i < result.length; ++i)
			{
				buffer.get(result.bytes, i * FIELD_SIZE_DOC_NAME,
					FIELD_SIZE_DOC_NAME);
			}
			break;
		case TYPE_SYNC_BYTE:
			result.bytes = new byte[] { buffer.get() };
			break;
		case TYPE_SYNC_DELETION:
		case TYPE_SYNC_MULTIBYTE:
			result.length = buffer.getInt();
			break;
		case TYPE_USER_JOIN:
			buffer.get(nameBuffer = new byte[FIELD_SIZE_USER_NAME]);
			result.name = new String(nameBuffer);
			break;
		}
		
		// read third field
		switch (type)
		{
		case TYPE_DOC_OPEN:
			buffer.get(nameBuffer = new byte[FIELD_SIZE_DOC_NAME]);
			result.name = new String(nameBuffer);
			break;
		case TYPE_SYNC_MULTIBYTE:
			buffer.get(result.bytes = new byte[result.length]);
			break;
		}
		
		return result;
	}
	
	/**
	 * Sends this message to the given channel.
	 * @param channel
	 * @throws CTEException - if the type is invalid
	 */
	public void send_to(WritableByteChannel channel)
	throws CTEException
	{
		// initialize buffer
		ByteBuffer buffer;
		if (type == MessageType.TYPE_SYNC_MULTIBYTE)
		{ buffer = ByteBuffer.allocate(type.getMessageSize(false, length)); }
		else
		{ buffer = ByteBuffer.allocate(type.getMessageSize(false)); }
		
		// append message type
		buffer.put((byte)type.ordinal());
		
		// append first field
		switch (type)
		{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_SAVE:
			buffer.putInt(id);
			break;
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
		case TYPE_DOC_OPEN:
			byte[] nameBytes = Arrays.copyOf(name.getBytes(), FIELD_SIZE_DOC_NAME);
			Arrays.fill(nameBytes, name.length(), nameBytes.length, (byte)0x00);
			buffer.put(nameBytes);
			break;
		case TYPE_DOC_LIST:
		case TYPE_USER_LOGOUT:
			break;
		case TYPE_SYNC_BYTE:
			buffer.put(bytes[0]);
			break;
		case TYPE_SYNC_DELETION:
			buffer.putInt(position);
			buffer.putInt(length);
			break;
		case TYPE_SYNC_MULTIBYTE:
			buffer.putInt(length);
			
		case TYPE_USER_LOGIN:
		default:
			throw new CTEException("invalid message type for sending",
					CTEException.ExceptionType.INVALID_TYPE);
		}
	}
}