package de.teamone.cte;

import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.channels.SocketChannel;

public class NetworkInterface implements Runnable
{
	private final String host;
	private final int port;
	
	//private List<NetworkEventHandler> eventHandlers;
	private Socket server;
	
	public NetworkInterface(String host, int port)
	{
		this.host = host;
		this.port = port;
	}
	
	public void activateDocument(int documentId)
	{}
	
	/**
	 * Builds up a connection to the server and sends a login request.
	 * @param username
	 * @param password
	 * @throws CTEException - if the connection failed
	 */
	public void connect(String username, String password)
	throws CTEException
	{
		// create and open a socket channel to the server
		try
		{
			SocketChannel socketChannel =	
					SocketChannel.open(new InetSocketAddress(host, port));
		}
		catch (Exception e)
		{
			throw new CTEException(
					"failed to connect to " + host + ":" + port,
					CTEException.ExceptionType.CONNECTION_FAILED, 
					e
			);
		}
	}
	
	@Override
	public void run()
	{}
}