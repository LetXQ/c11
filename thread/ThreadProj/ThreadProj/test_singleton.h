#pragma once
#include <iostream>

#include "singleton.h"

//#ifdef  __linux__

class Configure : public Singleton<Configure>
{
public:
	void test()
	{
		std::cout << "Print Configure\n";
	}
};

void test_singleton()
{
	Configure::instance()->test();
	std::cout << "This is Linux os\n";
}
//#elif _WIN32
//void test_singleton()
//{
//	std::cout << "This is windows os\n";
//}
//#endif //  __linux__
