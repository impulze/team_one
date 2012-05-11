package de.teamone.cte;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.channels.SocketChannel;
import java.util.List;

public class NetworkInterface implements Runnable
{
	private final String host;
	private final int port;
	
	private List<NetworkEventHandler> eventHandlers;
	private Socket server;
	
	public NetworkInterface(String host, int port)
	{
		this.host = host;
		this.port = port;
	}
	
	public void activateDocument(int documentId)
	{}
	
	public void connect(String username, String password)
	throws IOException
	{
		SocketChannel socketChannel =	
			SocketChannel.open(new InetSocketAddress(host, port));
	}
	
	@Override
	public void run()
	{}
}