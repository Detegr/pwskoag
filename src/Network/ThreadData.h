#pragma once
#include <Util/Base.h>
#include <Util/Timer.h>
#include <dtglib/Concurrency.h>

namespace pwskoag
{
	typedef std::vector<C_Entity *> t_Entities;
	class C_Player;
	class C_ServerPlayer;
	class C_ClientPlayer;
	struct C_ThreadData
	{
		C_Mutex*	m_Lock;
		C_Socket*	m_Socket;
		C_Timer*	m_Timer;
		t_Entities* m_Players;
		C_Mutex*	m_PlayerLock;
		C_CondVar*	m_PlayersModified;
		bool*		m_StopNow;
		void*		m_Void1;
		void*		m_Void2;
		C_ThreadData(C_Mutex* l, C_Socket* sock, C_Timer* t, t_Entities* p, C_Mutex* pl, bool* stop, void* void1=NULL, void* void2=NULL) :
			m_Lock(l), m_Socket(sock), m_Timer(t), m_Players(p), m_PlayerLock(pl), m_StopNow(stop), m_Void1(void1), m_Void2(void2) {}
	};

	struct C_LocalThreadData
	{
		C_Socket*			m_Socket;
		C_Timer*			m_Timer;
		C_Mutex				m_Lock;
		C_LocalThreadData(C_Socket* s) : m_Socket(s), m_Timer(new C_Timer) {}
	};
}
