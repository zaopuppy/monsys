//
// Created by Yi Zhao on 4/8/15.
//

#ifndef MONSYS_UPLOAD_WORKER_H
#define MONSYS_UPLOAD_WORKER_H


#include <libframework/thread.h>
#include <libframework/zinner_message.h>

class UploadWorker: public Z::Thread {

public:
  UploadWorker(): stop_(false) {}

  int send(ZInnerMsg *msg);

protected:
  void run();

private:
  bool stop_;

};


#endif //MONSYS_UPLOAD_WORKER_H
