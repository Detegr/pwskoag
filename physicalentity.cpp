#include "physicalentity.h"
#include "networkenum.h"

#define TOWORLD 10.0f

C_Entity::C_Entity(b2World& w, const C_Model& m, float s, bool dynamic, Type t) : 
	m_Id(0), m_Model(m), m_Scale(s), m_Dynamic(dynamic), m_Body(NULL), m_Data()
{
	static unsigned int id=0;
	id++;
	m_Id=id;
	memset(m_Data, 0, 2);
	m_Data[0]=t;
	b2BodyDef bodydef;
	if(dynamic) bodydef.type=b2_dynamicBody;
	m_Body=w.CreateBody(&bodydef);
	m_Body->SetUserData(&m_Data);

	b2PolygonShape hitshape;
	hitshape.SetAsBox((m_Model.M_Width()/2)*s*TOWORLD, (m_Model.M_Height()/2)*s*TOWORLD);

	b2FixtureDef fix;
	fix.shape=&hitshape;
	fix.density=1.0f;
	fix.friction=0.3f;
	fix.restitution=0.8f;

	dynamic ? m_Body->CreateFixture(&fix) : m_Body->CreateFixture(&hitshape, 0.0f);
}
void C_Entity::M_SetPosition(float x, float y)
{
	m_Body->SetTransform(b2Vec2(x*TOWORLD, y*TOWORLD), 0.0f);
}

void C_Entity::operator>>(dtglib::C_Packet& p)
{
	b2Vec2 pos=m_Body->GetPosition();
	float angle=m_Body->GetAngle();
	p << (unsigned char)NET::EntityBegin << m_Id << pos.x << pos.y << angle;
}

void C_Entity::M_DumpFullInstance(dtglib::C_Packet& p)
{
	b2Vec2 pos=m_Body->GetPosition();
	float angle=m_Body->GetAngle();
	p << (unsigned char)NET::FullEntityBegin << m_Id << m_Model.M_Name() << m_Scale << pos.x << pos.y << angle;
}

/*
void C_PhysicalEntity::M_Sync()
{
	b2Vec2 pos=m_Body->GetPosition();
	float32 ang=m_Body->GetAngle();
	//std::cout << m_Entity->M_ModelName() << " pos: " << pos.x << " " << pos.y << std::endl;
	//float entityscale=this->m_Entity->M_Scale();
	//m_Entity->M_SetPosition(pos.x/entityscale/10.0f, pos.y/entityscale/10.0f);
	//m_Entity->M_SetRotation((180/3.14)*ang);
}

C_StaticEntity::C_StaticEntity(b2World& w, C_Entity* e, float32 s)
{
	this->m_Entity=e;
	b2BodyDef bodydef;
	bodydef.position.Set(m_Entity->m_Posx*s, m_Entity->m_Posy*s);

	this->m_Body = w.CreateBody(&bodydef);

	b2PolygonShape hitshape;
	float entityscale=this->m_Entity->M_Scale();
	hitshape.SetAsBox((m_Entity->m_Model.M_Width()/2)*entityscale*s, (m_Entity->m_Model.M_Height()/2)*entityscale*s);
	this->m_Body->CreateFixture(&hitshape, 0.0f);
};

C_DynamicEntity::C_DynamicEntity(b2World& w, C_Entity* e, float32 s)
{
	this->m_Entity=e;
	float entityscale=this->m_Entity->M_Scale();

	b2BodyDef bodydef;
	bodydef.position.Set(m_Entity->m_Posx*s, m_Entity->m_Posy*s);
	bodydef.type=b2_dynamicBody;
	this->m_Body = w.CreateBody(&bodydef);

	b2PolygonShape hitshape;
	hitshape.SetAsBox((m_Entity->m_Model.M_Width()/2)*entityscale*s, (m_Entity->m_Model.M_Height()/2)*entityscale*s);

	b2FixtureDef fix;
	fix.shape=&hitshape;
	fix.density=1.0f;
	fix.friction=0.3f;
	fix.restitution=0.5f;

	this->m_Body->CreateFixture(&fix);
};
*/
