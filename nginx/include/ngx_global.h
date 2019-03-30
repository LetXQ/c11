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

struct NConfItem
{
	char m_item_name[50];
	char m_item_content[500];
};
using NConfItemVec = std::vector<NConfItem*>;