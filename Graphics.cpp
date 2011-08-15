#include "Graphics.h"

namespace Graphics
{
	RendererImpl* 	Renderer::impl;
	uint			Renderer::references;

	Renderer::Renderer(uint w, uint h)
	{
		if(!references){impl=new RendererImpl(w,h); ++references;}
		else {++references;}
	}
	Renderer::~Renderer()
	{
		--references;
		if(!references) delete impl;
	}
}
