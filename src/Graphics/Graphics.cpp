#include "Graphics.h"

namespace pwskoag
{
	C_RendererImpl* 	C_Renderer::impl;
	uint				C_Renderer::references;

	C_Renderer::C_Renderer(uint w, uint h)
	{
		if(!references){impl=new C_RendererImpl(w,h); ++references;}
		else {++references;}
	}
	C_Renderer::~C_Renderer()
	{
		--references;
		if(!references) delete impl;
	}
	void C_Renderer::M_AddObject(const C_GfxEntity& p)
	{
		std::cout << "Adding object to renderer..." << std::endl;
		impl->m_Objects.push_back(std::make_pair(p.M_Id(), &p.M_GetDRAW()));
	}
	void C_Renderer::M_AddObjectCheckExisting(const C_GfxEntity& p, const t_Entities& plrs, C_Mutex& playerlock)
	{
		impl->m_Lock.M_Lock();
		for(t_Drawable::iterator it=impl->m_Objects.begin(); it!=impl->m_Objects.end(); ++it)
		{
			if(it->second==&p.M_GetDRAW()) {impl->m_Lock.M_Unlock(); return;}
		}
		impl->m_Lock.M_Unlock();
		std::cout << "Adding object to renderer..." << std::endl;
		impl->m_Objects.push_back(std::make_pair(p.M_Id(), &p.M_GetDRAW()));
	}
}
