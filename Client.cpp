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
	/*
	signal(SIGINT, sighandler);
	Network::TcpClient c;
	Network::UdpClient u;
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
	Network::TcpClient c;
	std::string a="This string is sent automatically.";
	std::string b="String no. 2";
	/*
	c.AutoSend(Network::Command::String, &a);
	c.AutoSend(Network::Command::String, &b);
	*/
	c.Connect("localhost", 55555);
	msSleep(2000);
	std::cout << "Jee" << std::endl;
	msSleep(2000);
	c.Disconnect();
}
