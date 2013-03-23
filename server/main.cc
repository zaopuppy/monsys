// system api
#include <iostream>
#include <assert.h>

// libevent
#include <event2/event.h>

// project files
// #include "zsocket.h"
#include "zserver.h"
#include "zclient.h"
#include "zerrno.h"
#include "zlog.h"
#include "zdispatcher.h"

using namespace std;

// static void freeSession(ZTask *session)
// {
//     event_free(session->read_event);
//     // event_free(session->write_event);
//     delete session;
// }

bool start_server(const char* ip, uint16_t port, event_base* base, int type)
{
	ZTask *task = new ZServer(ip, port, base, type);
		// new ZServer(base, ZServer::TYPE_TERM);
	if (task->init() != OK) {
		printf("Failed to initialize server.\n");
		return false;
	}
	return true;
}

bool start_client(event_base* base) {
	int count = 1;

	ZTask *client;
	for (int i = 0; i < count; ++i) {
		client = new ZClient(base);
		if (client->init() != OK) {
			printf("Failed to init client: %d\n", i);
			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[])
{
	trace_bin("asf", 3);

	struct event_base* base = event_base_new();
	assert(base);

	if (!start_server("0.0.0.0", 1983, base, ZServer::TYPE_ZIGBEE)) {
		printf("failed to start server: (0.0.0.0, 1983).\n");
		return -1;
	}

	if (!start_server("0.0.0.0", 1984, base, ZServer::TYPE_TERM)) {
		printf("failed to start server: (0.0.0.0, 1984).\n");
		return -1;
	}

	// if (!start_client(base)) {
	// 	printf("failed to start client.\n");
	// 	return -1;
	// }

	// event_base_dispatch(base);
	while (1) {
		// printf("loop\n");
		event_base_loop(base, EVLOOP_NONBLOCK);
		ZDispatcher::instance()->routine();
		usleep(50 * 1000);	// 100 milliseconds
	}

	return 0;
}


