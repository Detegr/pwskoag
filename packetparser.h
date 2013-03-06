#pragma once

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
