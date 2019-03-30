#pragma once

#include "ngx_global.h"

class NConfig
{
private:
	NConfig();
private:
	static NConfig *m_p_ins;
public:
	~NConfig();

public:
	static NConfig* Instance()
	{
		if (!m_p_ins)
		{
			// 尽量在主线程调用一次，这样就不需要锁
			// mutex lock
			if (!m_p_ins)
			{
				m_p_ins = new NConfig();
				static GarCollect gc;
			}
		}
		return m_p_ins;
	}

	class GarCollect
	{
	public:
		~GarCollect()
		{
			if (NConfig::m_p_ins)
			{
				delete NConfig::m_p_ins;
				NConfig::m_p_ins = nullptr;
			}
		}
	};

public:
	bool Load(const char *file_name);
	const char *GetString(const char *item_name);
	int GetIntDefault(const char *item_name, const int def);

public:
	NConfItemVec m_conf_item_vec;
};