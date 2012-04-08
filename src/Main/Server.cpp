#include <Network/Network.h>
#include <Network/Server.h>
#include <iostream>
#include <signal.h>

using namespace pwskoag;

bool stopNow=false;

void sighandler(int);
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
	std::cout << "Starting TCP server." << std::endl;
	s.Start();
	std::cout << "Starting UDP server." << std::endl;
	u.Start();
	while(!stopNow)
	{
		s.M_PlayerLock(true);
		t_Entities& plrs=s.M_Players();
		for(t_Entities::iterator it=plrs.begin(); it!=plrs.end(); ++it)
		{
		}
		s.M_PlayerLock(false);
		g_Sleep(10);
	}
	u.Stop();
	s.Stop();
	return 0;
}
