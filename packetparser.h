#pragma once

#include "singleton.h"
#include "model.h"
#include "networkenum.h"
#include <assert.h>
#include <vector>
#include "dtglib/Packet.h"

using namespace dtglib;

class C_DummyParse {};

template <class T>
struct C_PacketParser
{
		static void* M_SpecificParse(C_Packet& p, void* data=NULL);
		static T& M_Parse(C_Packet& p);
};

template <class T>
T& C_PacketParser<T>::M_Parse(C_Packet& p)
{
	unsigned char header;
	p >> header;
	switch(header)
	{
		case NET::ModelBegin: return *(T*)C_PacketParser<C_Model>::M_SpecificParse(p); break;
		case NET::EntityBegin:
		{
			bool b=false;
			return *(T*)C_PacketParser<C_GfxEntity>::M_SpecificParse(p, &b);
			break;
		}
		case NET::FullEntityBegin:
		{
			bool b=true;
			return *(T*)C_PacketParser<C_GfxEntity>::M_SpecificParse(p, &b);
			break;
		}
	}
	assert(false);
	return *(T*)NULL;
}

template <>
void* C_PacketParser<C_GfxEntity>::M_SpecificParse(C_Packet& p, void* data)
{
	bool full=*(bool*)data;
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
	return e;
}

template <>
void* C_PacketParser<C_Model>::M_SpecificParse(C_Packet& p, void*)
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

	const C_Model* m=C_Singleton::M_ModelManager()->M_Create(name, verts, w, h);
	return (void*)m;
}
