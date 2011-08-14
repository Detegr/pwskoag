#include <iostream>
#include <signal.h>
#include "Network.h"

bool stopNow=false;
void sighandler(int)
{
	stopNow=true;
}
int main()
{
	signal(SIGINT, sighandler);
	Network::TcpClient c;
	Network::UdpClient u;
	c.Connect("localhost", 55555);
	u.Connect("localhost", 55556);
	while(!stopNow){sleep(1000);}
	c.Disconnect();
	u.Disconnect();
	std::cout << "Disconnected." << std::endl;
}
