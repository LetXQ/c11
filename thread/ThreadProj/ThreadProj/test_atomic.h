#pragma once

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

/*
 * ԭ�Ӳ���������Ҫ�������������������Ķ��̲߳�����̷�ʽ���ڶ��߳��в��ᱻ��ϵĳ���ִ��Ƭ��
 *			���ɷָ�Ĳ�����Ҫô��ɣ�Ҫôû���
 *			һ�����ڼ�������ͳ�ƣ��ۼƷ��ͻ���ն��ٸ����ݰ���
 * std::atomicԭ�Ӳ�������һ����ģ��
 *		atomic��ԭ�Ӳ������++�� --��+=��*=��-=��&=��|=�� ^=��֧�ֵģ��������������ܲ�֧�֣�g_atomic_count = g_atomic_count + 1,���ֲ�֧�֣�
 *
 *	cout << g_atomic_count << endl; // ��atomic��һ��ԭ�Ӳ���������һ�д��벻��ԭ�Ӳ�����������ն˵���������ֵ
 */


int g_count_01 = 0;
// ��װһ��int���͵Ķ��󣬿�����int����һ������
std::atomic<int> g_atomic_count = 0;
std::atomic<int> g_test_01(g_atomic_count.load());
std::mutex g_mutex_01;

void thread_func1()
{
	// ԭ�����͵ĸ�ֵ
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
		// ��g_atomic_count��һ��ԭ�Ӳ���������һ�д��벻��ԭ�Ӳ�����������ն˵���������ֵ
		// ������g_atomic_count����ʱ�������ʾ����ȷ
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
