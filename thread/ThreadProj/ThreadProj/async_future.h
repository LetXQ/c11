#pragma once
#include <iostream>
#include <cstring>
#include <thread>
#include <future>
#include <mutex>

/*
 * һ��std::async, std::future������̨���񲢷���ֵ
 *	std::async��һ������ģ�壬��������һ���첽���񣬷���һ��std::future����std::future��ģ��	
 *	�����첽�����Զ�����һ���̲߳�ִ�ж�Ӧ���߳���ں�����
 *	���ص�future�������̷߳��صĽ����ͨ��get������ȡ��ֻ��getһ��
 *	std::future�����첽��������Ļ���, ����get()����������ֱ���õ�����ֵ,���������get�����̻߳��ǻ�ȴ����е��첽����ִ�����
 
 * �� std::async ��һ��������
 *		1��std::launch::deferred �̺߳������ñ��ӳٵ�std::future��wait������ִ�У�
			���������wait��get���첽�����Ͳ���ִ�У�ʵ�����̶߳�û�д���
		2��std::launch::deferred �ӳٵ��ã����������߳���ִ��
		3��std::launch::async ��ʾ�ڵ���async����ʱ�ʹ����߳�

	��⣺
		1��std::async һ�㲻�д����̣߳�һ���������һ���첽������Ϊ��ʱ��async�������߳�
		2��std::async(std::launch::deferred��thread_func)���ӳٵ�����get��wait�ǵ����߳���ں������Ҳ��������߳�
		3��std::launch::async ������ǿ���첽���������߳�������
		4��std::launch::async | std::launch::deferred����ͬʱ�ã�
			�����Ǵ������̲߳�����ִ�У�Ҳ����deferred��Ϊ�����������̣߳����ӳٵ�����get��waitʱ�����߳���ں���
			ϵͳ���ܸ��ݵ�ǰϵͳ��Դ�����о���
		5�������ָ����һ��������Ĭ��ֵ��std::launch::async | std::launch::deferred����ϵͳ���о���

		6��std::async��thread����
			1��thread�����̣߳�����ϵͳ��Դ���ƣ��������ʧ�ܣ����������쳣����
				std::async���ᱨ�쳣���������Ϊ������Դ���ƣ����Ӷ�������ĵ��þͲ��ᴴ���µ��̣߳������ڵ���get��wait���ڵ��߳�ִ��
				std::async��std::launch::async��ǿ�ƴ����̣߳�����Ҫ������Դ���ƣ����������µ��̵߳��±���
			2��thread�����߳�����з���ֵ����Ҫ���߳�ִ����ϣ��ڴ����̵߳ĵط�����
				std::async�������첽���񣬿���ͨ��future�����ں����ķ���ֵ
			3�����飺һ��������߳��������˳���100-200������ϵͳ�е���ʱ��Ƭ
		7��std::async��ȷ����
			1����ָ����һ����������ϵͳ���о����Ƿ񴴽����߳�
			2��std::async(thread_func)������첽���񵽵���û�б��Ƴ�ִ�У�
				std::future_status status = fobj.wait_for(std::chrono::seconds(0));
				��wait_for�ȴ�0s����statusֵ�����ж��Ƿ����ӳ�ִ�л��Ǵ����߳�����ִ��

 *	
 * �� std::package_task
 *		��ģ�壬�����ǿɵ��ö���package_task���Խ����ö����װ���������㽫����Ϊ�߳���ں�������

 *
 * �� std::promise:
 *		�����ܹ���ĳ���߳��и�����ֵ���������߳��н����ֵȡ����

 * �� std::future:
 *		��Ա������wait_for

 * �� std::shared_future
 *		std::future��get�����ݵ�ת�ƣ�std::shared_future��get����ʱ��������
 */


int thread_func(int num)
{
	std::cout << "thread_func start id: " << std::this_thread::get_id() << ", " << num << std::endl;
	std::chrono::milliseconds dura(1000);
	std::this_thread::sleep_for(dura);
	std::cout << "thread_func end id: " << std::this_thread::get_id() << std::endl;

	return 5;
}

void promise_func(std::promise<int> &ret, int val)
{
	std::chrono::milliseconds dura(2000);
	std::this_thread::sleep_for(dura);
	// ����
	int result = val * 10;
	ret.set_value(result);
}

int future_func1(int param)
{
	std::cout << "future_func1 start id: " << std::this_thread::get_id() << ", " << param << std::endl;
	return 10;
}

void future_func2(std::future<int> &res)
{
	auto result = res.get();
	std::cout << "future_func2 end id: " << std::this_thread::get_id() << ", " << result << std::endl;
}

void future_func3(std::shared_future<int> &res)
{
	auto result = res.get();
	std::cout << "future_func2 end id: " << std::this_thread::get_id() << ", " << result << std::endl;
}

class AsyncObj
{
public:
	int thread_func(int param)
	{
		std::cout << "thread_func start id: " << std::this_thread::get_id() << ", " << param << std::endl;
		std::chrono::milliseconds dura(5000);
		std::this_thread::sleep_for(dura);
		std::cout << "thread_func end id: " << std::this_thread::get_id() << std::endl;
		return 89;
	}
};

void test_async()
{
	std::future<int> fobj = std::async(thread_func, 100);
	std::future_status status = fobj.wait_for(std::chrono::seconds(1));
	if (std::future_status::timeout == status)
	{
		// ��ʱ���߳�û��ִ����
		std::cout << "Future Status  Timeout\n";
	}
	else if (std::future_status::ready == status)
	{
		// �̳߳ɹ�����
		std::cout << "Future Status OK get: " << fobj.get() << std::endl;
	}
	else if (std::future_status::deferred == status)
	{
		// �ӳٵ��ã����async�ĵ�һ������������Ϊstd::launch::deffered,��Ҫ�ֶ�����get��wait
		std::cout << "Future Status  Deferred\n";
		std::cout << fobj.get() << std::endl;
	}
	
	AsyncObj obj;
	std::future<int> f_obj = std::async(&AsyncObj::thread_func, &obj, 100);
	std::cout << "test_async future val: " << f_obj.get() << std::endl;
	std::cout << "test_async id: " << std::this_thread::get_id() << std::endl;

	std::packaged_task<int(int)> pt(thread_func);
	thread t1(std::ref(pt), 100);
	t1.join();
	std::future<int> result = pt.get_future();
	std::cout << "Get future: " << result.get() << std::endl;

	std::promise<int> pro_val;
	thread t2(promise_func, std::ref(pro_val), 2);
	t2.join();
	std::future<int> ret = pro_val.get_future();
	std::cout << "Get Promise: " << ret.get() << std::endl;

	// std::shared_future
	std::packaged_task<int(int)> pt_fu(future_func1);
	thread t_fu(std::ref(pt_fu), 100);
	t_fu.join();
	
	//std::future<int> res_fu = pt_fu.get_future();
	// ִ����Ϻ�res_share��ֵ�ˣ�res_fuΪ��
	//std::shared_future<int> res_share(std::move(res_fu));
	//std::shared_future<int> res_share(res_fu.share());
	std::shared_future<int> res_share(pt_fu.get_future());
	std::cout << "Shared Future: " << res_share.get() << std::endl;
	thread t_fu2(future_func3, std::ref(res_share));
	t_fu2.join();
}