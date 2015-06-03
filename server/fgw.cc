#include <iostream>

#include <event2/event.h>

#include "libframework/zframework.h"

#include "zserial.h"
#include "fgw_client.h"
#include "upload_worker.h"

using namespace std;


int main(int argc, char *argv[])
{
  z_log_init("fgw.conf", "fgw");

  struct event_base* base = event_base_new();
  assert(base);

  ZDispatcher::init(base);

  // const char *serial_dev = "/dev/ttyUSB0";
  // const char *serial_dev = "/dev/tty.usbserial-FTG5WHHL";
  const char *serial_dev = "/dev/tty.usbserial-ftDX0P76";
  ZModule *serial = new ZSerial(base, serial_dev);
  if (serial->init() != OK) {
    Z_LOG_D("Failed to init serial module.");
    return FAIL;
  }

  FGWClient *client = new FGWClient(base);
  client->setServerAddress("192.168.2.105", 1984);
  if (OK != client->init()) {
    Z_LOG_E("Failed to initialize fgw client");
    return -1;
  }

  UploadWorker worker;
  worker.start();

  bool stop = false;

  while (!stop) {
    event_base_loop(base, EVLOOP_ONCE);
    // event_base_loop(base, 0);
    serial->checkMsgQueue();
    client->checkMsgQueue();
    // Z_LOG_D("end of loop");
  }

  z_log_fini();

  return 0;
}


