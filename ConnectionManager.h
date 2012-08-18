#pragma once
#include <dtglib/Network.h>

using namespace dtglib;
class C_Connection
{
	friend class C_ConnectionPool;
	private:
		C_Connection* m_Next;
		C_Connection* m_Prev;
		C_IpAddress m_Ip;
		ushort		m_Port;
		C_Packet	m_Packet;
	public:
		C_Connection(const C_IpAddress& ip, ushort port) :
			m_Next(0), m_Prev(0), m_Ip(ip), m_Port(port), m_Packet() {}
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
		bool M_Exists(const C_IpAddress& ip, ushort port) const;
};
