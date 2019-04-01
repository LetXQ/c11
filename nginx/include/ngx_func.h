#pragma once
#include <stdarg.h> 

using u_char = unsigned char;
using u_int = unsigned int;
// set proc title
void ngx_init_setproctitle();
void ngx_setproctitle(const char *title);

//和日志，打印输出有关
void ngx_log_init();
void ngx_log_stderr(int err, const char *fmt, ...);
u_char *ngx_log_errno(u_char *buf, u_char *last, int err);
void ngx_log_error_core(int level, int err, const char *fmt, ...);

u_char *ngx_slprintf(u_char *buf, u_char *last, const char *fmt, ...);
u_char *ngx_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);