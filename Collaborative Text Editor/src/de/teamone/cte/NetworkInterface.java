package de.teamone.cte;

import java.net.InetAddress;
import java.net.Socket;
import java.util.List;

public class NetworkInterface implements Runnable
{
	private final InetAddress host;
	private final int port;
	
	private List<NetworkEventHandler> eventHandlers;
	private Socket server;
	
	public NetworkInterface(InetAddress host, int port)
	{
		this.host = host;
		this.port = port;
	}
	
	public void activateDocument(int documentId)
	{}
	
	public void connect(String username, String password)
	{
		
	}
	
	@Override
	public void run()
	{}
}