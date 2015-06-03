//
// Created by Yi Zhao on 4/8/15.
//

#include "upload_worker.h"

#include <stdio.h>
#include <unistd.h>
#include <libframework/zerrno.h>

void UploadWorker::run() {
  for (; !stop_;) {
    sleep(1);
    printf("hi~~~~~\n");
  }
}

int UploadWorker::send(ZInnerMsg *msg) {
  if (msg == NULL) {
    return FAIL;
  }

  switch (msg->msg_type_) {
    default:
      return FAIL;
  }
}

