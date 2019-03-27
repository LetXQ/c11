#pragma once

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

/*
 * 原子操作：不需要互斥量加锁（无锁）的多线程并发编程方式，在多线程中不会被打断的程序执行片段
 *			不可分割的操作，要么完成，要么没完成
 *			一般用于计数或者统计（累计发送或接收多少个数据包）
 * std::atomic原子操作，是一个类模板
 *		atomic的原子操作针对++， --，+=，*=，-=，&=，|=， ^=是支持的，对其他操作可能不支持（g_atomic_count = g_atomic_count + 1,这种不支持）
 *
 *	cout << g_atomic_count << endl; // 读atomic是一个原子操作，但这一行代码不是原子操作，输出到终端的是曾经的值
 */


int g_count_01 = 0;
// 封装一个int类型的对象，可以像int类型一样操作
std::atomic<int> g_atomic_count = 0;
std::atomic<int> g_test_01(g_atomic_count.load());
std::mutex g_mutex_01;

void thread_func1()
{
	// 原子类型的赋值
	g_test_01 = 10;
	g_test_01.store(11);

	for (int i = 0; i < 10000; ++i)
	{
		//std::lock_guard<std::mutex> lg(g_mutex_01);
		//g_count_01++;
		g_atomic_count++;
	}
}

void read_atomic()
{
	while (true)
	{
		// 读g_atomic_count是一个原子操作，但这一行代码不是原子操作，输出到终端的是曾经的值
		// 但最终g_atomic_count不变时，输出显示就正确
		std::cout << g_atomic_count << std::endl;
	}
}

void test_atomic()
{
	thread t1(thread_func1);
	thread t2(thread_func1);

	t1.join();
	t2.join();

	std::cout << "Count: " << g_atomic_count << std::endl;
}
