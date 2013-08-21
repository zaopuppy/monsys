// system api
#include <iostream>
#include <assert.h>

#include <event2/event.h>

#include "fgw_client.h"
#include "libframework/zframework.h"

using namespace std;

const struct timeval ROUTINE_INTERVAL = { 1, 0 };

std::vector<FGWClient*> g_client_list;

ZEventProxy *g_routine_event_proxy = NULL;

static void routine(evutil_socket_t fd, short events, void *arg)
{
	// Z_LOG_D("routine()\n");
	// struct event *timeout_ev = event_new(g_base, -1, 0, routine, NULL);
	// event_add(timeout_ev, &ROUTINE_INTERVAL);
	g_routine_event_proxy->registerTimeout(NULL, &ROUTINE_INTERVAL);

	// if (g_timeout_ev) {
	// 	event_free(g_timeout_ev);
	// 	g_timeout_ev = NULL;
	// }
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

	const int client_count = 200;
	for (int i = 0; i < client_count; ++i) {
		if (0 != connectToServer("127.0.0.1", 1984, base)) {
		// if (0 != connectToServer("10.34.45.45", 1983, base)) {
			return -1;
		}
	}

	g_routine_event_proxy = new ZEventProxy(base, routine);
	g_routine_event_proxy->registerTimeout(NULL, &ROUTINE_INTERVAL);

	// basicly equals to event_base_loop()
	event_base_dispatch(base);
	// while (1) {
	// 	Z_LOG_D("beginning of loop\n");
	// 	// event_base_loop(base, EVLOOP_NONBLOCK);
	// 	event_base_loop(base, 0);
	// 	Z_LOG_D("end of loop\n");

	// }

	return 0;
}


