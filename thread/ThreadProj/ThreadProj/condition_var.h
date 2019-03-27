#pragma once
#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <list>

using namespace std;
/*
 * ����������
 *		�߳�A�ȴ�һ����������
 *		�߳�B����Ϣ������д���ݣ������߳�A
 *	
 *		��Ա������wait(),notify(),notifyall()
 *
 * һ����ٻ���
 *		1����������Ϣ����Ϊ�գ�����һ�����ݣ������ö��notify_one(); һ��write�̣߳����ж���߳�ͬʱread ��
 *
 * ������ֹ��ٻ���
 *		wait�����ĵڶ���������lambda���ʽ����ȷ�ж���Ϣ�����Ƿ�Ϊ��
 */

class User
{
public:
	// ����Ϣ���߳���ں���������Ϣ���뵽��Ϣ������
	void write_msg()
	{
		for (int i = 0; i < 100000; ++i)
		{
			std::cout << "Write msg: " << i << std::endl;

			std::unique_lock<std::mutex> ul(m_mutex1);
			m_msg_list.push_back(i);
			// ����������״̬��wait����
			m_cond_val.notify_one();
			// �������е�wait�������߳�waitʱ��ֻ����һ��������  
			//m_cond_val.notify_all();
			// ��������
		}

		std::cout << "write_msg end\n";

	}

	// ����Ϣ���߳���ں���������Ϣ����Ϣ������ȡ��
	void read_msg()
	{
		while (true)
		{
			std::unique_lock<std::mutex> ul(m_mutex1);

			/* 1, wait�ȴ�������ڶ�������lambda���ʽ�ķ���ֵ��false����ôwait��������������������ڱ���
			 *	  ��������notify_one��Ա��������Ϊֹ
			 * 2, wait�����û�еڶ�����������ô�͸��ڶ�����������falseЧ��һ��
			 */
			m_cond_val.wait(ul, [this] {
				if (!m_msg_list.empty())
					return true;
				return false;
			});

			int val = m_msg_list.front();
			m_msg_list.pop_front();

			std::cout << "read_msg : " << val << std::endl;
			ul.unlock(); // unique_lock������ǰunlock

			// �������һ�����壬���wait��notify_one����Ĵ������׶Σ������ղ���notify_one
			// ��������ʱ�������е�msg��������һ����ȡ��������������
			// ��������
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