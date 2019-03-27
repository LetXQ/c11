#pragma once
#include <thread>
#include <cstdlib>

/*
 * std::call_once �ĺ����ĵڶ���������һ��������func()
 * �߱�������������Ч���ϣ��Ȼ��������ĵ���Դ��
 * ��Ҫ��һ�����std::once_flag���ṹ�����ʹ��
 * call_once�ɹ��󣬱�Ǿͻᱻ����Ϊ�ѵ���״̬����ô��Ӧ�ĺ����Ͳ����ٲ������
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

		// �����߳�ͬʱִ�е�����ʱ������һ���߳�Ҫ������һ���߳�ִ����ϣ��Ƽ�˫���жϣ�call_onceЧ�ʿ��ܵ�һЩ
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

