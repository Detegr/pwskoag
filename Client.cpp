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
	c.Connect("localhost", 55555);
	while(!stopNow){sleep(1000);}
	c.Disconnect();
	std::cout << "Disconnected." << std::endl;
}
