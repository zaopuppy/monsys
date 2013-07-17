//
//  zlog.h
//  base
//
//  Created by Zhao yi on 2/7/13.
//  Copyright (c) 2013 222. All rights reserved.
//

#ifndef base_zlog_h
#define base_zlog_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LOG_NONE		0
#define LOG_ERROR		1
#define LOG_WARN		2
#define LOG_DEBUG		3
#define LOG_INFO		4

// modify this macro for different log level
#define LOG_LVL 	LOG_INFO

#if (LOG_LVL >= LOG_ERROR)
	#define Z_LOG_E(format, ...)		zlog("E|" format, ##__VA_ARGS__)
#else
	#define Z_LOG_E(...)
#endif

#if (LOG_LVL >= LOG_WARN)
	#define Z_LOG_W(format, ...)		zlog("W|" format, ##__VA_ARGS__)
#else
	#define Z_LOG_W(...)
#endif

#if (LOG_LVL >= LOG_DEBUG)
	#define Z_LOG_D(format, ...)		zlog("D|" format, ##__VA_ARGS__)
#else
	#define Z_LOG_D(...)
#endif

#if (LOG_LVL >= LOG_INFO)
	#define Z_LOG_I(format, ...)		zlog("I|" format, ##__VA_ARGS__)
#else
	#define Z_LOG_I(...)
#endif

char printable_char(char c);
char hex2char(unsigned char h);
void trace_bin(const char* data, uint32_t len);

void zlog(const char *format, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

