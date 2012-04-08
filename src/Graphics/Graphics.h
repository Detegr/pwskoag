#pragma once
#include <SFML/Graphics.hpp>
#include <Util/Base.h>
#include <Network/Client.h>
#include <Game/ClientPlayer.h>
#include <Network/ThreadData.h>
#include <vector>
#include <list>

namespace pwskoag
{
	typedef std::vector<std::pair<ushort, const C_Drawable *> > t_Drawable;
	class C_RendererImpl
	{
		friend class C_Renderer;
		friend class C_RendererSyncer;
		private:
			sf::RenderWindow	window;
			t_Drawable			m_Objects;
			C_Mutex				m_Lock;
			void M_Draw()
			{
				window.Clear();
				m_Lock.M_Lock();
				for(t_Drawable::const_iterator it=m_Objects.begin(); it!=m_Objects.end(); ++it)
				{
					it->second->M_Draw(window);
				}
				m_Lock.M_Unlock();
				window.Display();
			}
			C_RendererImpl(uint w, uint h) : window(sf::VideoMode(w,h,32), "pwskoag-client") {}
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
			C_Renderer(uint w, uint h);
			~C_Renderer();
			void M_AddObject(const C_GfxEntity& p);
			void M_AddObjectCheckExisting(const C_GfxEntity& p, const t_Entities& plrs, C_Mutex& playerlock);
			void M_Draw() {impl->M_Draw();}
			bool M_Running() const {return impl->window.IsOpen();}
			void M_Stop() {C_Lock l(impl->m_Lock); impl->window.Close();}
			void M_UpdateEvent() {impl->window.PollEvent(m_Event);}
			virtual sf::Event& M_GetEvent() {return m_Event;}
	};
	
	static void M_SyncRenderer(void* args)
	{
		C_ThreadData* data=static_cast<C_ThreadData*>(args);
		bool* stopnow=data->stopNow;
		C_CondVar* wait=data->m_PlayersModified;
		C_Mutex* pl=data->m_PlayerLock;
		t_Entities* plrs=data->m_Players;
		t_Drawable* objects=reinterpret_cast<t_Drawable *>(data->m_Void1);
		C_Mutex* lock=reinterpret_cast<C_Mutex *>(data->m_Void2);
		while(!*stopnow)
		{
			wait->M_Wait();
			std::cout << "Condvar signaled" << std::endl;
			lock->M_Lock();
			for(t_Drawable::iterator dt=objects->begin(); dt!=objects->end(); ++dt)
			{
				pl->M_Lock();
				bool found=false;
				for(t_Entities::iterator it=plrs->begin(); it!=plrs->end(); ++it)
				{
					if((*it)->M_Id()==dt->first) {std::cout << "Player found" << std::endl; found=true; break;}
				}
				pl->M_Unlock();
				if(found) continue;
				else
				{
					std::cout << "Erasing player" << std::endl;
					dt=objects->erase(dt);
					break;
				}
			}
			lock->M_Unlock();
		}
	}
	
	class C_RendererSyncer
	{
		private:
			C_Renderer* m_Renderer;
			TcpClient * m_Client;
			C_CondVar * m_Wait;
			bool		m_StopNow;
			C_ThreadData m_Data;
			C_Thread	m_Thread;
		public:
			C_RendererSyncer(C_Renderer& r, TcpClient& c) :
				m_Renderer(&r),
				m_Client(&c),
				m_Wait(&m_Client->M_GetPlayersModified()),
				m_StopNow(false),
				m_Data(C_ThreadData(NULL, NULL, NULL, &m_Client->M_Players(), &m_Client->M_GetPlayerLock(), m_Wait, &m_StopNow, &m_Renderer->impl->m_Objects, &m_Renderer->impl->m_Lock)),
				m_Thread()
				{
					m_Thread.M_Start(M_SyncRenderer, &m_Data);
				}
			~C_RendererSyncer()
			{
				M_Stop();
			}
			void M_Stop()
			{
				m_Wait->M_Signal();
				m_StopNow=true;
				m_Thread.M_Join();
			}
	};
}
