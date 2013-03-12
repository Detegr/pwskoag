#include "singleton.h"

C_Renderer* C_Singleton::m_Renderer;
C_InputHandler* C_Singleton::m_InputHandler;
C_ShaderManager* C_Singleton::m_ShaderManager;
C_ModelManager* C_Singleton::m_ModelManager;
C_Timer* C_Singleton::m_Timer;
C_PhysicsManager* C_Singleton::m_PhysicsManager;

C_Renderer* C_Singleton::M_Renderer()
{
	if(!m_Renderer) m_Renderer = new C_Renderer(DTGLIB_WINDOW_WIDTH, DTGLIB_WINDOW_HEIGHT);
	return m_Renderer;
}
C_InputHandler* C_Singleton::M_InputHandler()
{
	if(!m_InputHandler) m_InputHandler = new C_InputHandler;
	return m_InputHandler;
}
C_ShaderManager* C_Singleton::M_ShaderManager()
{
	if(!m_ShaderManager) m_ShaderManager = new C_ShaderManager;
	return m_ShaderManager;
}
C_ModelManager* C_Singleton::M_ModelManager()
{
	if(!m_ModelManager) m_ModelManager = new C_ModelManager;
	return m_ModelManager;
}
C_Timer* C_Singleton::M_Timer()
{
	if(!m_Timer) m_Timer = new C_Timer;
	return m_Timer;
}
C_PhysicsManager* C_Singleton::M_PhysicsManager()
{
	if(!m_PhysicsManager) m_PhysicsManager = new C_PhysicsManager;
	return m_PhysicsManager;
}

void C_Singleton::M_DestroySingletons()
{
	if(m_Renderer) delete m_Renderer;
	if(m_InputHandler) delete m_InputHandler;
	if(m_ShaderManager) delete m_ShaderManager;
	if(m_ModelManager) delete m_ModelManager;
	if(m_Timer) delete m_Timer;
	if(m_PhysicsManager) delete m_PhysicsManager;
}

