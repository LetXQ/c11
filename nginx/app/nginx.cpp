#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ngx_conf.h"
#include "ngx_macro.h"
#include "ngx_func.h"

// ��ȫ�ֱ������ж���
char **g_os_argv = nullptr; //ԭʼ�����в�������,��main�лᱻ��ֵ
char *gp_envmem = nullptr; // �±���Ļ��������ڴ�
int g_environlen = 0; //����������ռ�ڴ��С

//�ͽ��̱����йص�ȫ����
pid_t ngx_pid;               //��ǰ���̵�pid

int main(int argc, char* argv[])
{
	printf("--------- nginx starting !!!---------\n");
	g_os_argv = (char **)argv;//�������ָ��  

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