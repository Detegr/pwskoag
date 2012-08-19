#pragma once

#include "singleton.h"
#include "model.h"
#include "networkenum.h"
#include <assert.h>
#include <vector>
#include "dtglib/Packet.h"

using namespace dtglib;

template <class T>
struct C_PacketParser
{
	static T M_Parse(C_Packet& p);
};

template <class C_Model>
C_Model C_PacketParser<C_Model>::M_Parse(C_Packet& p)
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

	const C_Model& m=C_Singleton::M_ModelManager()->M_Create(name, verts, w, h);
	return m;
}
