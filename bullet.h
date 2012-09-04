#pragma once

#include <Box2D/Box2D.h>
#include <Box2D/Common/b2Math.h>
#include "physicalentity.h"

class C_Bullet : public C_Entity, public b2ContactListener
{
	friend class C_PhysicsManager;
	private:
		C_Bullet(b2World& w, const C_Model& m, float scale=1.0f, bool dynamic=true);
		void BeginContact(b2Contact*) {}
		void EndContact(b2Contact*) {}
		void PreSolve(b2Contact*, const b2Manifold*) {}
		void PostSolve(b2Contact* c, const b2ContactImpulse* impulse);
	public:
};
