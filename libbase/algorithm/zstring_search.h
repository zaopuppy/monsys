#ifndef _Z_STRING_SEARCH_H__
#define _Z_STRING_SEARCH_H__

#include <string.h>

#include "../ztypes.h"

int simple_string_search(const char *target, uint32_t target_len,
                         const char *str, uint32_t str_len);

int kmp_string_search(const char *target, uint32_t target_len,
                      const char *str, uint32_t str_len);

#endif // _Z_STRING_SEARCH_H__

