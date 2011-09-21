#include "Network.h"
#include <iostream>
#include <signal.h>

bool stopNow=false;

void sighandler(int)
{
	stopNow=true;
}

int main()
{
	signal(SIGINT, sighandler);
	Network::TcpServer s(55555);
	//Network::UdpServer u(&s, 55556);
	std::cout << "Starting server." << std::endl;
	s.Start();
	//u.Start();
	while(!stopNow) sleep((uint)~0);
	//u.Stop();
	s.Stop();
	return 0;
}
