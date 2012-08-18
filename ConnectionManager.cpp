#include "ConnectionManager.h"

void C_ConnectionPool::M_Add(C_Connection* c)
{
	if(!m_Head)
	{
		m_Head=c;
		m_Tail=c;
	}
	if(m_Tail)
	{
		m_Tail->m_Next=c;
		c->m_Prev=m_Tail;
		m_Tail=c;
	}
}

void C_ConnectionPool::M_Remove(C_Connection* c)
{
	if(c==m_Head) m_Head=c->m_Next;
	if(c==m_Tail) m_Tail=c->m_Prev;
	c->m_Prev->m_Next=c->m_Next;
	c->m_Next->m_Prev=c->m_Prev;
	delete c;
}

bool C_ConnectionPool::M_Exists(const C_IpAddress& ip, ushort port) const
{
	for(C_Connection* c=m_Head; m_Head!=m_Tail; c=c->m_Next)
	{
		if(c->m_Ip == ip && c->m_Port == port) return true;
	}
	return false;
}
