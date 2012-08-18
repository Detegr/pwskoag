#include "serversingleton.h"

C_ModelManager* C_Singleton::m_ModelManager;
C_PhysicsManager* C_Singleton::m_PhysicsManager;
C_Timer* C_Singleton::m_Timer;

C_ModelManager* C_Singleton::M_ModelManager()
{
	if(!m_ModelManager) m_ModelManager = new C_ModelManager;
	return m_ModelManager;
}
C_PhysicsManager* C_Singleton::M_PhysicsManager()
{
	if(!m_PhysicsManager) m_PhysicsManager = new C_PhysicsManager;
	return m_PhysicsManager;
}
C_Timer* C_Singleton::M_Timer()
{
	if(!m_Timer) m_Timer = new C_Timer;
	return m_Timer;
}

void C_Singleton::M_DestroySingletons()
{
	if(m_ModelManager) delete m_ModelManager;
	if(m_PhysicsManager) delete m_PhysicsManager;
	if(m_Timer) delete m_Timer;
}
