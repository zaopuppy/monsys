// system api
#include <iostream>
#include <assert.h>

#include <event2/event.h>

#include "fgw_server.h"

using namespace std;

ZServer *g_server = NULL;

static void routine(evutil_socket_t fd, short events, void *arg)
{
	g_server->routine(delta);
}

int main(int argc, char *argv[])
{
	struct event_base* base = event_base_new();
	assert(base);

	g_server = new FGWServer("0.0.0.0", 1983, base);
	if (OK != g_server->init()) {
		Z_LOG_E("Failed to start FGW Server, quit\n");
		return -1;
	}

	{
		const struct timeval ROUTINE_INTERVAL = { 0, 500 * 1000 };
		// struct event *timeout_ev = evtimer_new(base, routine, NULL);
		// XXX: don't use EV_PEERSIST, time-cost routine may be dangerous
		struct event *timeout_ev = event_new(base, -1, EV_PERSIST, routine, NULL);
		event_add(timeout_ev, &ROUTINE_INTERVAL);
	}

	// basicly equals to event_base_loop()
	// event_base_dispatch(base);
	while (1) {
		printf("beginning of loop\n");
		// event_base_loop(base, EVLOOP_NONBLOCK);
		event_base_loop(base, 0);
		printf("end of loop\n");

	}

	return 0;
}


