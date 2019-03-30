#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ngx_conf.h"
#include "common_def.h"
#include "ngx_func.h"

// ��ȫ�ֱ������ж���
char **g_os_argv = nullptr; //ԭʼ�����в�������,��main�лᱻ��ֵ
char *gp_envmem = nullptr; // �±���Ļ��������ڴ�
int g_environlen = 0; //����������ռ�ڴ��С

int main(int argc, char* argv[])
{
	printf("--------- nginx starting !!!---------");
	g_os_argv = (char **)argv;//�������ָ��  

	ngx_init_setproctitle();

	NConfig* p_conf = NConfig::Instance();
	if (!p_conf || !p_conf->Load("nginx.conf"))
	{
		printf("Load Conf failed!!!\n");
		exit(1);
	}

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