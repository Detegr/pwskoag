#pragma once

#include "singleton.h"
#include "model.h"
#include "networkenum.h"
#include <assert.h>
#include <vector>
#include "dtglib/Packet.h"

using namespace dtglib;

class C_DummyParse {};

struct C_PacketParser
{
		static void M_GfxEntity(C_Packet& p, bool full);
		static void M_Parse(C_Packet& p);
		static void M_EntityDeleted(C_Packet& p);
		static void M_Model(C_Packet& p);
};

void C_PacketParser::M_Parse(C_Packet& p)
{
	unsigned char header;
	p >> header;
	switch(header)
	{
		case NET::ModelBegin:
		{
			std::cout << "Model" << std::endl;
			M_Model(p);
			return;
		}
		case NET::EntityBegin:
		{
			std::cout << "Entity" << std::endl;
			M_GfxEntity(p, false);
			return;
		}
		case NET::FullEntityBegin:
		{
			std::cout << "Entityfull" << std::endl;
			M_GfxEntity(p, true);
			return;
		}
		case NET::EntityDeleted:
		{
			std::cout << "Entitydel" << std::endl;
			M_EntityDeleted(p);
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
	if(full) p >> id >> name >> scale >> x >> y >> angle;
	else p >> id >> x >> y >> angle;

	C_Renderer* r=C_Singleton::M_Renderer();
	C_GfxEntity* e=r->M_GetEntity(id);
	if(e)
	{
		e->M_SetPosition(x/10.0f, y/10.0f);
		e->M_SetRotation(angle);
	}
	else if(full)
	{
		C_GfxEntity* e=C_GfxEntity::M_Create(id, C_Singleton::M_ModelManager()->M_Get(name), scale);
		e->M_SetPosition(x/10.0f, y/10.0f);
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
