#pragma once
#ifdef _WIN32
	#include <Windows.h>
#else
	#include <pthread.h>
#endif
#include <iostream>

namespace pwskoag
{
	class C_Thread
	{
		private:
			typedef void(*t_ThreadFunc)(void*);

			#ifdef _WIN32
				HANDLE m_Thread;
				static unsigned __stdcall M_ThreadInit(void* args);
			#else
				pthread_t m_Thread;
				static void* M_ThreadInit(void* args);
			#endif
			
			struct C_Data
			{
				C_Data() : m_Func(NULL), m_Arg(NULL) {}
				C_Data(t_ThreadFunc f, void* a) : m_Func(f), m_Arg(a) {}
				t_ThreadFunc 	m_Func;
				void*			m_Arg;
			};

			C_Data m_Data;
		public:
			C_Thread(t_ThreadFunc f, void* args=NULL);
			~C_Thread();
			void M_Join();
	};

#ifndef _WIN32
	class Mutex
	{
		friend class CondVar;
		private:
			pthread_mutex_t mutex;
			pthread_mutexattr_t attr;
		public:
			Mutex();
			~Mutex() {pthread_mutex_destroy(&mutex);}
			void Lock() {pthread_mutex_lock(&mutex);}
			void Unlock() {pthread_mutex_unlock(&mutex);}
	};

	class Lock
	{
		private:
			Mutex* m;
		public:
			Lock(Mutex& m) : m(&m) {m.Lock();}
			~Lock() {m->Unlock();}
	};

	class CondVar
	{
		private:
			Mutex mutex;
			pthread_cond_t cond;
		public:
			CondVar() {pthread_cond_init(&cond,NULL);}
			~CondVar() {pthread_cond_destroy(&cond);}
			void Wait() {mutex.Lock(); pthread_cond_wait(&cond, &mutex.mutex); mutex.Unlock();}
			void SignalOne() {pthread_cond_signal(&cond);}
			void Signal() {pthread_cond_broadcast(&cond);}
	};
#endif
}