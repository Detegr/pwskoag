#include "Concurrency.h"
#include <stdexcept>
namespace Concurrency
{
	void* Thread::threadInit(void* args)
	{
		Thread::Data* d=(Thread::Data*)args;
		d->func(d->arg);
	}
	Thread::Thread(threadFunc f, void* args) : thread(NULL), data(Data(f, args))
	{
		int ret=pthread_create(&thread, NULL, threadInit, &data);
		if(ret)
		{
			throw std::runtime_error("Error creating a thread.");
		}
	}
}
