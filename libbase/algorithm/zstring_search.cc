#include "zstring_search.h"

int simple_string_search(const char *target, uint32_t target_len,
                         const char *str, uint32_t str_len)
{
  // matching empty string is allowed
  if (str_len == 0) {
    return 0;
  }

  if (str_len > target_len) {
    return -1;
  }

  int rv;
  for (uint32_t i = 0; i < target_len - str_len; ++i) {
    rv = memcmp(target + i, str, str_len);
    if (rv == 0) {
      return i;
    }
  }

  return -1;
}

int kmp_string_search(const char *target, uint32_t target_len,
                      const char *str, uint32_t str_len)
{
  return -1;
}
