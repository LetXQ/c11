#pragma once

#define NGX_MAX_ERROR_STR 2048 //��ʾ�Ĵ�����Ϣ������鳤��
#define NGX_MAX_UINT32_VALUE (uint32_t)0xffffffff
#define NGX_INT64_LEN (sizeof("-9223372036854775808") - 1)

#define NGX_SAVE_DELETE(p) { if (p) { delete p; p = nullptr; }}
#define NGX_MULTI_SAVE_DELETE(p) {if (p) {delete [] p; p = nullptr;}}

//����memcpy��������memcpy���ص���ָ��Ŀ��dst��ָ�룬
//�����ngx_cpymem���ص���Ŀ�꡾�������ݺ󡿵��յ�λ�ã��������ƶ������ʱ����
#define NGX_CPY_MEM(dst, src, n) (((u_char*)memcpy(dst, src, n)) + (n))
#define NGX_MIN(val1, val2) ((val2 > val2) ? (val2) : (val1))
#define NGX_MAX(val1, val2) ((val2 > val2) ? (val1) : (val2))


/*----------------Log Level start-------------*/
//���ǰ���־һ���ֳɰ˸��ȼ�������Ӹߵ��ͣ�������С�ļ�����ߣ����ִ�ļ�����͡����Է��������ʾ�����˵ȵ�
#define NGX_LOG_STDERR            0    //����̨����stderr������߼�����־����־�����ݲ���д��log����ָ�����ļ������ǻ�ֱ�ӽ���־�������׼�����豸�������̨��Ļ
#define NGX_LOG_EMERG             1    //���� ��emerg��
#define NGX_LOG_ALERT             2    //���� ��alert��
#define NGX_LOG_CRIT              3    //���� ��crit��
#define NGX_LOG_ERR               4    //���� ��error�������ڳ��ü���
#define NGX_LOG_WARN              5    //���� ��warn�������ڳ��ü���
#define NGX_LOG_NOTICE            6    //ע�� ��notice��
#define NGX_LOG_INFO              7    //��Ϣ ��info��
#define NGX_LOG_DEBUG             8    //���� ��debug������ͼ���

#define NGX_ERROR_LOG_PATH       "logs/error1.log"   //������־��ŵ�·�����ļ��� 
/*----------------Log Level end-------------*/