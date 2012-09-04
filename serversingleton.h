#pragma once
#include "modelmanager.h"
#include "physicsmanager.h"
#include "timer.h"

#define DTGLIB_WINDOW_WIDTH 1280
#define DTGLIB_WINDOW_HEIGHT 800

class C_Singleton
{
	private:
		static C_ModelManager* m_ModelManager;
		static C_PhysicsManager* m_PhysicsManager;
		static C_Timer* m_Timer;

	public:
		static C_ModelManager* M_ModelManager();
		static C_PhysicsManager* M_PhysicsManager();
		static C_Timer* M_Timer();
		static void M_DestroySingletons();
};
