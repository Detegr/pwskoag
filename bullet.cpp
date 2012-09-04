#include "bullet.h"

C_Bullet::C_Bullet(b2World& w, const C_Model& m, float scale, bool dynamic) : C_Entity(w,m,scale,dynamic)
{}

void C_Bullet::PostSolve(b2Contact*, const b2ContactImpulse*)
{
	std::cout << "Bullet contact end. ID: " << this->M_Id() << std::endl;
}
