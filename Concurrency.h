#pragma once
#include <pthread.h>
#include <iostream>
namespace Concurrency
{
	class Thread
	{
		private:
			typedef void(*threadFunc)(void*);
			struct Data
			{
				Data() : func(NULL), arg(NULL) {}
				Data(threadFunc f, void* a) : func(f), arg(a) {}
				threadFunc 	func;
				void*		arg;
			};

			pthread_t thread;
			Data data;
			static void* threadInit(void* args);
		public:
			Thread(threadFunc f, void* args=NULL);
			~Thread() {if(thread){pthread_detach(thread); thread=NULL;}}
			void Join() {pthread_join(thread, NULL);}
	};

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

	class CondVar
	{
		private:
			Mutex mutex;
			pthread_cond_t cond;
		public:
			CondVar() : cond(PTHREAD_COND_INITIALIZER) {}
			~CondVar() {pthread_cond_destroy(&cond);}
			void Wait() {mutex.Lock(); pthread_cond_wait(&cond, &mutex.mutex); mutex.Unlock();}
			void SignalOne() {pthread_cond_signal(&cond);}
			void Signal() {pthread_cond_broadcast(&cond);}
	};
}
