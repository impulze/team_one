/**
 * file: NetworkInterface.h
 * author: Maximilian Lasser [max.lasser@online.de]
 * created: Friday, 11th May 2012
 **/

#include <sys/socket.h>

class NetworkInterface
{
	public:
		NetworkInterface(int port);
	
	private:
		int listener;
};
