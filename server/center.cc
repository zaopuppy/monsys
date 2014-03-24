// system api
#include <iostream>
#include <assert.h>

#include <event2/event.h>

#include "libframework/zframework.h"

#include "fgw_server.h"
#include "zwebapi_server.h"
#include "database.h"
#include "zconfig.h"

#include "echo_server.h"

using namespace std;

int main(int argc, char *argv[])
{
  Z_LOG_I("Starting center...");

  // global event_base
  struct event_base* base = event_base_new();
  assert(base);

  // database
  {
    Z_LOG_I("Initializing database connection");

    if (MySQLDatabase::instance()->init() != 0) {
      Z_LOG_E("Failed to initialize database");
      return -1;
    }

    if (MySQLDatabase::instance()->connect() != 0) {
      Z_LOG_E("Failed to connect to database");
      return -1;
    }

    Z_LOG_I("done");
  }

  //
  ZDispatcher::init(base);

  // push server
  Z_LOG_I("Initializing push server(%s:%d)", PUSH_SERVER_IP, PUSH_SERVER_PORT);
  ZServer *fgw_server = new FGWServer(PUSH_SERVER_IP, PUSH_SERVER_PORT, base);
  if (OK != fgw_server->init()) {
    Z_LOG_E("Failed to start push server, quit");
    return -1;
  }
  Z_LOG_I("done.");

  // webapi server
  Z_LOG_I("Initializing web api server(%s:%d)", WEB_API_SERVER_IP, WEB_API_SERVER_PORT);
  ZServer *webapi_server = new ZWebApiServer(WEB_API_SERVER_IP, WEB_API_SERVER_PORT, base);
  if (OK != webapi_server->init()) {
    Z_LOG_E("Failed to start web api Server, quit");
    return -1;
  }
  Z_LOG_I("done.");

  // echo server
  Z_LOG_I("Initializing echo server(%s:%d)", ECHO_SERVER_IP, ECHO_SERVER_PORT);
  ZServer *echo_server = new EchoServer(ECHO_SERVER_IP, ECHO_SERVER_PORT, base);
  if (OK != echo_server->init()) {
    Z_LOG_E("Failed to initialize echo server, quit");
    return -1;
  }
  Z_LOG_I("done.");

  // main-loop
  // basicly equals to event_base_loop()
  // event_base_dispatch(base);
  while (1) {
    event_base_loop(base, EVLOOP_ONCE);
    fgw_server->checkMsgQueue();
    webapi_server->checkMsgQueue();
    echo_server->checkMsgQueue();
  }

  Z_LOG_I("exited normally");

  return 0;
}


