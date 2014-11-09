#ifndef _Z_STRING_H__
#define _Z_STRING_H__

typedef struct {
  // raw pointer, usually won't changed unless we already freed it
  char *raw;
  char *ptr;
  int raw_len;
  int len;
  //
  int is_const;
} zstr_t;

zstr_t* zstr_init(zstr_t *str, const char *init_val);

zstr_t* zstr_init_const(zstr_t *str, const char *init_val);

void zstr_deinit(zstr_t *str);

zstr_t* zstr_lstrip(zstr_t *str);

zstr_t* zstr_rstrip(zstr_t *str);

zstr_t* zstr_strip(zstr_t *str);

zstr_t* zstr_make_nullend(zstr_t *str);


#endif // _Z_STRING_H__

