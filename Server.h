#pragma once
#include <dtglib/Network.h>
#include <iostream>
#include "ConnectionManager.h"

using namespace dtglib;
class C_Server
{
	private:
		C_UdpSocket m_Socket;
		C_ConnectionPool m_Pool;
	public:
		C_Server(ushort port);
		void M_Start();
};

int main()
{
	C_Server s(54300);
	s.M_Start();
}
