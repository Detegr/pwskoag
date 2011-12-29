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
	TcpClient c;
	UdpClient u(&c);
	if(c.M_Connect("127.0.0.1", 55555))
	{
		if(u.M_Connect("127.0.0.1", 55556))
		{
			msSleep(1000);
			c.Append(String, std::string("This is a string appended from main loop."));
			msSleep(1000);
			u.Append(String, std::string("This is a string appended from main loop. Sent using UDP"));
			msSleep(1000);
			u.M_Disconnect();
		}
		c.M_Disconnect();
	}
}
