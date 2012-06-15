package de.teamone.cte;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.SocketChannel;
import java.util.LinkedList;
import java.util.List;

public class NetworkInterface implements Runnable
{
	private final String host;
	private final int port;
	
	private List<NetworkMessageHandler> messageHandlers;
	
	private SocketChannel server;
	
	/**
	 * Standard constructor.
	 * The arguments specify the server to connect to with the connect method.
	 * @param host - server host
	 * @param port - server port
	 */
	public NetworkInterface(String host, int port)
	{
		this.host = host;
		this.port = port;
		
		this.messageHandlers = new LinkedList<NetworkMessageHandler>();
		this.server = null;
	}
	
	/**
	 * Adds a NetworkMessageHandler. It will be called for each received message.
	 * If the handler has already been added it will be added another time and thus be
	 * called multiple times.
	 * @param handler
	 */
	public void addMessageHandler(NetworkMessageHandler handler)
	{ messageHandlers.add(handler); }
	
	/**
	 * Builds up a connection to the server.
	 * @throws CTEException - if the connection failed
	 */
	public void connect()
	throws CTEException
	{
		// check if we are already connected
		if (server != null)
		{
			throw new CTEException("already connected",
				CTEException.ExceptionType.ALREADY_CONNECTED);
		}
		
		// create and open a socket channel to the server
		try
		{ server = SocketChannel.open(new InetSocketAddress(host, port)); }
		catch (Exception e)
		{
			throw new CTEException("failed to connect to " + host + ":" + port,
				CTEException.ExceptionType.CONNECTION_FAILED, e);
		}
	}
	
	/**
	 * Disconnects from the server.
	 * @throws CTEException - if this NetworkInterface is not even connected to a
	 * 		server or the SocketChannel could not be closed for some reason
	 */
	public void disconnect()
	throws CTEException
	{
		// check if we aren't connected
		if (server == null)
		{
			throw new CTEException("not connected",
				CTEException.ExceptionType.NOT_CONNECTED);
		}
		
		// close the socket channel
		try
		{ server.close(); }
		catch (IOException e)
		{
			throw new CTEException("failed to disconnect from " + host + ":" + port,
				CTEException.ExceptionType.CONNECTION_FAILED, e);
		}
	}
	
	/**
	 * Removes a previously added NetworkMessageHandler. Removes only the first
	 * occurrence so if the handler has been added multiple times it will still get
	 * called when a message has been received unless all occurrences of the handler
	 * got removed.
	 * @param handler
	 */
	public void removeMessageHandler(NetworkMessageHandler handler)
	{ messageHandlers.remove(handler); }
	
	/**
	 * Blocks until a complete message was received.
	 * Returns immediately, if this NetworkInterface is not yet connected.
	 * Ignores invalid message types and read errors.
	 */
	@Override
	public void run()
	{
		// check if we're connected
		if (server == null)
		{ return; }
		
		// receive message
		// TODO: proper exception handling?
		Message message;
		try
		{ message = Message.receiveFrom(server); }
		catch (CTEException e)
		{ return; }
		catch (IOException e)
		{ return; }
		
		// invoke message handlers
		for (NetworkMessageHandler handler: messageHandlers)
		{ handler.handleMessage(message); }
	}
	
	/**
	 * Sends a message to the server.
	 * @param message
	 * @throws CTEException
	 * 		if this NetworkInterface is not connected
	 * 		see Message.send_to(WritableByteChannel) for further reasons
	 */
	public void sendMessage(Message message)
	throws CTEException
	{
		if (server == null)
		{
			throw new CTEException("not connected",
				CTEException.ExceptionType.NOT_CONNECTED);
		}
		message.send_to(server);
	}
}