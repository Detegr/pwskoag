#include "Concurrency.h"
#include <stdexcept>
#ifdef _WIN32
	#include <process.h>
#endif

namespace pwskoag
{
	#ifdef _WIN32
		unsigned __stdcall C_Thread::M_ThreadInit(void* args)
		{
			C_Thread::C_Data* d=(C_Thread::C_Data*)args;
			d->m_Func(d->m_Arg);
			return 0;
		}
		C_Thread::C_Thread(t_ThreadFunc f, void* args) : m_Thread(0), m_Data(C_Data(f, args))
		{
			m_Thread=(HANDLE)_beginthreadex(NULL,0,M_ThreadInit,&m_Data,0,NULL);
			if(!m_Thread)
			{
				throw std::runtime_error("Error creating a thread.");
			}
		}
	#else
		void* C_Thread::M_ThreadInit(void* args)
		{
			C_Thread::C_Data* d=(C_Thread::C_Data*)args;
			d->m_Func(d->m_Arg);
		}
		C_Thread::C_Thread(t_ThreadFunc f, void* args) : m_Thread(0), m_Data(C_Data(f, args))
		{
			int ret=pthread_create(&m_Thread, NULL, M_ThreadInit, &m_Data);
			if(ret)
			{
				throw std::runtime_error("Error creating a thread.");
			}
		}
	#endif

	void C_Thread::M_Join()
	{
		#ifdef _WIN32
			WaitForSingleObject(m_Thread, INFINITE);
		#else
			pthread_join(m_Thread, NULL);
		#endif
	}
	C_Thread::~C_Thread()
	{
		#ifdef _WIN32
			if(m_Thread)
			{
				M_Join();
				CloseHandle(m_Thread);
				m_Thread=NULL;
			}
		#else
			if(m_Thread)
			{
				M_Join();
				pthread_detach(m_Thread);
				m_Thread=0;
			}
		#endif
	}
#ifndef _WIN32
	Mutex::Mutex()
	{
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}
#endif
}
