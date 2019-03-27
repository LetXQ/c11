#pragma once
#include <iostream>
#include <cstring>
#include <thread>
#include <future>
#include <mutex>

/*
 * 一：std::async, std::future创建后台任务并返回值
 *	std::async是一个函数模板，用来启动一个异步任务，返回一个std::future对象，std::future类模板	
 *	启动异步任务：自动创建一个线程并执行对应的线程入口函数，
 *	返回的future对象含有线程返回的结果，通过get函数获取，只能get一次
 *	std::future访问异步操作结果的机制, 他的get()函数会阻塞直到拿到返回值,如果不调用get，主线程还是会等待所有的异步函数执行完毕
 
 * 二 std::async 第一个参数：
 *		1，std::launch::deferred 线程函数调用被延迟到std::future的wait函数才执行，
			如果不调用wait或get，异步函数就不会执行，实际上线程都没有创建
		2，std::launch::deferred 延迟调用，并且在主线程中执行
		3，std::launch::async 表示在调用async函数时就创建线程

	详解：
		1，std::async 一般不叫创建线程，一般叫做创建一个异步任务，因为有时候async不创建线程
		2，std::async(std::launch::deferred，thread_func)会延迟到调用get或wait是调用线程入口函数，且不创建新线程
		3，std::launch::async 参数会强制异步任务在新线程上运行
		4，std::launch::async | std::launch::deferred两个同时用，
			可能是创建新线程并立即执行，也可能deferred行为，不创建新线程，并延迟到调用get或wait时调用线程入口函数
			系统可能根据当前系统资源来自行决定
		5，如果不指定第一个参数，默认值是std::launch::async | std::launch::deferred，由系统自行决定

		6，std::async和thread区别：
			1，thread创建线程，由于系统资源限制，如果创建失败，整个程序报异常崩溃
				std::async不会报异常或崩溃，因为由于资源限制，不加额外参数的调用就不会创建新的线程，而是在调用get或wait所在的线程执行
				std::async用std::launch::async会强制创建线程，但需要承受资源限制，创建不了新的线程导致崩溃
			2，thread创建线程如果有返回值，需要等线程执行完毕，在创建线程的地方接收
				std::async创建的异步任务，可以通过future获得入口函数的返回值
			3，经验：一个程序的线程数量不宜超过100-200，操作系统有调度时间片
		7，std::async不确定性
			1，不指定第一个参数，有系统自行决定是否创建新线程
			2，std::async(thread_func)，这个异步任务到底有没有被推迟执行？
				std::future_status status = fobj.wait_for(std::chrono::seconds(0));
				用wait_for等待0s，由status值可以判断是否是延迟执行还是创建线程马上执行

 *	
 * 三 std::package_task
 *		类模板，参数是可调用对象，package_task可以将调用对象包装起来，方便将来作为线程入口函数调用

 *
 * 四 std::promise:
 *		我们能够在某个线程中给他赋值，在其他线程中将这个值取出来

 * 五 std::future:
 *		成员函数：wait_for

 * 六 std::shared_future
 *		std::future的get是数据的转移，std::shared_future的get函数时复制数据
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
	// 计算
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
		// 超时，线程没有执行完
		std::cout << "Future Status  Timeout\n";
	}
	else if (std::future_status::ready == status)
	{
		// 线程成功返回
		std::cout << "Future Status OK get: " << fobj.get() << std::endl;
	}
	else if (std::future_status::deferred == status)
	{
		// 延迟调用，如果async的第一个参数被设置为std::launch::deffered,需要手动调用get或wait
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
	// 执行完毕后res_share有值了，res_fu为空
	//std::shared_future<int> res_share(std::move(res_fu));
	//std::shared_future<int> res_share(res_fu.share());
	std::shared_future<int> res_share(pt_fu.get_future());
	std::cout << "Shared Future: " << res_share.get() << std::endl;
	thread t_fu2(future_func3, std::ref(res_share));
	t_fu2.join();
}