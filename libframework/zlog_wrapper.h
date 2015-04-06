#ifndef _ZLOG_WRAPPER_H__
#define _ZLOG_WRAPPER_H__


#define Z_LOG_D(_format, ...)   z_log_debug("E|" __FILE__ ":%d|" _format "\n", __LINE__, ##__VA_ARGS__)
#define Z_LOG_I(_format, ...)   z_log_info("E|" __FILE__ ":%d|" _format "\n", __LINE__, ##__VA_ARGS__)
#define Z_LOG_W(_format, ...)   z_log_warn("E|" __FILE__ ":%d|" _format "\n", __LINE__, ##__VA_ARGS__)
#define Z_LOG_E(_format, ...)   z_log_error("E|" __FILE__ ":%d|" _format "\n", __LINE__, ##__VA_ARGS__)
#define Z_LOG_F(_format, ...)   z_log_fatal("E|" __FILE__ ":%d|" _format "\n", __LINE__, ##__VA_ARGS__)

void z_log_init();

void z_log_debug(const char *format, ...);
void z_log_info(const char *format, ...);
void z_log_warn(const char *format, ...);
void z_log_error(const char *format, ...);
void z_log_fatal(const char *format, ...);



#endif // _ZLOG_WRAPPER_H__


