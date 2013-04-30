#include "zserver.h"

#include <assert.h>
// ::close
#include <unistd.h>
// inet_ntoa
#include <arpa/inet.h>

#include "zapi_module.h"
// #include "zwebapi_module.h"
#include "zzigbee_module.h"

#include "zerrno.h"

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	assert(arg);
	ZTask* task = (ZTask*)arg;
	task->event(fd, events);
}

int ZServer::init() {
	int rv = super_::init();
	if (rv != 0) {
		return rv;
	}

	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	assert(fd_ >= 0);

	evutil_make_socket_nonblocking(fd_);

#ifndef WIN32
	{
		int one = 1;
		setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
#endif // WIN32

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	// sin.sin_addr.s_addr = 0; // Listen IP
	inet_aton(ip_.c_str(), &sin.sin_addr);
	sin.sin_port = htons(port_);

	rv = bind(fd_, (struct sockaddr*) (&sin), sizeof(sin));
	if (rv < 0) {
		perror("bind");
		return FAIL;
	}

	rv = listen(fd_, 16);
	if (rv < 0) {
		perror("listen");
		return FAIL;
	}

	struct event* listen_event =
		event_new(base_, fd_, EV_READ|EV_PERSIST, SOCKET_CALLBACK, (void*)this);
	assert(listen_event);

	event_add(listen_event, NULL);

	state_ = STATE_ACCEPTING;

	return OK;
}

void ZServer::close() {
	::close(fd_);
	fd_ = -1;
	state_ = STATE_FINISHED;
}

void ZServer::event(evutil_socket_t fd, short events) {
	switch (state_) {
		case STATE_ACCEPTING:
			acceptClient(fd, events);
			break;
		default:
			close();
			break;
	}
}

void ZServer::doTimeout() {
}

bool ZServer::isComplete() {
	return (state_ == STATE_FINISHED);
}

int ZServer::onInnerMsg(ZInnerMsg *msg) {
	printf("ZServer::onInnerMsg()");
	return 0;
}

// static ZTask*
// getAcceptedSession(int type, event_base* base) {
// 	switch (type) {
// 		case ZServer::TYPE_ZIGBEE:
// 			return new ZZigBeeModule(base);
// 		case ZServer::TYPE_APICLIENT:
// 			return new ZApiModule(base);
// 		// case ZServer::TYPE_WEBCLIENT:
// 		// 	return new ZWebApiModule(base);
// 		default:
// 			printf("Unknown server type\n");
// 			assert(false);
// 			return NULL;
// 	}
// }

void ZServer::acceptClient(evutil_socket_t fd, short events) {
	printf("ZServer::accept()\n");

	struct sockaddr_storage ss;
	socklen_t slen = sizeof(ss);
	int clifd = accept(fd, (struct sockaddr*) (&ss), &slen);
	if (clifd < 0) {           // XXX EAGAIN?
		perror("accept");
	} else if (clifd > FD_SETSIZE) {
		printf("Maximum size of fd has reached.\n");
		::close(clifd); // XXX evutil_closesocket
	} else {
		//
		struct sockaddr_in* addr = (struct sockaddr_in*)(&ss);
		unsigned short port = ntohs(addr->sin_port);
		printf("accepted connection from: %s:%u\n",
				inet_ntoa(addr->sin_addr), port);

		evutil_make_socket_nonblocking(clifd);

		onAccept(clifd, addr, port);
		// ZTask* task = getAcceptedSession(type_, base_);
		// assert(task);

		// task->read_event_ =
		// 	event_new(base_, clifd, EV_READ|EV_PERSIST, SOCKET_CALLBACK, task);

		// assert(task->init() == OK);

		// evutil_make_socket_nonblocking(clifd);
		// event_add(task->read_event_, NULL);
	}
}

// void ZServer::onAccept_o(evutil_socket_t fd, short events) {
// 	printf("ZServer::onAccept()\n");
// 
// 	struct sockaddr_storage ss;
// 	socklen_t slen = sizeof(ss);
// 	int clifd = accept(fd, (struct sockaddr*) (&ss), &slen);
// 	if (clifd < 0) {           // XXX EAGAIN?
// 		perror("accept");
// 	} else if (clifd > FD_SETSIZE) {
// 		printf("Maximum size of fd has reached.\n");
// 		::close(clifd); // XXX evutil_closesocket
// 	} else {
// 		//
// 		struct sockaddr_in* addr = (struct sockaddr_in*)(&ss);
// 		unsigned short port = ntohs(addr->sin_port);
// 		printf("accepted connection from: %s:%u\n",
// 				inet_ntoa(addr->sin_addr), port);
// 
// 		//
// 		// ZAcceptedSession* session = new ZAcceptedSession(base_);
// 		ZTask* task = getAcceptedSession(type_, base_);
// 		assert(task);
// 
// 		task->read_event_ =
// 			event_new(base_, clifd, EV_READ|EV_PERSIST, SOCKET_CALLBACK, task);
// 
// 		assert(task->init() == OK);
// 
// 		evutil_make_socket_nonblocking(clifd);
// 		event_add(task->read_event_, NULL);
// 		// ZClientSocket* client = new ZClientSocket(base_event);
// 		// assert(client);
// 		// client->read_event = event_new(base_event, clifd, EV_READ|EV_PERSIST, SOCKET_DO_READ, client);
// 		// assert(client->read_event);
// 
// 		// assert(client->init());
// 
// 		// evutil_make_socket_nonblocking(clifd);
// 		// event_add(client->read_event, NULL);
// 	}
// }




