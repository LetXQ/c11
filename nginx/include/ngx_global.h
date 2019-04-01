#pragma once
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>

// �������ó������
//�ⲿȫ����������ע�ⲻ���������ʼ��
extern char **g_os_argv;
extern char *gp_envmem;
extern int g_environlen;

// nginx������
struct NConfItem
{
	char m_item_name[50];
	char m_item_content[500];
};
using NConfItemVec = std::vector<NConfItem*>;

//��������־��� 
struct ngx_log_t
{
	int m_log_level = 0;   //��־���� ������־���ͣ�ngx_macro.h���0-8��9������
	int m_fd = 0;          //��־�ļ�������
};
extern ngx_log_t g_ngx_log;

extern pid_t ngx_pid;