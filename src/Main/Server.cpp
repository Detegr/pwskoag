#include <Network/Network.h>
#include <Network/Server.h>
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
	#ifdef _WIN32
		C_SocketInitializer init;
	#endif
	signal(SIGINT, sighandler);
	TcpServer s(55555);
	UdpServer u(&s, 55556);
	std::cout << "Starting server." << std::endl;
	s.Start();
	u.Start();
	while(!stopNow) msSleep((unsigned int)~0);
	u.Stop();
	s.Stop();
	return 0;
}
