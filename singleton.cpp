#include "singleton.h"

C_Renderer* C_Singleton::m_Renderer;
C_InputHandler* C_Singleton::m_InputHandler;
C_ShaderManager* C_Singleton::m_ShaderManager;
C_ModelManager* C_Singleton::m_ModelManager;
C_GfxTimer* C_Singleton::m_GfxTimer;

C_Renderer* C_Singleton::M_Renderer()
{
	if(!m_Renderer) m_Renderer = new C_Renderer(PWSKOAG_WINDOW_WIDTH, PWSKOAG_WINDOW_HEIGHT);
	return m_Renderer;
}
C_Renderer* C_Singleton::M_Renderer(unsigned int w, unsigned int h)
{
	if(!m_Renderer) m_Renderer = new C_Renderer(w,h);
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
C_GfxTimer* C_Singleton::M_GfxTimer()
{
	if(!m_GfxTimer) m_GfxTimer = new C_GfxTimer;
	return m_GfxTimer;
}

void C_Singleton::M_DestroySingletons()
{
	if(m_Renderer) delete m_Renderer;
	if(m_InputHandler) delete m_InputHandler;
	if(m_ShaderManager) delete m_ShaderManager;
	if(m_ModelManager) delete m_ModelManager;
	if(m_GfxTimer) delete m_GfxTimer;
}

