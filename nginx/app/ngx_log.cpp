#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#include "ngx_global.h"
#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_conf.h"

static u_char err_levels[][20] = {
	{"stderr"},    //0������̨����
	{"emerg"},     //1������
	{"alert"},     //2������
	{"crit"},      //3������
	{"error"},     //4������
	{"warn"},      //5������
	{"notice"},    //6��ע��
	{"info"},      //7����Ϣ
	{"debug"}      //8������
};

ngx_log_t g_ngx_log;

/*
 * errΪ0ʱ����logֻ������ն�
 * ���򣬽�log����ն˵�ͬʱ�������log�ļ���
 */
void ngx_log_stderr(int err, const char *fmt, ...)
{
	va_list args;

	u_char err_str[NGX_MAX_ERROR_STR + 1];
	memset(err_str, 0, sizeof(err_str));

	u_char *p, *last;
	last = err_str + NGX_MAX_ERROR_STR;
	p = NGX_CPY_MEM(err_str, "nginx: ", 7);

	va_start(args, fmt);

	p = ngx_vslprintf(p, last, fmt, args);
	va_end(args);

	// ��log������ļ���
	if (err > 0)
	{
		p = ngx_log_errno(p, last, err);
	}

	if (p >= (last - 1))
	{
		p = (last - 1) - 1;
	}
	*p++ = '\n';
	write(STDERR_FILENO, err_str, p - err_str);
}

u_char *ngx_log_errno(u_char *buf, u_char *last, int err)
{
	char *p_errinfo = strerror(err);
	size_t len = strlen(p_errinfo);

	char left_str[10] = { 0 };
	sprintf(left_str, " (%d: ", err);
	size_t left_len = strlen(left_str);

	char right_str[] = ") ";
	size_t right_len = strlen(right_str);

	size_t extra_len = left_len + right_len;
	if ((buf + len + extra_len) < last)
	{
		buf = NGX_CPY_MEM(buf, left_str, left_len);
		buf = NGX_CPY_MEM(buf, p_errinfo, len);
		buf = NGX_CPY_MEM(buf, right_str, right_len);
	}
	return buf;
}

void ngx_log_error_core(int level, int err, const char *fmt, ...)
{
	u_char  *last;
	u_char  errstr[NGX_MAX_ERROR_STR + 1];   //���+1Ҳ���ҷ�������ģ����������Բο�ngx_log_stderr()������д����

	memset(errstr, 0, sizeof(errstr));
	last = errstr + NGX_MAX_ERROR_STR;

	struct timeval   tv;
	struct tm        tm;
	time_t           sec;   //��
	u_char           *p;    //ָ��ǰҪ�������ݵ����е��ڴ�λ��
	va_list          args;

	memset(&tv, 0, sizeof(struct timeval));
	memset(&tm, 0, sizeof(struct tm));

	gettimeofday(&tv, nullptr);     //��ȡ��ǰʱ�䣬������1970-01-01 00:00:00�����ھ������������ڶ���������ʱ����һ�㲻���ġ�        

	sec = tv.tv_sec;             //��
	localtime_r(&sec, &tm);      //�Ѳ���1��time_tת��Ϊ����ʱ�䣬���浽����2��ȥ����_r�����̰߳�ȫ�İ汾������ʹ��
	tm.tm_mon++;                 //�·�Ҫ����������
	tm.tm_year += 1900;          //���Ҫ�����²�����

	u_char strcurrtime[40] = { 0 };  //����ϳ�һ����ǰʱ���ַ�������ʽ���磺2019/01/08 19:57:11
	ngx_slprintf(strcurrtime,
		(u_char *)-1,                       //����һ��u_char *��һ�� (u_char *)-1,�� �õ��Ľ���� 0xffffffff....�����ֵ�㹻��
		"%4d/%02d/%02d %02d:%02d:%02d",     //��ʽ�� ��/��/�� ʱ:��:��
		tm.tm_year, tm.tm_mon,
		tm.tm_mday, tm.tm_hour,
		tm.tm_min, tm.tm_sec);
	p = NGX_CPY_MEM(errstr, strcurrtime, strlen((const char *)strcurrtime));  //�������ӽ������õ����磺     2019/01/08 20:26:07
	p = ngx_slprintf(p, last, " [%s] ", err_levels[level]);                //��־�������ӽ������õ����磺  2019/01/08 20:26:07 [crit] 
	p = ngx_slprintf(p, last, "%P: ", ngx_pid);                             //֧��%P��ʽ������id���ӽ������õ����磺   2019/01/08 20:50:15 [crit] 2037:

	va_start(args, fmt);                     //ʹargsָ����ʼ�Ĳ���
	p = ngx_vslprintf(p, last, fmt, args);   //��fmt��args����Ū��ȥ����ϳ�������ַ���
	va_end(args);                            //�ͷ�args 

	if (err)  //���������벻��0����ʾ�д�����
	{
		//�������ʹ�����ϢҲҪ��ʾ����
		p = ngx_log_errno(p, last, err);
	}
	//��λ�ò������ǻ���ҲҪӲ���뵽ĩβ�����¸��ǵ���������
	if (p >= (last - 1))
	{
		p = (last - 1) - 1; //��β���ո�������������о�nginx������ƺ��Ͳ��� 
							 //�Ҿ��ã�last-1��������� һ������Ч���ڴ棬�����λ��Ҫ����\0����������Ϊ�ټ�1�����λ�ã����ʺϱ���\n
	}
	*p++ = '\n'; //���Ӹ����з�       

	//��ôд������ͼ���㣺��ʱ���԰�����Ū��while���ȥ����ҿ��Խ��һ������д��
	ssize_t   n;
	while (1)
	{
		if (level > g_ngx_log.m_log_level)
		{
			//Ҫ��ӡ�������־�ĵȼ�̫��󣨵ȼ�����̫�󣬱������ļ��е����ִ�)
			//������־�Ͳ���ӡ��
			break;
		}
		//�����Ƿ����˵��жϣ������˰ɣ������ɹ���Ա��֤�������ɣ� 

		//д��־�ļ�        
		n = write(g_ngx_log.m_fd, errstr, p - errstr);  //�ļ�д��ɹ��������;
		if (n == -1)
		{
			//дʧ��������
			if (errno == ENOSPC) //дʧ�ܣ���ԭ���Ǵ���û�ռ���
			{
				//����û�ռ���
				//û�ռ仹д��ë�߰�
				//��do nothing�ɣ�
			}
			else
			{
				//����������������ô�ҿ��ǰ����������ʾ����׼�����豸�ɣ�
				if (g_ngx_log.m_fd != STDERR_FILENO) //��ǰ�Ƕ�λ���ļ��ģ�����������
				{
					n = write(STDERR_FILENO, errstr, p - errstr);
				}
			}
		}
		break;
	} //end while    
}

void ngx_log_init()
{
	u_char *plogname = NULL;
	size_t nlen;

	//�������ļ��ж�ȡ����־��ص�������Ϣ
	NConfig *p_config = NConfig::Instance();
	plogname = (u_char *)p_config->GetString("Log");
	if (plogname == NULL)
	{
		//û��������Ҫ����ȱʡ��·���ļ�����
		plogname = (u_char *)NGX_ERROR_LOG_PATH; //"logs/error.log" ,logsĿ¼��Ҫ��ǰ��������
	}
	g_ngx_log.m_log_level = p_config->GetIntDefault("LogLevel", NGX_LOG_NOTICE);//ȱʡ��־�ȼ�Ϊ6��ע�⡿ �������ʧ�ܣ��͸�ȱʡ��־�ȼ�
	//nlen = strlen((const char *)plogname);

	//ֻд��|׷�ӵ�ĩβ|�ļ��������򴴽��������Ҫ����������ָ���ļ�����Ȩ�ޡ�
	//mode = 0644���ļ�����Ȩ�ޣ� 6: 110    , 4: 100��     ���û�����д�� �û������飺�������������� ��ʦ�ڵ����µ�һ�ڽ��ܹ�
	g_ngx_log.m_fd = open((const char *)plogname, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (g_ngx_log.m_fd == -1)  //����д�����ֱ�Ӷ�λ�� ��׼������ȥ 
	{
		ngx_log_stderr(errno, "[alert] could not open error log file: open() \"%s\" failed", plogname);
		g_ngx_log.m_fd = STDERR_FILENO; //ֱ�Ӷ�λ����׼����ȥ��        
	}
	return;
}