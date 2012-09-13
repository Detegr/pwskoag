#include "contactlistener.h"
#include "physicalentity.h"

void C_ContactListener::BeginContact(b2Contact *c)
{
	int* ud1=(int*)c->GetFixtureA()->GetBody()->GetUserData();
	int* ud2=(int*)c->GetFixtureB()->GetBody()->GetUserData();
	// Count the hits
	if(ud1[0] == C_Entity::Type::Bullet && ud2[0] == C_Entity::Type::Bullet)
	{
		ud1[1]=255;
		ud2[1]=255;
		return;
	}
	if(ud1[0] == C_Entity::Type::Bullet) ud1[1]++;
	if(ud2[0] == C_Entity::Type::Bullet) ud2[1]++;
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
