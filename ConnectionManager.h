#pragma once
#include "dtglib/Network.h"

using namespace dtglib;
class C_Connection
{
	friend class C_ConnectionPool;
	private:
		C_Connection* m_Next;
		C_Connection* m_Prev;
		C_Packet	m_Packet;
	public:
		C_IpAddress m_Ip;
		ushort		m_Port;
		C_Connection(const C_IpAddress& ip, ushort port) :
			m_Next(NULL), m_Prev(NULL), m_Packet(), m_Ip(ip), m_Port(port) {}
};

class C_ConnectionPool
{
	private:
		C_Connection* m_Head;
		C_Connection* m_Tail;
	public:
		C_ConnectionPool() : m_Head(0), m_Tail(0) {}
		void M_Add(C_Connection* c);
		void M_Remove(C_Connection* c);
		C_Connection* M_Exists(const C_IpAddress& ip, ushort port) const;
		C_Connection* M_Head() const { return m_Head; }
		void M_SendToAll(C_UdpSocket& sock, C_Packet& p) const;
};
