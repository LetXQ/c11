#pragma once
#include <thread>
#include <cstdlib>

/*
 * std::call_once 改函数的第二个参数是一个函数名func()
 * 具备互斥量能力，效率上，比互斥量消耗的资源少
 * 需要与一个标记std::once_flag（结构）结合使用
 * call_once成功后，标记就会被设置为已调用状态，那么对应的函数就不会再不会调用
 */
#ifdef __linux__

template <class T>
class Singleton
{
public:
	static T* instance()
	{
		pthread_once(&m_ponce, &Singleton::init());
		return m_instance;
	}

private:
	static void init()
	{
		if (!m_instance)
		{
			m_instance = new T;
			atexit(destroy);
		}
	}

	static void destroy()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}
private:
	static T* volatile m_instance;
	static pthread_once_t m_ponce;
};

template <class T>
T* volatile Singleton<T>::m_instance = nullptr;

template<class T>
pthread_once_t Singleton<T>::m_ponce = PTHREAD_ONCE_INIT;
#elif _WIN32

std::mutex g_mutex;
std::once_flag g_once_flag;

template<class T>
class Singleton
{
public:
	static T* instance()
	{
		//if (!m_p_instance)
		//{
		//	std::unique_lock<std::mutex> ul(g_mutex);
		//	if (!m_p_instance)
		//	{
		//		create_instance();
		//	}
		//}

		// 两个线程同时执行到这里时，其中一个线程要等另外一个线程执行完毕，推荐双重判断，call_once效率可能低一些
		std::call_once(g_once_flag, create_instance);
		return m_p_instance;
	}

//protected:
//	Singleton() {}
//	Singleton(const Singleton&);
//	Singleton& operator=(const Singleton&);
private:
	static void create_instance()
	{
		m_p_instance = new T();
		static Garbage tmp;
	}
private:
	class Garbage
	{
		friend class Singleton;
		~Garbage()
		{
			if (Singleton::m_p_instance)
			{
				delete Singleton::m_p_instance;
				Singleton::m_p_instance = nullptr;
			}
		}
	};
private:
	static T* volatile m_p_instance;
};
template <class T>
T* volatile Singleton<T>::m_p_instance = nullptr;

#endif // __linux__

