#include "physicsmanager.h"
#include "serversingleton.h"
#include <typeinfo>

C_PhysicsManager::C_PhysicsManager() :
	m_World(b2Vec2(0.0f, -5.0f))
{
	m_World.SetAllowSleeping(true);
}
C_PhysicsManager::~C_PhysicsManager()
{
	for(std::vector<C_Entity*>::iterator it=m_Bodies.begin(); it!=m_Bodies.end(); ++it)
	{
		delete *it;
	}
}
C_Entity* C_PhysicsManager::M_CreateDynamicEntity(const C_Model& m, float scale)
{
	m_Bodies.push_back(new C_Entity(m_World,m,scale,true));
	return m_Bodies.back();
}
C_Entity* C_PhysicsManager::M_CreateStaticEntity(const C_Model& m, float scale)
{
	m_Bodies.push_back(new C_Entity(m_World,m,scale,false));
	return m_Bodies.back();
}

void C_PhysicsManager::M_Simulate()
{
	double dt=C_Singleton::M_Timer()->M_Get()/0.016;
	m_World.Step(m_TimeStep*dt, m_VelocityIterations, m_PositionIterations);
}
