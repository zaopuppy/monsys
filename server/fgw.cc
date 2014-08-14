#include <iostream>
#include <assert.h>

// libevent
#include <event2/event.h>

#include "libframework/zframework.h"

#include "libbase/zlog.h"
#include "zserial.h"
#include "fgw_client.h"

using namespace std;


bool start_serial(event_base *base)
{
  // const char* serial_dev = "/dev/tty.usbmodemfd141";
  // const char* serial_dev = "/dev/tty.usbserial-ftDX0P76";
  const char* serial_dev = "/dev/ttyUSB0";
  // const char* serial_dev = "/dev/tty.usbserial-FTG5WHHL";
  ZModule *h = new ZSerial(base, serial_dev);
  if (h->init() != OK) {
    Z_LOG_D("Failed to init serial module.");
    return false;
  }
  return true;
}

typedef struct {
  event_base *base;
  int max_client_count;
  int client_count;
  ZEventProxy *proxy;
} test_param_t;

// const int MAX_CLIENT_COUNT = 1000;

void test_callback(evutil_socket_t fd, short events, void *arg)
{
  test_param_t *param = (test_param_t*)arg;
  Z_LOG_E("[%d] Time to create a new client..", param->client_count);
  if (param->client_count >= param->max_client_count) {
    Z_LOG_E("Maximum client count reached, canel timer");
    param->proxy->cancel();
    return;
  }

  FGWClient *client = new FGWClient(param->base);
  if (OK != client->init()) {
    Z_LOG_E("Failed to initialize client");
  }
}

int main(int argc, char *argv[])
{

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

  // ZEventProxy test_event(base, test_callback);
  // test_param_t param;
  // param.base = base;
  // param.max_client_count = 1;
  // param.client_count = 1;
  // struct timeval tv = { 1, 0 };
  // test_event.registerPersistTimeout(&param, &tv);

  // basicly equals to event_base_loop()
  // event_base_dispatch(base);
  while (1) {
    // Z_LOG_D("beginning of loop");
    event_base_loop(base, EVLOOP_ONCE);
    // event_base_loop(base, 0);
    serial->checkMsgQueue();
    client->checkMsgQueue();
    // Z_LOG_D("end of loop");
  }

  return 0;
}


