#pragma once
#include <Util/Base.h>
#include <SFML/Graphics.hpp>
#include <Game/ClientPlayer.h>
#include <vector>
#include <list>

namespace pwskoag
{
	typedef std::vector<std::pair<ushort, const sf::Drawable *> > t_Drawable;
	class C_RendererImpl
	{
		friend class C_Renderer;
		private:
			sf::RenderWindow	window;
			t_Drawable			m_Objects;
			void M_Draw()
			{
				window.Clear();
				for(t_Drawable::const_iterator it=m_Objects.begin(); it!=m_Objects.end(); ++it) window.Draw(*it->second);
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
			sf::Event				m_Event;
			static C_RendererImpl* 	impl;
			static uint				references;
		public:
			C_Renderer(uint w, uint h);
			~C_Renderer();
			void M_AddObject(const C_EntityGfx& p)
			{
				std::cout << "Adding object to renderer..." << std::endl;
				impl->m_Objects.push_back(std::make_pair(p.M_Id(), &p.M_GetDRAW().M_GetDrawableObj()));
			}
			void M_AddObjectCheckExisting(const C_EntityGfx& p)
			{
				for(t_Drawable::const_iterator it=impl->m_Objects.begin(); it!=impl->m_Objects.end(); ++it)
				{
					if(it->second==&p.M_GetDRAW().M_GetDrawableObj()) return;
				}
				std::cout << "Adding object to renderer..." << std::endl;
				impl->m_Objects.push_back(std::make_pair(p.M_Id(), &p.M_GetDRAW().M_GetDrawableObj()));
			}
			void M_Draw() {impl->M_Draw();}
			bool M_Running() const {return impl->window.IsOpened();}
			void M_Stop() {impl->window.Close();}
			virtual sf::Event& M_GetEvent() {impl->window.GetEvent(m_Event); return m_Event;}
	};
}
