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
		static void* M_SpecificParse(C_Packet& p);
		static T& M_Parse(C_Packet& p);
};

template <class T>
T& C_PacketParser<T>::M_Parse(C_Packet& p)
{
	int header;
	p >> header;
	switch(header)
	{
		case NET::ModelBegin: return *(T*)C_PacketParser<C_Model>::M_SpecificParse(p); break;
		case NET::EntityBegin: return *(T*)C_PacketParser<C_GfxEntity>::M_SpecificParse(p); break;
	}
	assert(false);
	return *(T*)NULL;
}

template <>
void* C_PacketParser<C_GfxEntity>::M_SpecificParse(C_Packet& p)
{
	std::string name;
	float scale,x,y;
	unsigned int id=~0U;
	float angle;
	p >> id >> name >> scale >> x >> y >> angle;

	C_Renderer* r=C_Singleton::M_Renderer();
	C_GfxEntity* e=r->M_GetEntity(id);
	if(e)
	{
		e->M_SetPosition(x/10.0f, y/10.0f);
		e->M_SetRotation(angle);
	}
	else
	{
		C_GfxEntity* e=C_GfxEntity::M_Create(id, C_Singleton::M_ModelManager()->M_Get(name), scale);
		e->M_SetPosition(x/10.0f, y/10.0f);
		e->M_SetRotation(angle);
	}
	return e;
}

template <>
void* C_PacketParser<C_Model>::M_SpecificParse(C_Packet& p)
{
	int header;

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
