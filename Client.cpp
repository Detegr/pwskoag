#include <iostream>
#include <signal.h>
#include "Network.h"
#include "Event.h"
#include "Graphics.h"

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
	c.AutoSend(Network::Command::String, &a);
	c.Connect("localhost", 55555);
	msSleep(5000);
	a="MODIFIED STRING :O";
	msSleep(5000);
	c.Disconnect();
}
