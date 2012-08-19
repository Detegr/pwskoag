#include "ConnectionManager.h"
#include <iostream>

void C_ConnectionPool::M_Add(C_Connection* c)
{
	if(!m_Head)
	{
		m_Head=c;
		m_Tail=c;
		return;
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
	else if(c==m_Tail) m_Tail=c->m_Prev;
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
