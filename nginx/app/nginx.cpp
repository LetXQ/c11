#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ngx_conf.h"
#include "ngx_macro.h"
#include "ngx_func.h"

// 对全局变量进行定义
char **g_os_argv = nullptr; //原始命令行参数数组,在main中会被赋值
char *gp_envmem = nullptr; // 新保存的环境变量内存
int g_environlen = 0; //环境变量所占内存大小

//和进程本身有关的全局量
pid_t ngx_pid;               //当前进程的pid

int main(int argc, char* argv[])
{
	printf("--------- nginx starting !!!---------\n");
	g_os_argv = (char **)argv;//保存参数指针  

	ngx_init_setproctitle();

	NConfig* p_conf = NConfig::Instance();
	if (!p_conf || !p_conf->Load("nginx.conf"))
	{
		ngx_log_stderr(0, "Load Conf failed!!", "nginx.conf");
		exit(1);
	}

	ngx_log_init();
	ngx_log_stderr(0, "Nginx log init ok [%s]!!!", "Leo");
	ngx_log_error_core(1, 0, "Test Log Core");
	int port = p_conf->GetIntDefault("ListenPort", 0);
	printf("Get Conf port: %d", port);
	auto test_item = p_conf->GetString("DBInfo");
	if (test_item)
	{
		printf("test_item [%s]\n", test_item);
	}
	else
	{
		printf("item not exist!\n");
	}
	/*for (;;)
	{
		sleep(1);
		printf("Sleep 1 second!");
	}*/

	printf("gp_envmem[%p]\n", gp_envmem);
	NGX_SAVE_DELETE(gp_envmem);
	printf("--------- nginx quit !!!---------\n");

	return 0;
}