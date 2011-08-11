#include <string>
#include "Network.h"
int main()
{
	Network::TcpClient c;
	c.Connect("localhost", 55555);
	for(int i=0; i<50; ++i)
	{
		c.Send(Network::Command::Heartbeat);
		usleep(100000);
	}
	c.Disconnect();
}
