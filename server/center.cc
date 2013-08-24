// system api
#include <iostream>
#include <assert.h>

#include <event2/event.h>

#include "fgw_server.h"
#include "zwebapi_server.h"
#include "libframework/zframework.h"

using namespace std;

ZServer *g_server = NULL;

static void routine(evutil_socket_t fd, short events, void *arg)
{
	g_server->routine(500);
}

int main(int argc, char *argv[])
{
	struct event_base* base = event_base_new();
	assert(base);

	// push server
	g_server = new FGWServer("0.0.0.0", 1984, base);
	if (OK != g_server->init()) {
		Z_LOG_E("Failed to start FGW Server, quit\n");
		return -1;
	}

	// webapi server
	ZServer *webapi_server = new ZWebApiServer("0.0.0.0", 1983, base);
	if (OK != webapi_server->init()) {
		Z_LOG_E("Failed to start WEBAPI Server, quit\n");
		return -1;
	}

	const struct timeval ROUTINE_INTERVAL = { 0, 500 * 1000 };
	ZEventProxy routine_timer(base, routine);
	routine_timer.registerPersistTimeout(NULL, &ROUTINE_INTERVAL);
	// basicly equals to event_base_loop()
	event_base_dispatch(base);
	// while (1) {
	// 	printf("beginning of loop\n");
	// 	// event_base_loop(base, EVLOOP_NONBLOCK);
	// 	event_base_loop(base, 0);
	// 	printf("end of loop\n");

	// }

	return 0;
}


