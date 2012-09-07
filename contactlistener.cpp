#include "contactlistener.h"
#include "physicalentity.h"

void C_ContactListener::BeginContact(b2Contact *c)
{
	C_Entity::Type* ud1=(C_Entity::Type*)c->GetFixtureA()->GetBody()->GetUserData();
	C_Entity::Type* ud2=(C_Entity::Type*)c->GetFixtureB()->GetBody()->GetUserData();
	if(*ud1 == C_Entity::Type::Bullet || *ud2 == C_Entity::Type::Bullet)
	{
		std::cout << "Bullet hit." << std::endl;
	}
}
void C_ContactListener::EndContact(b2Contact *)
{
}
void C_ContactListener::PreSolve(b2Contact*, const b2Manifold*)
{
}
void C_ContactListener::PostSolve(b2Contact*, const b2ContactImpulse*)
{
}
