#include <iostream>
#include "../Concurrency/Concurrency.h"
#include "../Util/Base.h"

void thread(void* args)
{
	std::cout << "Hi, this is a thread test, with argument :" << (*reinterpret_cast<std::string*>(args)).c_str() << std::endl;
}

int main()
{
	std::string str("String :)");
	pwskoag::C_Thread t(thread, &str);
	pwskoag::msSleep(100);
	std::cout << "This is the main thread." << std::endl;
	t.M_Join();
	return 0;
}
