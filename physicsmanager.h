#pragma once
#include <Box2D/Box2D.h>
#include <Box2D/Common/b2Math.h>
#include <vector>
#include <iostream>
#include "physicalentity.h"

class C_PhysicsManager
{
	friend class C_PhysicalEntity;
	friend class C_StaticEntity;
	friend class C_DynamicEntity;
	friend class C_Singleton;

	private:
		C_PhysicsManager();
		~C_PhysicsManager();
		static const float32 m_ScaleFactor = 10.0f;
		static const float32 m_TimeStep = 1.0f/60.0f;
		static const int32 m_VelocityIterations = 6; // 8
		static const int32 m_PositionIterations = 3; // 3
		b2World m_World;

		std::vector<C_Entity*> m_Bodies;
	public:
		void M_Simulate();
		C_Entity* M_CreateDynamicEntity(const C_Model& m, float s=1.0f);
		C_Entity* M_CreateStaticEntity(const C_Model& m, float s=1.0f);
};