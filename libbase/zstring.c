#include "zstring.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static size_t roundup_size(size_t size) {
  return (size + 7) & ~0x7;
}

zstr_t* zstr_init(zstr_t *str, const char *init_val)
{
  str->len = strlen(init_val);
  str->raw_len = str->len;
  str->raw = (char*)malloc(roundup_size(str->len));
  memcpy(str->raw, init_val, str->len);
  str->ptr = str->raw;
  str->is_const = 0;
  return str;
}

zstr_t* zstr_init_const(zstr_t *str, const char *init_val)
{
  str->len = strlen(init_val);
  str->raw_len = str->len;
  str->raw = (char*)init_val;
  str->ptr = str->raw;
  str->is_const = 1;
  return str;
}

void zstr_deinit(zstr_t *str)
{
  if (!str->is_const) {
    free(str->raw);
  }

  str->len = 0;
  str->raw_len = 0;
  str->raw = 0;
  str->ptr = 0;
  str->is_const = 0;
}

zstr_t* zstr_lstrip(zstr_t *str)
{
  char *p = str->ptr;
  while (isspace(*p) && (str->len) > 0) {
    ++p;
    --(str->len);
  }

  str->ptr = p;

  return str;
}

zstr_t* zstr_rstrip(zstr_t *str)
{
  if (str->len <= 0) {
    return str;
  }

  char *p = str->ptr + str->len - 1;
  while (isspace(*p) && (str->len) > 0) {
    --p;
    --(str->len);
  }

  return str;
}

zstr_t* zstr_strip(zstr_t *str)
{
  return zstr_rstrip(zstr_lstrip(str));
}

zstr_t* zstr_make_nullend(zstr_t *str)
{
  // if (str->is_const) {
  //   return str;
  // }

  str->ptr[str->len] = 0x00;

  return str;
}


#ifdef __UNIT__

#include <stdio.h>

int main(int argc, char *argv[])
{
  {
    char s[1024] = "   aaa  b  cc  ";

    zstr_t str;
    zstr_init_const(&str, s);

    printf("original: [%s]\n", str.ptr);

    zstr_lstrip(&str);
    zstr_make_nullend(&str);

    printf("lstrip: [%s]\n", str.ptr);

    zstr_rstrip(&str);
    zstr_make_nullend(&str);

    printf("rstrip: [%s]\n", str.ptr);
  }

  {
    char s[1024] = "   aa  a b cc    ";
    zstr_t str;
    zstr_init_const(&str, s);

    printf("original: [%s]\n", str.ptr);

    zstr_strip(&str);
    zstr_make_nullend(&str);

    printf("strip: [%s]\n", str.ptr);
  }

  return 0;
}

#endif // __UNIT__



