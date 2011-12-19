#include <iostream>
#include <signal.h>
#include "Network.h"

using namespace pwskoag;

bool stopNow=false;
void sighandler(int)
{
	stopNow=true;
}
int main()
{
	/*
	signal(SIGINT, sighandler);
	Network::TcpClient c;
	c.Connect("localhost", 55555);
	u.Connect("localhost", 55556);
	while(!stopNow){sleep(1000);}
	c.Disconnect();
	u.Disconnect();
	std::cout << "Disconnected." << std::endl;
	*/
	//Graphics::Renderer r(640,480);
	//EventManager e;
	//while(1) r.Draw();
	/*
	c.AutoSend(Network::Command::String, &a);
	c.AutoSend(Network::Command::String, &b);
	*/
	TcpClient c;
	//Network::UdpClient u;
	c.M_Connect("localhost", 55555);
	//u.Connect("localhost", 55556);
	msSleep(4000);
	//u.Disconnect();
	c.M_Disconnect();
}
