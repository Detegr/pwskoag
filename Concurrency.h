#pragma once
#include <pthread.h>
#include <iostream>

namespace pwskoag
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
			~Thread() {if(thread){Join(); pthread_detach(thread); thread=0;}}
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
}
