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
			C_Singleton::M_Renderer()->GetEntity(id)->SetPlayer(true);
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
	r->DeleteEntity(r->GetEntity(id));
}

void C_PacketParser::M_GfxEntity(C_Packet& p, bool full)
{
	std::string name;
	float scale,x,y,angle;
	scale=x=y=angle=0.0f;
	unsigned short id=65535;
	if(full) p >> id >> name >> scale >> x >> y >> angle;
	else p >> id >> x >> y >> angle;

	C_Renderer* r=C_Singleton::M_Renderer();
	C_GfxEntity* e=r->GetEntity(id);
	if(e)
	{
		e->M_SetPosition(x,y);
		e->M_SetRotation(angle);
	}
	else if(full)
	{
		std::cout << "Creating entity with id: " << id << std::endl;
		C_GfxEntity* e;
		if(name == "triangle")
		{
			e=C_GfxEntity::M_Create(id, C_Singleton::M_ModelManager()->Get(name), "ulle.tga", scale);
		}
		else
		{
			e=C_GfxEntity::M_Create(id, C_Singleton::M_ModelManager()->Get(name), scale);
		}
		e->M_SetPosition(x,y);
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

	C_Singleton::M_ModelManager()->Create(name, verts, w, h);
}
