#pragma once
#include <Box2D/Box2D.h>
#include <Box2D/Common/b2Math.h>
#include <vector>
#include <iostream>
#include "physicalentity.h"
#include "bullet.h"

class C_PhysicsManager
{
	friend class C_PhysicalEntity;
	friend class C_StaticEntity;
	friend class C_DynamicEntity;
	friend class C_Singleton;

	private:
		C_PhysicsManager();
		~C_PhysicsManager();
		static float32 m_TimeStep;
		static const int32 m_VelocityIterations = 8; // 8
		static const int32 m_PositionIterations = 3; // 3
		b2World m_World;
		b2ContactListener* m_ContactListener;

		std::vector<C_Entity*> m_Bodies;
	public:
		void M_Simulate(double timestep);
		C_Entity* M_CreateDynamicEntity(const C_Model& m, float s=1.0f);
		C_Entity* M_CreateDynamicEntity(unsigned short id, const C_Model& m, float s=1.0f);
		C_Entity* M_CreateStaticEntity(const C_Model& m, float s=1.0f);
		C_Entity* M_CreateStaticEntity(unsigned short id, const C_Model& m, float s=1.0f);
		C_Bullet* M_CreateBullet(const C_Model& m, float s=1.0f);
		void M_DestroyEntity(C_Entity* e);
		void M_SetContactListener(b2ContactListener* cl);
		C_Entity* GetEntity(unsigned short id) const;
};
