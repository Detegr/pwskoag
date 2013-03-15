#include "packetparser.h"

#include "singleton.h"
#include "model.h"
#include "networkenum.h"
#include <assert.h>
#include <vector>

void C_PacketParser::M_Parse(C_Packet& p)
{
	unsigned char header;
	p >> header;
	switch(header)
	{
		case NET::ModelBegin:
		{
			M_Model(p);
			return;
		}
		case NET::EntityBegin:
		{
			M_GfxEntity(p, false);
			return;
		}
		case NET::FullEntityBegin:
		{
			M_GfxEntity(p, true);
			return;
		}
		case NET::EntityDeleted:
		{
			M_EntityDeleted(p);
			return;
		}
		case NET::PlayerId:
		{
			unsigned short id;
			p >> id;
			C_GfxEntity* e=C_Singleton::M_Renderer()->M_GetEntity(id);
			e->SetPlayer(true);
			/*C_Entity* pe=*/C_Singleton::M_PhysicsManager()->M_CreateDynamicEntity(e->M_Id(), C_Singleton::M_ModelManager()->M_Get(e->M_ModelName()), e->M_Scale());
			//pe->SetPosition(e->GetPosition());
			//pe->SetRotation(e->GetRotation());
			return;
		}
	}
	std::cout << "UNKNOWN HEADER: " << (int)header << std::endl;
	assert(false);
	return;
}

void C_PacketParser::M_EntityDeleted(C_Packet& p)
{
	unsigned short id;
	p >> id;
	C_Renderer* r=C_Singleton::M_Renderer();
	r->M_DeleteEntity(r->M_GetEntity(id));
}

void C_PacketParser::M_GfxEntity(C_Packet& p, bool full)
{
	std::string name;
	float scale,x,y,angle;
	scale=x=y=angle=0.0f;
	unsigned short id=65535;
	unsigned char type;

	if(full) p >> id >> type >> name >> scale >> x >> y >> angle;
	else p >> id >> x >> y >> angle;

	C_PhysicsManager* pm=C_Singleton::M_PhysicsManager();
	C_Renderer* r=C_Singleton::M_Renderer();
	C_GfxEntity* e=r->M_GetEntity(id);
	C_Entity* pe=pm->GetEntity(id);
	if(e)
	{
		e->SetPosition(x,y);
		e->M_SetRotation(angle);
		if(pe)
		{
			pe->SetPosition(x,y);
			pe->SetRotation(angle);
		}
	}
	else if(full)
	{
		std::cout << "Creating entity with id: " << id << std::endl;
		//C_PhysicsManager* p=C_Singleton::M_PhysicsManager();
		C_ModelManager* m=C_Singleton::M_ModelManager();
		const C_Model& model=m->M_Get(name);
		C_GfxEntity* e=C_GfxEntity::M_Create(id, model, scale);
		//C_Entity* pe;
		//if(e->IsPlayer())
		//{
			/*
		if(type == C_Entity::Fixed)
		{
			pe=p->M_CreateStaticEntity(id, model, scale);
		}
		else
		{
		*/
		//}
		e->SetPosition(x,y);
		e->M_SetRotation(angle);
	}
}

void C_PacketParser::M_Model(C_Packet& p)
{
	unsigned char header;

	std::string name;
	p >> name;

	std::vector<float> verts;
	p >> header;
	while(header == NET::ModelIndex)
	{
		float f;
		p >> f;
		verts.push_back(f);
		p >> header;
	}
	assert(header == NET::ModelDimensions);
	float w,h;
	p >> w;
	p >> h;

	C_Singleton::M_ModelManager()->M_Create(name, verts, w, h);
}
