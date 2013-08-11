// system api
#include <iostream>
#include <assert.h>

#include <event2/event.h>

#include "fgw_client.h"

using namespace std;

std::vector<FGWClient*> g_client_list;

static void routine(evutil_socket_t fd, short events, void *arg)
{
}

int connectToServer(const char *ip, short port, event_base *base)
{
	FGWClient *client = new FGWClient(base);
	if (!client) {
		Z_LOG_E("client == NULL\n");
		exit(-1);
	}
	client->setServerAddress(ip, port);
	if (0 != client->init()) {
		Z_LOG_E("Failed to initialize fgw client\n");
		return -1;
	}

	g_client_list.push_back(client);

	Z_LOG_D("current %u clients\n", g_client_list.size());

	return 0;
}

int main(int argc, char *argv[])
{
	struct event_base* base = event_base_new();
	assert(base);

	// g_server = new FGWServer("0.0.0.0", 1983, base);
	// if (OK != g_server->init()) {
	// 	Z_LOG_E("Failed to start FGW Server, quit\n");
	// 	return -1;
	// }

	const int client_count = 1;
	for (int i = 0; i < client_count; ++i) {
		if (0 != connectToServer("127.0.0.1", 1983, base)) {
		// if (0 != connectToServer("10.34.45.45", 1983, base)) {
			return -1;
		}
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
		Z_LOG_D("beginning of loop\n");
		// event_base_loop(base, EVLOOP_NONBLOCK);
		event_base_loop(base, 0);
		Z_LOG_D("end of loop\n");

	}

	return 0;
}


