// system api
#include <iostream>
#include <assert.h>

// libevent
#include <event2/event.h>

#include "libframework/zframework.h"

// project files
// #include "zsocket.h"
#include "zwebapi_server.h"
#include "libbase/zlog.h"
#include "zserial.h"
#include "fgw_client.h"


using namespace std;


bool start_webapi_server(const char* ip, uint16_t port, event_base* base)
{
	ZModule *module = new ZWebApiServer(ip, port, base);
	if (module->init() != OK) {
		printf("Failed to initialize server.");
		return false;
	}

	return true;
}

bool start_serial(event_base *base)
{
	// const char* serial_dev = "/dev/tty.usbmodemfd141";
	const char* serial_dev = "/dev/tty.usbserial-ftDX0P76";
	// const char* serial_dev = "/dev/tty.usbserial-FTG5WHHL";
	ZModule *h = new ZSerial(base, serial_dev);
	if (h->init() != OK) {
		printf("Failed to init serial module.");
		return false;
	}
	return true;
}


static void routine(evutil_socket_t fd, short events, void *arg)
{
	// printf("routine()");
	// sleep(2);
	ZDispatcher::instance()->routine(100);
}

int main(int argc, char *argv[])
{
	// test_zigbee_message_get();
	// test_zigbee_message_set();
	// test_zigbee_message_reg();
	// test_json();
	// test_util();
	// test_other();
	// test_formatter();
	// test_zigbee_update();
	// Z_LOG_E("hello");
	// return 0;

	struct event_base* base = event_base_new();
	assert(base);
	
	FGWClient *client = new FGWClient(base);
	client->setServerAddress("127.0.0.1", 1984);
	if (OK != client->init()) {
		Z_LOG_E("Failed to initialize fgw client");
		return -1;
	}
	// if (!start_server("0.0.0.0", 1984, base, ZServer::TYPE_ZIGBEE)) {
	// 	printf("failed to start server: (0.0.0.0, 1983).");
	// 	return -1;
	// }

	// if (!start_server("0.0.0.0", 1983, base, ZServer::TYPE_WEBCLIENT)) {
	// 	printf("failed to start server: (0.0.0.0, 1984).");
	// 	return -1;
	// }

	// if (!start_server("0.0.0.0", 1983, base, ZServer::TYPE_APICLIENT)) {
	// 	printf("failed to start server: (0.0.0.0, 1984).");
	// 	return -1;
	// }

	// if (!start_webapi_server("0.0.0.0", 1983, base)) {
	// 	printf("failed to start server: (0.0.0.0, 1983).");
	// 	return -1;
	// }

	// if (!start_api_server("0.0.0.0", 1984, base)) {
	// 	printf("failed to start server: (0.0.0.0, 1983).");
	// 	return -1;
	// }

 	// if (!start_client(base)) {
 	// 	printf("failed to start client.");
 	// 	return -1;
 	// }
	
	if (!start_serial(base)) {
		printf("failed to start serial.");
		return FAIL;
	}

	// long begin_time = ZTime::getInMillisecond();
	// long end_time = 0;
	// long delta_time = 0;

	// setup timeout
	const struct timeval ROUTINE_INTERVAL = { 0, 500 * 1000 };
	ZEventProxy routine_timer(base, routine);
	routine_timer.registerPersistTimeout(NULL, &ROUTINE_INTERVAL);
	// {
	// 	const struct timeval ROUTINE_INTERVAL = { 0, 500 * 1000 };
	// 	// struct event *timeout_ev = evtimer_new(base, routine, NULL);
	// 	// XXX: don't use EV_PEERSIST, time-cost routine may be dangerous
	// 	struct event *timeout_ev = event_new(base, -1, EV_PERSIST, routine, NULL);
	// 	event_add(timeout_ev, &ROUTINE_INTERVAL);
	// }

	// basicly equals to event_base_loop()
	// event_base_dispatch(base);
	while (1) {
		printf("beginning of loop");
		// event_base_loop(base, EVLOOP_NONBLOCK);
		event_base_loop(base, 0);
		printf("end of loop");

		// ZDispatcher::instance()->routine(delta_time);

		// usleep(100 * 1000);	// 100 milliseconds

		// end_time = ZTime::getInMillisecond();
		// delta_time = end_time - begin_time;
		// begin_time = end_time;

		// // printf("delta_time: [%ld]", delta_time);
		// assert(delta_time > 0);
		// if (delta_time <= 0) {
		// 	delta_time = 100;
		// }
	}

	return 0;
}


