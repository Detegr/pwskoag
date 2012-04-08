#pragma once

#include <Base.h>
#include <Player.h>

namespace pwskoag
{
	static C_Entity* g_EntityById(std::vector<C_Entity*>& vec, ushort id)
	{
		for(std::vector<C_Entity *>::iterator it=vec.begin(); it!=vec.end(); ++it)
		{
			if((*it)->M_Id()==id) return *it;
		}
		return NULL;
	}
}