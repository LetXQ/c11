#pragma once
#include <iostream>
#include <thread>
#include <cstring>
#include <memory>

using namespace std;

/*
 * 传递临时对象作为线程参数：
 * 1，若传递int等简单类型的参数，都是值传递，不要用引用
 * 2，如果传递类对象，避免隐式类型转换，一定要在创建线程时就构建对应的类的临时对象
 * 3，线程函数传递类对象，一定要用引用来接，不然会多拷贝一次
 */

void print_func()
{
	std::cout << " start thread id: " << std::this_thread::get_id() << std::endl;
	// todo
	std::cout << " finish thread id: " << std::this_thread::get_id() << std::endl;
}

//void param_func(const int& i, char* p_buff)
void param_func(const int& i, const string& p_buff)
{
	// 这里的i虽然用&，但实际是值传递
	std::cout << i << std::endl;
	// 参数char* 指针是有问题的，一旦detach后，会出现不可预料的后果
	std::cout << p_buff.c_str() << std::endl;
}

void ref_func(string& str)
{
	std::cout << str.c_str() << std::endl;
	str.assign("Nice meet to you");
}

void point_func(unique_ptr<int> p_i)
{

}

class MyThread
{
public:
	// 这里用引用不可取
	// int& m_i;
	// MyThread(int& i) : m_i(i) {}
	int m_i = 0;
	MyThread(int i) : m_i(i) {}

	// 重载()
	void operator()()
	{
		std::cout << " start thread id: " << std::this_thread::get_id() << std::endl;
		// todo
		std::cout << " finish thread id: " << std::this_thread::get_id() << std::endl;
	}
};

class MyThread1
{
public:
	void thread_func(int num)
	{
		std::cout << "num: " << num << std::endl;
	}
};

void test_creat_thread()
{
	// 以函数创建线程
	//thread t_obj(print_func);

	// 以类对象创建线程
	int i_val = 7;
	MyThread m_obj(i_val);
	//thread t_obj(m_obj);

	// 以lambda表达式
	auto lam_func = [] {
		std::cout << " start thread id: " << std::this_thread::get_id() << std::endl;
		// todo
		std::cout << " finish thread id: " << std::this_thread::get_id() << std::endl;
	};
	thread t_obj(lam_func);

	// 以类成员函数作为线程函数
	MyThread1 m_obj1;
	thread t_obj4(&MyThread1::thread_func, m_obj1, 100);
	// 这两种使用引用传递的话，一定要join
	//thread t_obj4(&MyThread1::thread_func,&m_obj1, 100);
	//thread t_obj4(&MyThread1::thread_func, std::ref(m_obj1), 100);
	t_obj4.join();

	int& r_val = i_val;
	char my_buf[] = "This is Hello World!";
	// detach的风险：这里my_buf到底什么时候转为string的，
	// 事实上，存在my_buf被回收了（主线程执行完毕），系统才用my_buf转为string，导致程序不稳定
	// thread t_obj1(param_func, i_val, my_buf); 这里直接用my_buf指针有风险
	thread t_obj1(param_func, i_val, string(my_buf));
	t_obj1.join();
	//t_obj1.detach();

	// 引用传递到子线程使用std::ref
	string t_str("Hello World");
	thread t_obj2(ref_func, std::ref(t_str));
	t_obj2.join();
	std::cout << t_str.c_str() << std::endl;

	//智能指针传递
	unique_ptr<int> t_pointer(new int(3));
	thread t_obj3(point_func, std::move(t_pointer));
	t_obj3.join(); // 这里一定要用join，不能用detach，不然该处内存被释放，子线程使用就会有问题

	// 判断该线程是否可以join或者detach
	std::cout << "can join: " << (t_obj.joinable() ? true : false) << std::endl;

	/*
	 * join函数会阻塞主线程，当子线程执行完毕后，返回主线程继续执行
	 */
	t_obj.join();

	/*
	 * detach函数，主线程和子线程分离，主线程不必等待子线程运行结束（因为有些主线程不必等待所有子线程退出才能退出）
	 * 一旦detach之后，子线程就与主线程失去联系，子线程就会驻留后台呗c++运行时库接管，由运行时库辅助清理该线程的资源
	 * 需要注意点：
	 * 1，当通过引用传递主线程的变量到子线程时，一旦主线程执行结束，该变量被回收，子线程的运行结果不可预料
	 * 2，通过类对象创建线程时，一旦调用detach时，这个m_obj对象在主线程结束后也会被回收，
		实际上thread t_obj(m_obj)这种操作， m_obj会被复制到子线程中（只要类对象中没有引用，指针，就不会产生问题）
	 */
	 //t_obj.detach();
	std::cout << " main thread id: " << std::this_thread::get_id() << std::endl;
}