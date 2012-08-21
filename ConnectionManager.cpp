#include "ConnectionManager.h"
#include <iostream>

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
		C_Entity* e=c->M_GetEntity();
		b2Body* b=e->M_Body();
		unsigned char keyvec=c->M_GetKeys();
		if(keyvec & 0x1)
		{
			b->SetAngularVelocity(-3.0f);
		}
		else if(keyvec & 0x2)
		{
			b->SetAngularVelocity(3.0f);
		}
		else b->SetAngularVelocity(0.0f);
		if(keyvec & 0x8)
		{
			float32 a = c->M_GetEntity()->M_Body()->GetAngle();
			b2Vec2 force = b2Vec2(-sin(a), cos(a));
			force *= 6.0f;
			b->ApplyForceToCenter(force);
		}
		sock.M_Send(p, c->m_Ip, c->m_Port);
	}
}
