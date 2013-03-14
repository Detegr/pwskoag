#include "ConnectionManager.h"
#include "serversingleton.h"
#include "networkenum.h"

using namespace dtglib;

C_Connection* C_ConnectionPool::M_Add(C_Connection* c)
{
	if(!m_Head)
	{
		m_Head=c;
		m_Tail=c;
		return c;
	}
	if(m_Tail)
	{
		m_Tail->m_Next=c;
		c->m_Prev=m_Tail;
		m_Tail=c;
	}
	return c;
}

void C_ConnectionPool::M_Remove(C_Connection* c)
{
	if(c==m_Head)
	{
		if(c->m_Next)
		{
			m_Head=c->m_Next;
			m_Head->m_Prev=NULL;
		}
		else m_Head=NULL;
	}
	else if(c==m_Tail)
	{
		m_Tail=c->m_Prev;
		c->m_Prev->m_Next = NULL;
	}
	else
	{
		c->m_Prev->m_Next=c->m_Next;
		c->m_Next->m_Prev=c->m_Prev;
	}
	C_Singleton::M_PhysicsManager()->M_DestroyEntity(c->M_GetEntity());
	delete c;
}

C_Connection* C_ConnectionPool::M_Exists(const C_IpAddress& ip, ushort port) const
{
	for(C_Connection* c=m_Head; c; c=c->m_Next)
	{
		if(c->m_Ip == ip && c->m_Port == port) return c;
	}
	return NULL;
}

void C_ConnectionPool::M_SendToAll(C_UdpSocket& sock, C_Packet& p) const
{
	for(C_Connection* c=m_Head; c; c=c->m_Next)
	{
		if(c->M_Pending()) continue;

		for(std::list<C_Bullet*>::iterator it=c->m_Bullets.begin(); it!=c->m_Bullets.end(); ++it)
		{
			if((*it)->M_Hits() >= C_Entity::BULLET_HITS)
			{
				p << (unsigned char)NET::EntityDeleted << (*it)->M_Id();
				C_Singleton::M_PhysicsManager()->M_DestroyEntity(*it);
				it=c->m_Bullets.erase(it);
				continue;
			}
			*(*it) >> p;
		}
		sock.M_Send(p, c->m_Ip, c->m_Port);
	}
}

void C_ConnectionPool::HandlePlayerInput(C_Connection* c, C_Packet&)
{
	C_Singleton::M_PhysicsManager()->ApplyPlayerForces(c->M_GetEntity(), c->M_GetKeys());
}
