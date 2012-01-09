#pragma once
#include <Util/Base.h>
#include <Util/Timer.h>
#include <Concurrency/Concurrency.h>

namespace pwskoag
{
	class C_Player;
	class C_ServerPlayer;
	class C_ClientPlayer;
	struct C_ThreadData
	{
		C_Mutex*	lock;
		Socket*		socket;
		C_Timer*	timer;
		std::vector<C_Player *>* m_Players;
		C_Mutex*	m_PlayerLock;
		C_CondVar*	m_PlayersModified;
		bool*		stopNow;
		void*		m_Void1;
		void*		m_Void2;
		C_ThreadData(C_Mutex* l, Socket* sock, C_Timer* t, std::vector<C_Player *>* p, C_Mutex* pl, C_CondVar* plm, bool* stop, void* void1=NULL, void* void2=NULL) :
			lock(l), socket(sock), timer(t), m_Players(p), m_PlayerLock(pl), m_PlayersModified(plm), stopNow(stop), m_Void1(void1), m_Void2(void2) {}
	};

	struct LocalThreadData
	{
		Socket*				socket;
		C_Timer*			timer;
		C_Mutex				lock;
		LocalThreadData(Socket* s) : socket(s), timer(new C_Timer) {}
	};
}
