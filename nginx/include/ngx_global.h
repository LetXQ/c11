#pragma once
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>

// 用于设置程序标题
//外部全局量声明，注意不能在这里初始化
extern char **g_os_argv;
extern char *gp_envmem;
extern int g_environlen;

// nginx的配置
struct NConfItem
{
	char m_item_name[50];
	char m_item_content[500];
};
using NConfItemVec = std::vector<NConfItem*>;

//和运行日志相关 
struct ngx_log_t
{
	int m_log_level = 0;   //日志级别 或者日志类型，ngx_macro.h里分0-8共9个级别
	int m_fd = 0;          //日志文件描述符
};
extern ngx_log_t g_ngx_log;

extern pid_t ngx_pid;