//
// Created by Yi Zhao on 4/8/15.
//

#include "upload_worker.h"

#include <stdio.h>
#include <unistd.h>

void UploadWorker::run() {
  for (; !stop_;) {
    sleep(1);
    printf("hi~~~~~\n");
  }
}
