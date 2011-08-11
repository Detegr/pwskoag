#include <string>
#include "Network.h"
int main()
{
	Network::TcpClient c;
	c.Connect("localhost", 55555);
	std::string str("Hello TcpClient!");
	c.Append(Network::Command::String, str);
	c.Send();
	c.Disconnect();
}
