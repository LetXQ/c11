#pragma once
#include <iostream>
#include <thread>
#include <cstring>
#include <memory>
#include <list>
#include <mutex>

#include <Windows.h>

/*
 * һ������̵߳�ִ��˳�����ҵģ�������ϵͳ�ڲ����̵߳����е��Ȼ����й�
 *
 * �����̼߳�Ĺ������ݣ�
 *		1��ֻ�������ǰ�ȫ�ȶ���
 *		2���ж���д�����벻�������϶���������򵥴���ʽ������ʱ����д��д��ʱ���ܶ�
 * 
 * ��, ��������
 *		 ��һ������󣬿������Ϊһ����������̳߳�����lock��Ա��������ʱ��ֻ��һ���߳��������ɹ����������أ���
 *		 ���û�����ɹ������̾ͻῨ��lock���ô������ϳ���ȥ����ֱ���ɹ�
 *		 ������ʹ��ҪС�ģ������������ݲ���Ҳ���٣����ˣ�Ӱ��Ч�ʣ����ˣ��ﲻ������Ч�� 
 *
 * �ģ���������ʹ�ã�
 *		1��lock �� unlockҪ�ɶ�ʹ��,ʹ��һ��lock���ͱ������һ��unlock
 *		2��c11����std::lock_guard����ģ�壬ֱ��ȡ�� lock �� unlock��
 *		   lock_guard�Ĺ��캯��ִ����mutex::lock(),����������ִ����mutex::unlock()
 *
 * �壬������
 *		����������������������������������JinLock��,������YinLock��
 *		�����߳�A,B
 *		1, �߳�Aִ�У�������JinLock��Ȼ��Ҫȥ��YinLock
 *		2����ʱ�����������л�
 *		3���߳�B��ʼִ�У�����YinLock,��ΪYinLockû�б��������Կ��Գɹ����ϣ���ʱȥ��JinLockʱ�Ͳ���������
 *		4���߳�A�ò���YinLock���߳�B�ò���JinLock�����̶��߲���ȥ
 *   �����Ľ��������
 *		1������������������˳��һ��
 *
 * ����std::lock()����ģ�壺
 *		ͬʱ��ס�����������������������Ҫô����ס��Ҫô��û��ס
 *
 * �ߣ�unique_lockȡ��lock_guard(�Ƽ�ʹ��lock_guard):
 *		����ģ�壬��lock_guard��Ч�ʲ�һ�㣬�ڴ�ռ����
 *		�ڶ�������std::adopt_lock��ǣ�mutex�Ѿ���lock������Ҫunique_lock�Ĺ��캯����ȥlock
 *		�ڶ�������std::try_to_lock,ǰ��mutex���ܱ�lockס������ȥ�������û�гɹ������������أ���������
 *		�ڶ�������std::defer_lock,��ʼ����һ��û��lock��mutex�������ں��ʵĵط�����unique_lock��lock����������Ҫ�ֶ�����unlock
 *		unique_lock�ĳ�Ա������
 *			lock
 *			unlock
 *			try_lock�����Լ���������true��false����������
 *			release������������mutex�Ķ���ָ�룬���ͷ�����Ȩ��Ҳ����˵mutex��unique_lock�����й�ϵ,��Ҫ�Լ�����mutex��unlock
 *
 * �ˣ��������ȣ�
 *		��ס����Ķ���
 *
 * �ţ�unique_lock��mutex����Ȩת��
 *		����Ȩ���ܸ��ƣ�ֻ��ת�ƣ�std::move��
 *
 * ʮ��recursive_mutex �ݹ�Ķ�ռ�������� mutex ��ռ���������Լ�lockʱ������lock���ˣ�
 *		����ͬһ���̣߳�ͬһ�����������Ա����lock
 *		��Ȼ����recursive_mutex����Ҫ���Ǵ����Ƿ����Ż��ռ䣬
 *		�ݹ�Ķ�ռ������Ч�ʿ϶���,�ҵݹ����������
 *
 * ʮһ��timed_mutex, recursive_timed_mutex,����ʱ���� 
 *		std::time_mutex:
 *			try_lock_for():������һ��ʱ�䣬�ȴ�һ��ʱ�䣬����õ������߳���ʱ��û�õ������ͼ�����������
 *			try_lock_until()��������δ��ʱ��㣬��δ��ʱ���û����ʱ���ڣ�����õ���������û�õ�������ʱ�䵽�ˣ����̶������
 */

/*
 * windows �ٽ�����
 * ������ν����ٽ�����
		��ͬһ���̣߳���ͬ�߳��л������ȴ�����windows����ͬ�ٽ��������Ľ��루EnterCriticalSection�����Ա���ε���
		�����ü��ν��룬�Ͷ�Ӧ�ĵ��ü����뿪��c11��mutex��ͬһ���߳��ǲ���������lock���Σ�
 */
//#define WINDOW_LXQ

// ����lock_guard������ʱ�Զ�lock������ʱ�Զ�unlock
// RAII�ࣨResource Acquisition is Initialization����Դ��ȡ����ʼ��������ָ�룬������
class CWinLock
{
public:
	CWinLock(CRITICAL_SECTION *p_win_sec)
		: m_p_win_sec(p_win_sec)
	{
		EnterCriticalSection(m_p_win_sec);
	}
	~CWinLock()
	{
		LeaveCriticalSection(m_p_win_sec);
	}
private:
	CRITICAL_SECTION *m_p_win_sec;
};

class Player
{
public:
	Player()
	{
#ifdef WINDOW_LXQ
		// ��windows�ٽ������ȳ�ʼ��
		InitializeCriticalSection(&m_win_sec);
#endif // WINDOW_LXQ

	}
	// ����Ϣ���߳���ں���������Ϣ���뵽��Ϣ������
	void write_msg()
	{
		for (int i = 0; i < 100000; ++i)
		{	
#ifdef WINDOW_LXQ
			EnterCriticalSection(&m_win_sec);
			m_msg_list.push_back(i);
			LeaveCriticalSection(&m_win_sec);
#else

			// ͬʱ��ס�����������
			//std::lock(m_mutex1, m_mutex2);
			// std::adopt_lock��ʾǰ���Ѿ����ù�mutex::lock��lock_guard�Ĺ��캯���Ͳ�����ȥ����mutex::lock
			//std::lock_guard<std::mutex> lg1(m_mutex1, std::adopt_lock);
			//std::lock_guard<std::mutex> lg2(m_mutex2, std::adopt_lock);

			//�ڶ�������std::try_to_lock�����Բ��������ڴ˴���ǰ��mutex�����Լ���lockס
			//std::unique_lock<std::mutex> ul(m_mutex1, std::try_to_lock);
			
			//�ڶ�������std::defer_lock��ǰ��mutex�����Լ���lockס,������unique_lock��try_lock
			std::unique_lock<std::mutex> ul(m_mutex1, std::defer_lock);
			// �õ���
			//if (ul.owns_lock())
			if(ul.try_lock())
			{
				std::cout << "Write msg: " << i << std::endl;
				//m_mutex.lock();
				m_msg_list.push_back(i);
				//m_mutex.unlock();
			}
			else
			{
				std::cout << "Write msg get no lock\n";
				// û�õ�����todo 
			}
#endif // WINDOW_LXQ
		}

		std::cout << "write_msg end\n";

	}

	// ����Ϣ���߳���ں���������Ϣ����Ϣ������ȡ��
	void read_msg()
	{
		for (int i = 0; i < 100000; ++i)
		{
			int val = 0;
			bool ret = get_msg(val);
			if (!ret)
			{
				std::cout << "Empty msg list\n";
			}
			else
			{
				std::cout << "Msg: " << val << std::endl;
			}
		}
		std::cout << "read_msg end\n";
	}

	void recur_func1()
	{
		std::lock_guard<std::recursive_mutex> lg(m_mutex3);
		// todo
		// �������func2ʱ��������õ�std::mutex�ᵼ��m_mutex���������õ��±���
		recur_func2();
	}

	void recur_func2()
	{
		std::lock_guard<std::recursive_mutex> lg(m_mutex3);
		// todo
	}

	void timed_func()
	{
		std::chrono::milliseconds timeout(100);
		// �ȴ�100ms���Ի�ȡ��
		//if (m_mutex4.try_lock_for(timeout))
		if (m_mutex4.try_lock_until(std::chrono::steady_clock::now() + timeout))
		{
			// todo
			m_mutex4.unlock();
		}
		else
		{
			// û�õ���
			std::chrono::milliseconds sleeptime(100);
			std::this_thread::sleep_for(sleeptime);

		}
	}

private:
	bool get_msg(int& val)
	{
#ifdef WINDOW_LXQ
		CWinLock cwl(&m_win_sec);
		//EnterCriticalSection(&m_win_sec);
		if (!m_msg_list.empty())
		{
			val = m_msg_list.front();
			m_msg_list.pop_front();
			//LeaveCriticalSection(&m_win_sec);
			return true;
		}
		//LeaveCriticalSection(&m_win_sec);
		return false;
#else
		// ����m_mutex2����m_mutex1���ϵĻ��ͻ��������
		//std::lock_guard<std::mutex> lg2(m_mutex2);
		//std::lock_guard<std::mutex> lg1(m_mutex1); // ����lock_guard��������lock��unlock
		
		// ʹ��unique_lock���ڶ�������adopt_lock
		m_mutex1.lock();
		std::unique_lock<std::mutex> ul(m_mutex1, std::adopt_lock);


		// �߳���Ϣ10s
		std::chrono::milliseconds dura(200);
		std::this_thread::sleep_for(dura);

		// m_mutex.lock();
		if (!m_msg_list.empty())
		{
			val = m_msg_list.front();
			m_msg_list.pop_front();
			// m_mutex.unlock();
			return true;
		}
		// m_mutex.unlock();
		return false;
#endif // WINDOW_LXQ
	}
private:
	using msg_list_t = std::list<int>;
	msg_list_t m_msg_list;

	std::mutex m_mutex1;
	std::mutex m_mutex2;
	std::recursive_mutex m_mutex3;

	std::timed_mutex m_mutex4;
#ifdef WINDOW_LXQ
	CRITICAL_SECTION m_win_sec;
#endif
};

void test_share_data()
{
	Player m_obj;
	thread read_obj(&Player::read_msg, &m_obj);
	thread write_obj(&Player::write_msg, &m_obj);

	read_obj.join();
	write_obj.join();
}