#include <unistd.h> // env
#include <string.h>

#include "ngx_global.h"

/*
 *	设置可执行程序的标题
 *	前提：所有的命令行参数之后都不会用到，可以被随意覆盖
 *	注意：标题长度不能太长，超过原始标题和原环境变量的大小
 *  实现：
 *		./nginx -s -v
 *		argv所指向的内存段之就是连续的环境变量的参数信息(可以通过全局的environ[char **]访问)
 *		environ和argv是紧连的，所以可以重新分配一段内存用于保存环境变量的内容
 */

void ngx_init_setproctitle()
{
	// 统计环境变量所占的内存
	for (int i = 0; environ[i]; ++i)
	{
		// strlen不包含'\0'结束符
		g_environlen += strlen(environ[i]) + 1;
	}

	gp_envmem = new char[g_environlen];
	memset(gp_envmem, 0, g_environlen);
	printf("ngx_init_setproctitle::gp_envmem[%p]\n", gp_envmem);

	// 将原内存复制到新的地方
	char *ptmp = gp_envmem;
	for (int i = 0; environ[i]; ++i)
	{
		size_t size = strlen(environ[i]) + 1;
		strcpy(ptmp, environ[i]);
		environ[i] = ptmp;
		ptmp += size;
	}
}

// 设置可执行程序的标题
void ngx_setproctitle(const char *title)
{
	size_t title_len = strlen(title);
	size_t e_environlen = 0;

	for (int i = 0; g_os_argv[i]; ++i)
	{
		e_environlen += strlen(g_os_argv[i]) + 1;
	}

	size_t esy = e_environlen + g_environlen;
	// 标题过长
	if (esy <= title_len)
		return;

	// 防止argv被滥用
	g_os_argv[1] = nullptr;

	char *ptmp = g_os_argv[0];
	strcpy(ptmp, title);

	// 跳过标题
	ptmp += title_len;
	// 将标题之后的没有用到的内存清零
	size_t cha = esy - title_len;
	memset(ptmp, 0, cha);
}