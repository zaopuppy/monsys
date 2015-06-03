//
// Created by Yi Zhao on 4/6/15.
//

#include "zlog_wrapper.h"

#include <stdio.h>


static zlog_category_t *s_log_category = NULL;

static inline void set_category(zlog_category_t *new_category) {
  s_log_category = new_category;
}

zlog_category_t* z_log_get_category() {
  return s_log_category;
}

void trace_bin(const char* data, uint32_t len)
{

}

int z_log_init(const char *log_file, const char *category_name)
{
  int rv = -1;
  rv = zlog_init(log_file);
  if (rv != 0) {
    printf("failed to initialize zlog\n");
    return rv;
  }

  zlog_category_t *category = zlog_get_category(category_name);
  if (category == NULL) {
    printf("failed to get category\n");
    return -1;
  }

  set_category(category);

  return 0;
}

void z_log_fini() {
  zlog_fini();
}

