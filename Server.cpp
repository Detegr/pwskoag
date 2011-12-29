#include "Network.h"
#include <iostream>
#include <signal.h>

using namespace pwskoag;

bool stopNow=false;

void sighandler(int)
{
	stopNow=true;
}

int main()
{
	signal(SIGINT, sighandler);
	Packet p;
	TcpServer s(55555);
	UdpServer u(&s, 55556);
	std::cout << "Starting server." << std::endl;
	s.Start();
	u.Start();
	while(!stopNow) sleep((uint)~0);
	u.Stop();
	s.Stop();
	return 0;
}
