#include "Server.h"

C_Server::C_Server(ushort port) : m_Socket(port), m_Pool() {}

void C_Server::M_Start()
{
	m_Socket.M_Bind();
	C_Packet p;
	C_IpAddress ip;
	ushort port;
	while(1)
	{
		p.M_Clear();
		if(m_Socket.M_Receive(p, &ip, &port))
		{
			if(!m_Pool.M_Exists(ip,port))
			{
				m_Pool.M_Add(new C_Connection(ip, port));
				std::cout << "New connection" << std::endl;
			}
			else std::cout << "Old connection" << std::endl;
		}
	}
}
