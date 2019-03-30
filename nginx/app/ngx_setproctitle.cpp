#include <unistd.h> // env
#include <string.h>

#include "ngx_global.h"

/*
 *	���ÿ�ִ�г���ı���
 *	ǰ�᣺���е������в���֮�󶼲����õ������Ա����⸲��
 *	ע�⣺���ⳤ�Ȳ���̫��������ԭʼ�����ԭ���������Ĵ�С
 *  ʵ�֣�
 *		./nginx -s -v
 *		argv��ָ����ڴ��֮���������Ļ��������Ĳ�����Ϣ(����ͨ��ȫ�ֵ�environ[char **]����)
 *		environ��argv�ǽ����ģ����Կ������·���һ���ڴ����ڱ��滷������������
 */

void ngx_init_setproctitle()
{
	// ͳ�ƻ���������ռ���ڴ�
	for (int i = 0; environ[i]; ++i)
	{
		// strlen������'\0'������
		g_environlen += strlen(environ[i]) + 1;
	}

	gp_envmem = new char[g_environlen];
	memset(gp_envmem, 0, g_environlen);
	printf("ngx_init_setproctitle::gp_envmem[%p]\n", gp_envmem);

	// ��ԭ�ڴ渴�Ƶ��µĵط�
	char *ptmp = gp_envmem;
	for (int i = 0; environ[i]; ++i)
	{
		size_t size = strlen(environ[i]) + 1;
		strcpy(ptmp, environ[i]);
		environ[i] = ptmp;
		ptmp += size;
	}
}

// ���ÿ�ִ�г���ı���
void ngx_setproctitle(const char *title)
{
	size_t title_len = strlen(title);
	size_t e_environlen = 0;

	for (int i = 0; g_os_argv[i]; ++i)
	{
		e_environlen += strlen(g_os_argv[i]) + 1;
	}

	size_t esy = e_environlen + g_environlen;
	// �������
	if (esy <= title_len)
		return;

	// ��ֹargv������
	g_os_argv[1] = nullptr;

	char *ptmp = g_os_argv[0];
	strcpy(ptmp, title);

	// ��������
	ptmp += title_len;
	// ������֮���û���õ����ڴ�����
	size_t cha = esy - title_len;
	memset(ptmp, 0, cha);
}