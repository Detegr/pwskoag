#pragma once
#include "dtglib/Network.h"
#include "physicalentity.h"
#include "bullet.h"
#include "timer.h"
#include <list>

using namespace dtglib;
class C_Connection
{
	friend class C_ConnectionPool;
	private:
		C_Connection* m_Next;
		C_Connection* m_Prev;
		C_Entity*	m_Entity;
		C_Timer		m_ShootTimer;
		std::list<C_Bullet*> m_Bullets;
		unsigned char m_KeyVec;
		bool m_Pending;
	public:
		C_IpAddress m_Ip;
		ushort		m_Port;
		C_Connection(const C_IpAddress& ip, ushort port) :
			m_Next(NULL), m_Prev(NULL), m_Entity(NULL), m_KeyVec(0), m_Pending(true), m_Ip(ip), m_Port(port) {}
		void SetEntity(C_Entity* e) {m_Entity=e;}
		C_Entity* GetEntity() const { return m_Entity; }
		void SetKeys(unsigned char keyvec) { m_KeyVec=keyvec; }
		unsigned char GetKeys() const { return m_KeyVec; }
		void Pending(bool p) {m_Pending=p;}
		bool Pending() const {return m_Pending;}
};

class C_ConnectionPool
{
	private:
		C_Connection* m_Head;
		C_Connection* m_Tail;
	public:
		C_ConnectionPool() : m_Head(0), m_Tail(0) {}
		C_Connection* M_Add(C_Connection* c);
		void M_Remove(C_Connection* c);
		C_Connection* M_Exists(const C_IpAddress& ip, ushort port) const;
		C_Connection* M_Head() const { return m_Head; }
		void M_SendToAll(C_UdpSocket& sock, C_Packet& p) const;
};
