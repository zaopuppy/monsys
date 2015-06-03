//
// Created by Yi Zhao on 4/8/15.
//

#ifndef MONSYS_THREAD_H
#define MONSYS_THREAD_H

#include <pthread.h>

namespace Z {

class Thread {

public:
  bool init() { return true; }

  bool start();

protected:
  virtual void run() = 0;

private:
  pthread_t handle_;

};

}

#endif //MONSYS_THREAD_H
