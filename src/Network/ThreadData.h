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
		bool*		stopNow;
		C_ThreadData(C_Mutex* l, Socket* sock, C_Timer* t, std::vector<C_Player *>* p, C_Mutex* pl, bool* stop) :
			lock(l), socket(sock), timer(t), m_Players(p), m_PlayerLock(pl), stopNow(stop) {}
	};

	struct LocalThreadData
	{
		Socket*				socket;
		C_Timer				timer;
		C_Mutex				lock;
		LocalThreadData(Socket* s) : socket(s), timer(C_Timer()) {}
	};
}