#ifndef _ZLOG_WRAPPER_H__
#define _ZLOG_WRAPPER_H__


#include <zlog.h>

#include "ztypes.h"


#define Z_LOG_D(_format, ...)   zlog_debug(z_log_get_category(), "D|" __FILE__ ":%d|" _format, __LINE__, ##__VA_ARGS__)
#define Z_LOG_I(_format, ...)   zlog_info(z_log_get_category(), "I|" __FILE__ ":%d|" _format, __LINE__, ##__VA_ARGS__)
#define Z_LOG_W(_format, ...)   zlog_warn(z_log_get_category(), "W|" __FILE__ ":%d|" _format, __LINE__, ##__VA_ARGS__)
#define Z_LOG_E(_format, ...)   zlog_error(z_log_get_category(), "E|" __FILE__ ":%d|" _format, __LINE__, ##__VA_ARGS__)
#define Z_LOG_F(_format, ...)   zlog_fatal(z_log_get_category(), "F|" __FILE__ ":%d|" _format, __LINE__, ##__VA_ARGS__)


int z_log_init(const char *log_file, const char *category_name);

void z_log_fini();

zlog_category_t* z_log_get_category();

void trace_bin(const char* data, uint32_t len);


#endif // _ZLOG_WRAPPER_H__

