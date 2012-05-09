#pragma once
#include <SFML/Graphics.hpp>
#include <Util/Base.h>
#include <Network/Client.h>
#include <Game/ClientPlayer.h>
#include <Network/ThreadData.h>
#include <vector>

namespace pwskoag
{
	typedef std::vector<std::pair<ushort, const C_Drawable *> > t_Drawable;
	class C_RendererImpl
	{
		friend class C_Renderer;
		friend class C_RendererSyncer;
		private:
			sf::RenderWindow	window;
			t_Entities&			m_Objects;
			C_Mutex				m_Lock;
			void M_Draw()
			{
				window.Clear();
				for(t_Entities::const_iterator it=m_Objects.begin(); it!=m_Objects.end(); ++it)
				{
					C_ClientPlayer* e = dynamic_cast<C_ClientPlayer*>(*it);
					e->M_GetDRAW().M_Draw(window);
				}
				window.Display();
			}
			C_RendererImpl(uint w, uint h, t_Entities& objects) : window(sf::VideoMode(w,h,32), "pwskoag-client"), m_Objects(objects) {}
	};

	class C_Pollable
	{
		virtual sf::Event& M_GetEvent()=0;
	};

	class C_Renderer : public C_Pollable
	{
		friend class C_RendererSyncer;
		private:
			static C_RendererImpl* 	impl;
			static uint				references;
			sf::Event				m_Event;
		public:
			C_Renderer(uint w, uint h, t_Entities& objects);
			~C_Renderer();
			void M_Draw() {impl->M_Draw();}
			bool M_Running() const {return impl->window.IsOpen();}
			void M_Stop() {C_Lock l(impl->m_Lock); impl->window.Close();}
			void M_UpdateEvent() {impl->window.PollEvent(m_Event);}
			virtual sf::Event& M_GetEvent() {return m_Event;}
	};
}
