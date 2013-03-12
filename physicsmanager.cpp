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

void C_PhysicsManager::M_Simulate()
{
	double dt=C_Singleton::M_Timer()->M_Get()/0.030;
	m_World.Step(m_TimeStep*dt, m_VelocityIterations, m_PositionIterations);
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
