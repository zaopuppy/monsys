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

// only use it if target string is short
int kmp_string_search(const char *target, uint32_t target_len,
                      const char *str, uint32_t str_len)
{
  int *next;

  // pre-process next array
  // 求next字符串的关键，在于next[i]的值，必须表示到第i个字符时，从头能匹配到的最大字串
  // 如果仅仅表示“匹配到第i个字符串时，应该从哪个字符开始重新比较”，那么next[i]所指的值，
  // 最后一个字符是不确定的，不成参考
  // 如果非要按第二种方式使用怎么办？直接增加一个判断即可，确保第i个字符的比较状态时确定的
  {
    next = new int[str_len];

    next[0] = -1;
    int j;
    for (int i = 1; i < str_len; ++i) {
      j = i - 1;
      while ((j >= 0) && (next[j] != str[i])) {
        j = next[j];
      }

    }
  }

  // search it
  {
    int i = 0;
    int j = 0;
    while (true) {
      if (target[i] == str[j]) {
        ++i;
        ++j;
        if (j == str_len) {
          return i - str_len;
        }
      } else {
        j = next[j];
        if (j < 0) {
          ++i;
          j = 0;
        }
      }
      if (i >= target_len) {
        // end of text
        return -1;
      }

    }
  }

  return -1;
}
