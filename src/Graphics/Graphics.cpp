#include "Graphics.h"

namespace pwskoag
{
	C_RendererImpl* 	C_Renderer::impl;
	uint				C_Renderer::references;

	C_Renderer::C_Renderer(uint w, uint h, t_Entities& objects)
	{
		if(!references){impl=new C_RendererImpl(w,h,objects); ++references;}
		else {++references;}
	}
	C_Renderer::~C_Renderer()
	{
		--references;
		if(!references) delete impl;
	}
}
