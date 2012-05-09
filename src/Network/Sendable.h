#pragma once
#include <dtglib/Network.h>

using namespace dtglib;
namespace pwskoag
{
	class C_Sendable
	{
		private:
		public:
			C_TcpSocket* m_Tcp;
			C_UdpSocket* m_Udp;
			C_Packet* m_Packet;
			C_Sendable() : m_Tcp(NULL), m_Packet(NULL) {}
			C_Sendable(C_TcpSocket* s, C_Packet* p) : m_Tcp(s), m_Packet(p) {}
			virtual ~C_Sendable() {}
			virtual void M_Send()=0;
			virtual void M_SendUdp(C_UdpSocket& s)=0;
	};
}