//
// Created by Yi Zhao on 4/8/15.
//

#ifndef MONSYS_UPLOAD_WORKER_H
#define MONSYS_UPLOAD_WORKER_H


#include <libframework/thread.h>

class UploadWorker: public Z::Thread {

public:
  UploadWorker(): stop_(false) {}

  void run();

private:
  bool stop_;

};


#endif //MONSYS_UPLOAD_WORKER_H
