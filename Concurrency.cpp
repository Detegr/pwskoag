#include "Concurrency.h"
#include <stdexcept>

namespace pwskoag
{
	void* Thread::threadInit(void* args)
	{
		Thread::Data* d=(Thread::Data*)args;
		d->func(d->arg);
	}
	Thread::Thread(threadFunc f, void* args) : thread(0), data(Data(f, args))
	{
		int ret=pthread_create(&thread, NULL, threadInit, &data);
		if(ret)
		{
			throw std::runtime_error("Error creating a thread.");
		}
	}
	Mutex::Mutex()
	{
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}
}
