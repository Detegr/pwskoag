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
}
