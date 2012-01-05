#pragma once
#include <Util/Base.h>
#include <SFML/Graphics.hpp>
#include <vector>

namespace pwskoag
{
	typedef std::vector<const sf::Drawable *> t_Drawable;
	class C_RendererImpl
	{
		friend class C_Renderer;
		private:
			sf::RenderWindow				window;
			std::vector<const sf::Drawable *> 	objectsToDraw;
			void M_Draw()
			{
				window.Clear();
				for(t_Drawable::const_iterator it=objectsToDraw.begin(); it!=objectsToDraw.end(); ++it) window.Draw(**it);
				window.Display();
			}
			C_RendererImpl(uint w, uint h) : window(sf::VideoMode(w,h,32), "") {}
	};

	class C_Pollable
	{
		virtual sf::Event& M_GetEvent()=0;
	};

	class C_Renderer : public C_Pollable
	{
		private:
			sf::Event m_Event;
			static C_RendererImpl* 	impl;
			static uint				references;
		public:
			C_Renderer(uint w, uint h);
			~C_Renderer();
			void M_AddObject(const sf::Drawable& obj) {impl->objectsToDraw.push_back(&obj);}
			void M_Draw() {impl->M_Draw();}
			bool M_Running() const {return impl->window.IsOpened();}
			void M_Stop() {impl->window.Close();}
			virtual sf::Event& M_GetEvent() {impl->window.GetEvent(m_Event); return m_Event;}
	};
}
