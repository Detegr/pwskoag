#include <iostream>
#include "../Concurrency/Concurrency.h"

void thread(void* args)
{
	std::cout << "Hi, this is a thread test, with argument :" << *reinterpret_cast<std::string*>(args) << std::endl;
}

int main()
{
	std::string str("String :)");
	pwskoag::Thread t(thread, &str);
	sleep(1);
	std::cout << "This is the main thread." << std::endl;
	t.Join();
	return 0;
}
