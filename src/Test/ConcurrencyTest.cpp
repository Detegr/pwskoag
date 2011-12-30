#include <iostream>
#include "../Concurrency/Concurrency.h"
#include "../Util/Base.h"

void thread(void* args)
{
	pwskoag::C_Mutex* m=reinterpret_cast<pwskoag::C_Mutex*>(args);
	std::cout << "Waiting for mutex..." << std::endl;
	m->M_Lock();
	std::cout << "Locked a mutex!" << std::endl;
	pwskoag::msSleep(1000);
	m->M_Unlock();
	std::cout << "And released it." << std::endl;
}

void condwait(void* args)
{
	pwskoag::C_CondVar* c=reinterpret_cast<pwskoag::C_CondVar*>(args);
	std::cout << "Waiting for condvar..." << std::endl;
	c->M_Wait();
	std::cout << "Hooray!" << std::endl;
}

int main()
{
	pwskoag::C_Mutex m;
	{
		pwskoag::C_Thread t(thread, &m);
		pwskoag::C_Thread t1(thread, &m);
		pwskoag::C_Thread t2(thread, &m);
		pwskoag::msSleep(100);
		std::cout << "This is the main thread." << std::endl;
	}
	{
		pwskoag::C_CondVar c;
		pwskoag::C_Thread t(condwait, &c);
		pwskoag::C_Thread t1(condwait, &c);
		pwskoag::C_Thread t2(condwait, &c);
		pwskoag::msSleep(2000);
		c.M_SignalOne();
		std::cout << "One signaled?" << std::endl;
		pwskoag::msSleep(2000);
		c.M_Signal();
		pwskoag::msSleep(1000);
	}
	return 0;
}
