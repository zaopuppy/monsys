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

char printable_char(char c);
char hex2char(unsigned char h);
void trace_bin(const char* data, uint32_t len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

