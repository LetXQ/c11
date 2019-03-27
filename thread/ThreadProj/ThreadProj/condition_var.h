#pragma once
#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <list>

using namespace std;
/*
 * 条件变量：
 *		线程A等待一个条件满足
 *		线程B往消息队列里写数据，触发线程A
 *	
 *		成员函数：wait(),notify(),notifyall()
 *
 * 一，虚假唤醒
 *		1，产生：消息队列为空（插入一条数据，但调用多次notify_one(); 一个write线程，但有多个线程同时read ）
 *
 * 二，防止虚假唤醒
 *		wait函数的第二个参数用lambda表达式，正确判断消息队列是否为空
 */

class User
{
public:
	// 收消息的线程入口函数，将消息加入到消息队列中
	void write_msg()
	{
		for (int i = 0; i < 100000; ++i)
		{
			std::cout << "Write msg: " << i << std::endl;

			std::unique_lock<std::mutex> ul(m_mutex1);
			m_msg_list.push_back(i);
			// 将处于阻塞状态的wait唤醒
			m_cond_val.notify_one();
			// 唤醒所有的wait，两个线程wait时，只会有一个被唤醒  
			//m_cond_val.notify_all();
			// 处理其他
		}

		std::cout << "write_msg end\n";

	}

	// 读消息的线程入口函数，将消息从消息队列中取出
	void read_msg()
	{
		while (true)
		{
			std::unique_lock<std::mutex> ul(m_mutex1);

			/* 1, wait等待，如果第二个参数lambda表达式的返回值是false，那么wait会解锁互斥量，并阻塞在本行
			 *	  阻塞到被notify_one成员函数唤醒为止
			 * 2, wait的如果没有第二个参数，那么就跟第二个参数返回false效果一样
			 */
			m_cond_val.wait(ul, [this] {
				if (!m_msg_list.empty())
					return true;
				return false;
			});

			int val = m_msg_list.front();
			m_msg_list.pop_front();

			std::cout << "read_msg : " << val << std::endl;
			ul.unlock(); // unique_lock可以提前unlock

			// 这里存在一个陷阱，如果wait在notify_one后面的处理代码阶段，可能收不到notify_one
			// 所以这里时处理所有的msg，而不是一个个取，或者其他考虑
			// 处理数据
		}
		std::cout << "read_msg end\n";
	}

private:
	bool get_msg(int& val)
	{
		if (!m_msg_list.empty())
		{
			std::unique_lock<std::mutex> ul(m_mutex1);
			if (!m_msg_list.empty())
			{
				val = m_msg_list.front();
				m_msg_list.pop_front();
				return true;
			}
		}
		return false;
	}
private:
	using msg_list_t = std::list<int>;
	msg_list_t m_msg_list;

	std::mutex m_mutex1;
	std::condition_variable m_cond_val;
};

void test_condition_val()
{
	User m_obj;
	thread t_obj1(&User::read_msg, &m_obj);
	thread t_obj2(&User::write_msg, &m_obj);

	t_obj1.join();
	t_obj2.join();
}