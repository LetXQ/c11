#pragma once

#include <string.h>
#include <stdio.h>

// 截取字符串尾部空格
void Rtrim(char *str)
{
	if (!str)
		return;

	size_t len = strlen(str);
	while (len > 0 && str[len-1] == ' ')
		str[--len] = 0;
}

// 截取字符串首部空格
void Ltrim(char *str)
{
	if (!str || str[0] != ' ')
		return;

	size_t len = strlen(str);
	char *tmp = str;

	while((*tmp) !='\0')
	{
		if ((*tmp) == ' ')
			tmp++;
		else
			break;
	}

	if ((*tmp) == '\0')
	{
		*str = '\0';
		return;		
	}

	char *tmp2 = str;
	while((*tmp) != '\0')
	{
		*tmp2 = *tmp;
		tmp++;
		tmp2++;
	}
	*tmp2 = '\0';
}