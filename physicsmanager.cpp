#include "physicsmanager.h"
#include "serversingleton.h"
#include <typeinfo>

float32 C_PhysicsManager::m_TimeStep;

C_PhysicsManager::C_PhysicsManager() :
	m_World(b2Vec2(0.0f, 0.0f))
{
	std::cout << "Initializing physicsmanager...";
	C_PhysicsManager::m_TimeStep = 1.0f/30.0f;
	m_World.SetAllowSleeping(true);
	std::cout << "OK!" << std::endl;
}
C_PhysicsManager::~C_PhysicsManager()
{
	std::cout << "Destroying physicsmanager...";
	for(std::vector<C_Entity*>::iterator it=m_Bodies.begin(); it!=m_Bodies.end(); ++it)
	{
		delete *it;
	}
	delete m_ContactListener;
	std::cout << "OK!" << std::endl;
}
C_Entity* C_PhysicsManager::M_CreateDynamicEntity(const C_Model& m, float scale)
{
	m_Bodies.push_back(new C_Entity(m_World,m,scale,true));
	return m_Bodies.back();
}
C_Entity* C_PhysicsManager::M_CreateDynamicEntity(unsigned short id, const C_Model& m, float scale)
{
	m_Bodies.push_back(new C_Entity(id, m_World,m,scale,true));
	return m_Bodies.back();
}
C_Bullet* C_PhysicsManager::M_CreateBullet(const C_Model& m, float scale)
{
	C_Bullet* b=new C_Bullet(m_World,m,scale,true);
	m_Bodies.push_back(b);
	return b;
}
C_Entity* C_PhysicsManager::M_CreateStaticEntity(const C_Model& m, float scale)
{
	m_Bodies.push_back(new C_Entity(m_World,m,scale,false));
	return m_Bodies.back();
}
C_Entity* C_PhysicsManager::M_CreateStaticEntity(unsigned short id, const C_Model& m, float scale)
{
	m_Bodies.push_back(new C_Entity(id, m_World,m,scale,false));
	return m_Bodies.back();
}

void C_PhysicsManager::M_Simulate(double timestep)
{
	m_World.Step(timestep, m_VelocityIterations, m_PositionIterations);
}

void C_PhysicsManager::M_DestroyEntity(C_Entity* e)
{
	bool d=false;
	for(std::vector<C_Entity*>::iterator it=m_Bodies.begin(); it!=m_Bodies.end(); ++it)
	{
		if((*it) == e)
		{
			it=m_Bodies.erase(it);
			d=true;
			break;
		}
	}
	if(d) delete e;
}

void C_PhysicsManager::M_SetContactListener(b2ContactListener* cl)
{
	m_ContactListener=cl;
	m_World.SetContactListener(m_ContactListener);
}

C_Entity* C_PhysicsManager::GetEntity(unsigned short id) const
{
	for(std::vector<C_Entity*>::const_iterator it=m_Bodies.begin(); it!=m_Bodies.end(); ++it)
	{
		if((*it)->m_Id == id) return *it;
	}
	return NULL;
}

void C_PhysicsManager::ApplyPlayerForces(C_Entity* e, unsigned char keyvec)
{
	b2Body* b=e->M_Body();
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
		float32 a = b->GetAngle();
		b2Vec2 force = b2Vec2(-sin(a), cos(a));
		force *= 6.0f;
		b->ApplyForceToCenter(force);
	}
	/*
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
	*/
}
