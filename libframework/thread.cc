//
// Created by Yi Zhao on 4/8/15.
//

#include "thread.h"

#include <stddef.h>

static void* thread_func(void *param) {
  if (param == NULL) {
    return NULL;
  }

  Z::Thread *thread = (Z::Thread *) param;

  thread->run();

  return NULL;
}

bool Z::Thread::start() {

  int rv = pthread_create(&handle_, NULL, thread_func, this);
  if (rv != 0) {
    return false;
  }

  return true;
}
