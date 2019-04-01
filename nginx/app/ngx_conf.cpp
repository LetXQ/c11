#include <fcntl.h>

#include "ngx_conf.h"
#include "ngx_string.h"
#include "ngx_macro.h"


// 静态成员初始化
NConfig *NConfig::m_p_ins = nullptr;

NConfig::NConfig()
{

}


NConfig::~NConfig()
{
	for(auto elem : m_conf_item_vec)
	{
		NGX_SAVE_DELETE(elem);
	}
	m_conf_item_vec.clear();
}

bool NConfig::Load(const char *file_name)
{
	FILE *fp = fopen(file_name, "r");
	if (!fp)
		return false;

	char linebuf[512];
	memset(linebuf, 0, sizeof(linebuf));

	while(!feof(fp))
	{
		if (!fgets(linebuf, 512, fp))
			continue;

		if (linebuf[0] == 0)
			continue;

		if (*linebuf == ';' || *linebuf == ' ' || *linebuf == '#' || *linebuf =='\t' || *linebuf == '\n')
			continue;

		ngx_proc_string:
			// 行位有换行，回车，空格就截取掉
			if(strlen(linebuf) > 0)
			{
				if (linebuf[strlen(linebuf) - 1] == 10 // 换行
					|| linebuf[strlen(linebuf) - 1] == 13  // 回车
					|| linebuf[strlen(linebuf) - 1] == 32) // 空格
				{
					linebuf[strlen(linebuf) - 1] = 0;
					goto ngx_proc_string;
				}
			}

			if (linebuf[0] == 0)
				continue;

			if (*linebuf == '[')
				continue;

			// ListenPort = 7789 , ptmp指向'='
			char *ptmp = strchr(linebuf, '=');
			if (ptmp)
			{
				NConfItem* p_conf_item = new NConfItem();
				memset(p_conf_item, 0, sizeof(p_conf_item));

				// '='左边拷贝到itemname, '='拷贝到itemcontent
				strncpy(p_conf_item->m_item_name, linebuf, (int)(ptmp-linebuf));
				strcpy(p_conf_item->m_item_content, ptmp+1);

				Rtrim(p_conf_item->m_item_name);
				Ltrim(p_conf_item->m_item_name);
				Rtrim(p_conf_item->m_item_content);
				Ltrim(p_conf_item->m_item_content);

				m_conf_item_vec.push_back(p_conf_item);
			}
	}

	fclose(fp);
	return true;
}


const char *NConfig::GetString(const char *item_name)
{
	for (auto elem : m_conf_item_vec)
	{
		if (strcasecmp(item_name, elem->m_item_name) == 0)
			return elem->m_item_content;
	}
	return nullptr;
}

int NConfig::GetIntDefault(const char *item_name, const int def)
{
	for (auto elem : m_conf_item_vec)
	{
		if (strcasecmp(item_name, elem->m_item_name) == 0)
			return atoi(elem->m_item_content);
	}
	return def;
}