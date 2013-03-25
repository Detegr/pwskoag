#pragma once
#include "renderer.h"
#include "inputhandler.h"
#include "shadermanager.h"
#include "modelmanager.h"
#include "gfxtimer.h"

#define PWSKOAG_WINDOW_WIDTH 1280
#define PWSKOAG_WINDOW_HEIGHT 800

class C_Singleton
{
	private:
		static C_Renderer* m_Renderer;
		static C_InputHandler* m_InputHandler;
		static C_ShaderManager* m_ShaderManager;
		static C_ModelManager* m_ModelManager;
		static C_GfxTimer* m_GfxTimer;

	public:
		static C_Renderer* M_Renderer();
		static C_Renderer* M_Renderer(unsigned int w, unsigned int h);
		static C_InputHandler* M_InputHandler();
		static C_ShaderManager* M_ShaderManager();
		static C_ModelManager* M_ModelManager();
		static C_GfxTimer* M_GfxTimer();
		static void M_DestroySingletons();
};
