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

		C_Entity* e=c->M_GetEntity();
		b2Body* b=e->M_Body();
		unsigned char keyvec=c->M_GetKeys();
		if(keyvec & 0x1)
		{
			b->SetAngularVelocity(-0.3f);
		}
		else if(keyvec & 0x2)
		{
			b->SetAngularVelocity(0.3f);
		}
		else b->SetAngularVelocity(0.0f);
		if(keyvec & 0x8)
		{
			float32 a = c->M_GetEntity()->M_Body()->GetAngle();
			b2Vec2 force = b2Vec2(-sin(a), cos(a));
			force *= 0.6f;
			b->ApplyForceToCenter(force);
		}
		if(keyvec & 0x20)
		{
			if(c->m_ShootTimer.M_Get() > .25f)
			{
				c->m_ShootTimer.M_Reset();
				C_PhysicsManager* pm=C_Singleton::M_PhysicsManager();
				C_ModelManager* m=C_Singleton::M_ModelManager();
				C_Bullet* b=pm->M_CreateBullet(m->M_Get("bullet"), 0.05f);
				b2Body* body=c->M_GetEntity()->M_Body();
				b2Vec2 pos=body->GetPosition();
				float angle=body->GetAngle();
				float speed=20.0f;
				b2Vec2 newv(-sin(angle), cos(angle));
				newv*=speed;
				b2Vec2 align=b2Vec2(-sin(angle), cos(angle));
				//align*=0.2;
				pos+=align;
				b->M_Body()->SetTransform(pos, 0.0f);
				b->M_Body()->SetLinearVelocity(newv);
				b->M_Body()->SetBullet(true);
				b->M_DumpFullInstance(p);
				c->m_Bullets.push_back(b);
			}
		}
		sock.M_Send(p, c->m_Ip, c->m_Port);
	}
}
