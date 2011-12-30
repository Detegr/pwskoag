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

	class C_Mutex
	{
		friend class C_CondVar;
		private:
			#ifdef _WIN32
				HANDLE m_Mutex;
			#else
				pthread_mutex_t m_Mutex;
				pthread_mutexattr_t m_Attr;
			#endif
		public:
			C_Mutex();
			~C_Mutex() {pthread_mutex_destroy(&m_Mutex);}
			void M_Lock() {pthread_mutex_lock(&m_Mutex);}
			void M_Unlock() {pthread_mutex_unlock(&m_Mutex);}
	};

	class C_Lock
	{
		private:
			C_Mutex* m_Mutex;
		public:
			C_Lock(C_Mutex& m) : m_Mutex(&m) {m_Mutex.M_Lock();}
			~C_Lock() {m_Mutex->M_Unlock();}
	};

	class C_CondVar
	{
		private:
			C_Mutex m_Mutex;
			pthread_cond_t m_Cond;
		public:
			C_CondVar() {pthread_cond_init(&m_Cond,NULL);}
			~C_CondVar() {pthread_cond_destroy(&m_Cond);}
			void M_Wait() {mutex.Lock(); pthread_cond_wait(&m_Cond, &m_Mutex.m_Mutex); m_Mutex.M_Unlock();}
			void M_SignalOne() {pthread_cond_signal(&m_Cond);}
			void M_Signal() {pthread_cond_broadcast(&m_Cond);}
	};
}