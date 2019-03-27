#pragma once
#include <iostream>
#include <thread>
#include <cstring>
#include <memory>

using namespace std;

/*
 * ������ʱ������Ϊ�̲߳�����
 * 1��������int�ȼ����͵Ĳ���������ֵ���ݣ���Ҫ������
 * 2�������������󣬱�����ʽ����ת����һ��Ҫ�ڴ����߳�ʱ�͹�����Ӧ�������ʱ����
 * 3���̺߳������������һ��Ҫ���������ӣ���Ȼ��࿽��һ��
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
	// �����i��Ȼ��&����ʵ����ֵ����
	std::cout << i << std::endl;
	// ����char* ָ����������ģ�һ��detach�󣬻���ֲ���Ԥ�ϵĺ��
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
	// ���������ò���ȡ
	// int& m_i;
	// MyThread(int& i) : m_i(i) {}
	int m_i = 0;
	MyThread(int i) : m_i(i) {}

	// ����()
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
	// �Ժ��������߳�
	//thread t_obj(print_func);

	// ������󴴽��߳�
	int i_val = 7;
	MyThread m_obj(i_val);
	//thread t_obj(m_obj);

	// ��lambda���ʽ
	auto lam_func = [] {
		std::cout << " start thread id: " << std::this_thread::get_id() << std::endl;
		// todo
		std::cout << " finish thread id: " << std::this_thread::get_id() << std::endl;
	};
	thread t_obj(lam_func);

	// �����Ա������Ϊ�̺߳���
	MyThread1 m_obj1;
	thread t_obj4(&MyThread1::thread_func, m_obj1, 100);
	// ������ʹ�����ô��ݵĻ���һ��Ҫjoin
	//thread t_obj4(&MyThread1::thread_func,&m_obj1, 100);
	//thread t_obj4(&MyThread1::thread_func, std::ref(m_obj1), 100);
	t_obj4.join();

	int& r_val = i_val;
	char my_buf[] = "This is Hello World!";
	// detach�ķ��գ�����my_buf����ʲôʱ��תΪstring�ģ�
	// ��ʵ�ϣ�����my_buf�������ˣ����߳�ִ����ϣ���ϵͳ����my_bufתΪstring�����³����ȶ�
	// thread t_obj1(param_func, i_val, my_buf); ����ֱ����my_bufָ���з���
	thread t_obj1(param_func, i_val, string(my_buf));
	t_obj1.join();
	//t_obj1.detach();

	// ���ô��ݵ����߳�ʹ��std::ref
	string t_str("Hello World");
	thread t_obj2(ref_func, std::ref(t_str));
	t_obj2.join();
	std::cout << t_str.c_str() << std::endl;

	//����ָ�봫��
	unique_ptr<int> t_pointer(new int(3));
	thread t_obj3(point_func, std::move(t_pointer));
	t_obj3.join(); // ����һ��Ҫ��join��������detach����Ȼ�ô��ڴ汻�ͷţ����߳�ʹ�þͻ�������

	// �жϸ��߳��Ƿ����join����detach
	std::cout << "can join: " << (t_obj.joinable() ? true : false) << std::endl;

	/*
	 * join�������������̣߳������߳�ִ����Ϻ󣬷������̼߳���ִ��
	 */
	t_obj.join();

	/*
	 * detach���������̺߳����̷߳��룬���̲߳��صȴ����߳����н�������Ϊ��Щ���̲߳��صȴ��������߳��˳������˳���
	 * һ��detach֮�����߳̾������߳�ʧȥ��ϵ�����߳̾ͻ�פ����̨��c++����ʱ��ӹܣ�������ʱ�⸨��������̵߳���Դ
	 * ��Ҫע��㣺
	 * 1����ͨ�����ô������̵߳ı��������߳�ʱ��һ�����߳�ִ�н������ñ��������գ����̵߳����н������Ԥ��
	 * 2��ͨ������󴴽��߳�ʱ��һ������detachʱ�����m_obj���������߳̽�����Ҳ�ᱻ���գ�
		ʵ����thread t_obj(m_obj)���ֲ����� m_obj�ᱻ���Ƶ����߳��У�ֻҪ�������û�����ã�ָ�룬�Ͳ���������⣩
	 */
	 //t_obj.detach();
	std::cout << " main thread id: " << std::this_thread::get_id() << std::endl;
}