// system api
#include <iostream>
#include <assert.h>

#include <event2/event.h>

#include "fgw_server.h"
#include "zwebapi_server.h"
#include "libframework/zframework.h"
#include "database.h"

#include "echo_server.h"

using namespace std;

ZServer *g_fgw_server = NULL;
ZServer *g_webapi_server = NULL;

static void routine(evutil_socket_t fd, short events, void *arg)
{
	g_fgw_server->routine(500);
	g_webapi_server->routine(500);
}

int main(int argc, char *argv[])
{
	Z_LOG_I("Starting center...");

	{
		if (MySQLDatabase::instance()->init() != 0) {
			Z_LOG_E("Failed to initialize database");
			return -1;
		}

		if (MySQLDatabase::instance()->connect() != 0) {
			Z_LOG_E("Failed to connect to database");
			return -1;
		}
	}

	struct event_base* base = event_base_new();
	assert(base);

	// push server
	g_fgw_server = new FGWServer("0.0.0.0", 1984, base);
	if (OK != g_fgw_server->init()) {
		Z_LOG_E("Failed to start FGW Server, quit");
		return -1;
	}

	// webapi server
	g_webapi_server = new ZWebApiServer("0.0.0.0", 1983, base);
	if (OK != g_webapi_server->init()) {
		Z_LOG_E("Failed to start WEBAPI Server, quit");
		return -1;
	}

	// echo server
	ZServer *echo_server = new EchoServer("0.0.0.0", 4444, base);
	if (OK != echo_server->init()) {
		Z_LOG_E("Failed to initialize echo server, quit");
		return -1;
	}

	const struct timeval ROUTINE_INTERVAL = { 0, 500 * 1000 };
	ZEventProxy routine_timer(base, routine);
	routine_timer.registerPersistTimeout(NULL, &ROUTINE_INTERVAL);
	// basicly equals to event_base_loop()
	event_base_dispatch(base);
	// while (1) {
	// 	printf("beginning of loop");
	// 	// event_base_loop(base, EVLOOP_NONBLOCK);
	// 	event_base_loop(base, 0);
	// 	printf("end of loop");

	// }

	return 0;
}


