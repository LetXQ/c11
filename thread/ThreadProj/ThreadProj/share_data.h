#pragma once
#include <iostream>
#include <thread>
#include <cstring>
#include <memory>
#include <list>
#include <mutex>

#include <Windows.h>

/*
 * 一，多个线程的执行顺序是乱的，跟操作系统内部对线程的运行调度机制有关
 *
 * 二，线程间的共享数据：
 *		1，只读数据是安全稳定的
 *		2，有读有写，代码不做处理，肯定崩溃，最简单处理方式，读的时候不能写，写的时候不能读
 * 
 * 三, 互斥量：
 *		 是一个类对象，可以理解为一把锁，多个线程尝试用lock成员函数加锁时，只有一个线程能锁定成功（函数返回），
 *		 如果没有锁成功，流程就会卡在lock调用处，不断尝试去锁，直到成功
 *		 互斥量使用要小心，保护共享数据不多也不少，多了，影响效率，少了，达不到保护效果 
 *
 * 四，互斥量的使用：
 *		1，lock 和 unlock要成对使用,使用一个lock，就必须调用一次unlock
 *		2，c11引入std::lock_guard的类模板，直接取代 lock 和 unlock，
 *		   lock_guard的构造函数执行了mutex::lock(),西沟函数里执行了mutex::unlock()
 *
 * 五，死锁：
 *		至少有两把锁（两个互斥量），金锁（JinLock）,银锁（YinLock）
 *		两个线程A,B
 *		1, 线程A执行，先锁了JinLock，然后要去锁YinLock
 *		2，此时出现上下文切换
 *		3，线程B开始执行，先所YinLock,因为YinLock没有被锁，所以可以成功锁上，此时去锁JinLock时就产生了死锁
 *		4，线程A拿不到YinLock，线程B拿不到JinLock，流程都走不下去
 *   死锁的解决方案：
 *		1，两个互斥量上锁的顺序一致
 *
 * 六，std::lock()函数模板：
 *		同时锁住多个（至少两个）互斥量，要么都锁住，要么都没锁住
 *
 * 七，unique_lock取代lock_guard(推荐使用lock_guard):
 *		是类模板，比lock_guard灵活，效率差一点，内存占更多
 *		第二个参数std::adopt_lock标记，mutex已经被lock，不需要unique_lock的构造函数再去lock
 *		第二个参数std::try_to_lock,前提mutex不能被lock住，尝试去锁，如果没有成功，会立即返回，不会阻塞
 *		第二个参数std::defer_lock,初始化了一个没有lock的mutex，可以在合适的地方调用unique_lock的lock函数，不需要手动调用unlock
 *		unique_lock的成员函数：
 *			lock
 *			unlock
 *			try_lock，尝试加锁，返回true和false，并不阻塞
 *			release，返回它所有mutex的对象指针，并释放所有权，也就是说mutex和unique_lock不再有关系,需要自己调用mutex的unlock
 *
 * 八，锁的粒度：
 *		锁住代码的多少
 *
 * 九，unique_lock的mutex所有权转移
 *		所有权不能复制，只能转移（std::move）
 *
 * 十，recursive_mutex 递归的独占互斥量， mutex 独占互斥量（自己lock时，别人lock不了）
 *		允许同一个线程，同一个互斥量可以被多次lock
 *		当然用了recursive_mutex，需要考虑代码是否有优化空间，
 *		递归的独占互斥量效率肯定低,且递归次数有限制
 *
 * 十一，timed_mutex, recursive_timed_mutex,带超时功能 
 *		std::time_mutex:
 *			try_lock_for():参数是一段时间，等待一段时间，如果拿到锁或者超过时间没拿到锁，就继续下面流程
 *			try_lock_until()：参数是未来时间点，在未来时间点没到的时间内，如果拿到锁，或者没拿到所，但时间到了，流程都会继续
 */

/*
 * windows 临界区域
 * 二：多次进入临界区域
		在同一个线程（不同线程中会阻塞等待），windows的相同临界区域进入的进入（EnterCriticalSection）可以被多次调用
		但调用几次进入，就对应的调用几次离开（c11的mutex在同一个线程是不允许连续lock两次）
 */
//#define WINDOW_LXQ

// 类似lock_guard，构造时自动lock，析构时自动unlock
// RAII类（Resource Acquisition is Initialization）资源获取即初始化（智能指针，容器）
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
		// 用windows临界区，先初始化
		InitializeCriticalSection(&m_win_sec);
#endif // WINDOW_LXQ

	}
	// 收消息的线程入口函数，将消息加入到消息队列中
	void write_msg()
	{
		for (int i = 0; i < 100000; ++i)
		{	
#ifdef WINDOW_LXQ
			EnterCriticalSection(&m_win_sec);
			m_msg_list.push_back(i);
			LeaveCriticalSection(&m_win_sec);
#else

			// 同时锁住多个互斥量，
			//std::lock(m_mutex1, m_mutex2);
			// std::adopt_lock表示前面已经调用过mutex::lock，lock_guard的构造函数就不会再去调用mutex::lock
			//std::lock_guard<std::mutex> lg1(m_mutex1, std::adopt_lock);
			//std::lock_guard<std::mutex> lg2(m_mutex2, std::adopt_lock);

			//第二个参数std::try_to_lock，可以不会阻塞在此处，前提mutex不能自己先lock住
			//std::unique_lock<std::mutex> ul(m_mutex1, std::try_to_lock);
			
			//第二个参数std::defer_lock，前提mutex不能自己先lock住,后面用unique_lock的try_lock
			std::unique_lock<std::mutex> ul(m_mutex1, std::defer_lock);
			// 拿到锁
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
				// 没拿到锁，todo 
			}
#endif // WINDOW_LXQ
		}

		std::cout << "write_msg end\n";

	}

	// 读消息的线程入口函数，将消息从消息队列中取出
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
		// 这里调用func2时，如果都用的std::mutex会导致m_mutex被连续调用导致崩溃
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
		// 等待100ms尝试获取锁
		//if (m_mutex4.try_lock_for(timeout))
		if (m_mutex4.try_lock_until(std::chrono::steady_clock::now() + timeout))
		{
			// todo
			m_mutex4.unlock();
		}
		else
		{
			// 没拿到锁
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
		// 这里m_mutex2先于m_mutex1锁上的话就会产生死锁
		//std::lock_guard<std::mutex> lg2(m_mutex2);
		//std::lock_guard<std::mutex> lg1(m_mutex1); // 用了lock_guard不能再用lock和unlock
		
		// 使用unique_lock，第二个参数adopt_lock
		m_mutex1.lock();
		std::unique_lock<std::mutex> ul(m_mutex1, std::adopt_lock);


		// 线程休息10s
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