#pragma once
#include <Util/Base.h>
#include <Util/Timer.h>
#include <Concurrency/Concurrency.h>
#include "Network_common.h"

namespace pwskoag
{
	class C_ServerPlayer;
	struct C_ThreadData
	{
		C_Mutex*	lock;
		Socket*		socket;
		C_Timer*	timer;
		std::vector<C_ServerPlayer *>* m_Players;
		bool*		stopNow;
		C_ThreadData(C_Mutex* l, Socket* sock, C_Timer* t, std::vector<C_ServerPlayer *>* p, bool* stop) :
			lock(l), socket(sock), timer(t), m_Players(p), stopNow(stop) {}
	};

	struct LocalThreadData
	{
		Socket*				socket;
		C_Timer				timer;
		C_Mutex	lock;
		LocalThreadData(Socket* s) : socket(s), timer(C_Timer()) {}
	};
}
