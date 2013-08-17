#include "zclient.h"

#include <errno.h>
#include <assert.h>
// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

#include "libbase/zlog.h"
#include "zerrno.h"
#include "zdispatcher.h"

// TODO: server wait timeout
// static const struct timeval SERVER_WAIT_TIMEOUT = { 20, 0 };
static const struct timeval RETRY_INTERVAL = { 5, 0 };

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	Z_LOG_D("SOCKET_CALLBACK(%d, %d)\n", fd, events);
	assert(arg);
	ZClient *m = (ZClient*)arg;
	m->event(fd, events);
}

static void TIMEOUT_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	Z_LOG_D("TIMEOUT_CALLBACK(%d, %d)\n", fd, events);
	assert(arg);
	ZClient *m = (ZClient*)arg;
	m->doTimeout();
}

void ZClient::doTimeout()
{
	Z_LOG_D("ZClient::doTimeout()\n");

	// XXX: temporary solution
	if (state_ != STATE_CONNECTED) {
		timeout_event_ = NULL;
		disconnect();
	}
	// scheduleReconnect();
}

int ZClient::init() {
	if (ZDispatcher::instance()->registerModule(this) != OK) {
		return FAIL;
	}

	int rv = onDisconnected(-1, 0);
	if (rv != OK && rv != ERR_IO_PENDING) {
		return FAIL;
	}

	handler_->init();

	return OK;
}

int ZClient::sendMsg(ZInnerMsg *msg)
{
	Z_LOG_D("ZClient::sendMsg(%p)\n", msg);
	handler_->onInnerMsg(msg);
	return 0;
}

int ZClient::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("ZClient::sendMsg(%p)\n", msg);
	return 0;
}

void ZClient::disconnect() {
	if (fd_ >= 0) {
		::close(fd_); // XXX: evutil_closesocket(fd_);
		fd_ = -1;
	}

	// XXX: damn it, it doesn't work
	if (write_event_) {
		event_free(write_event_);
		write_event_ = NULL;
	}

	if (read_event_) {
		event_free(read_event_);
		read_event_ = NULL;
	}

}

// 0: success, connected
//-1: IO_PENDING
// this method should not change current state;
int ZClient::connect() {
	disconnect();

	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		Z_LOG_E("failed to get new socket file descriptor\n");
		return FAIL;
	}

	evutil_make_socket_nonblocking(fd_);

#ifndef WIN32
	{
		int one = 1;
		if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
			perror("setsockopt");
			::close(fd_);
			fd_ = -1;
			return FAIL;
		}
	}
#endif // WIN32

	int rv;

	// // need bind or not?
	// if (false) {
	// 	rv = bind(fd, (struct sockaddr*) (&sin), sizeof(sin));
	// 	if (rv < 0) {
	// 		perror("bind");
	// 		return false;
	// 	}
	// }

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(server_ip_.c_str());
	sin.sin_port = htons(server_port_);

	// connect(int socket, const struct sockaddr *address, socklen_t address_len);
	rv = ::connect(fd_, (struct sockaddr*) (&sin), sizeof(sin));
	if (rv < 0) {
		if (errno != EINPROGRESS) {
			perror("connect");
			Z_LOG_D("Can not initial connection\n");
			::close(fd_); // XXX: evutil_closesocket
			fd_ = -1;
			return FAIL;
		} else {
			// setup one-shot timer
			// if old timer is still there, remove it first
			if (timeout_event_) {
				event_free(timeout_event_);
				timeout_event_ = NULL;
			}
			const struct timeval timeout_val = { timeout_, 0 };
			struct event *timeout_ev = evtimer_new(base_, TIMEOUT_CALLBACK, (void*)this);
			event_add(timeout_ev, &timeout_val);
			return ERR_IO_PENDING;
		}
	}

	return OK;
}

void ZClient::close() {
	::close(fd_); // XXX: evutil_closesocket(fd_);
	fd_ = -1;
	state_ = STATE_FINISHED;
}

void ZClient::event(evutil_socket_t fd, short events) {
	Z_LOG_D("ZClient::event()\n");
	switch (state_) {
		case STATE_WAITING_FOR_CONNECT:
			onWaitingForConnect(fd, events);
			break;
		case STATE_CONNECTED:
			onConnected(fd, events);
			break;
		case STATE_DISCONNECTED:
			onDisconnected(fd, events);
			break;
		case STATE_FINISHED:
			// should never happen
			assert(false);
			break;
		default:
			close();
			break;
	}
}

int ZClient::onWaitingForConnect(evutil_socket_t fd, short events) {
	Z_LOG_D("ZClient::onWaitingForConnect(%d, %d)\n", fd, events);

	// clear timer event first
	if (timeout_event_) {
		event_del(timeout_event_);
		// event_remove_timer(timeout_event_);
		event_free(timeout_event_);
		timeout_event_ = NULL;
	}

	int val = -1;
	socklen_t val_len = sizeof(val);
	int rv = getsockopt(fd, SOL_SOCKET, SO_ERROR, &val, &val_len);
	if (rv == 0 && val == 0) {
		Z_LOG_D("Connected: %p\n", this);
		// event_free(write_event_);
		// write_event_ = NULL;
		read_event_ =
			event_new(base_, fd, EV_READ|EV_PERSIST, SOCKET_CALLBACK, (void*)this);
		event_add(read_event_, NULL);

		state_ = STATE_CONNECTED;
		if (handler_) {
			Z_LOG_D("handler_->fd_ = %d\n", fd);
			handler_->fd_ = fd;
			handler_->onConnected();
		}
	// XXX: why sometimes this fail?
	// } else if (rv == 0 && errno == EINPROGRESS) {
	// 	Z_LOG_W("still connecting: %p\n", this);
	// 	// one shot event
	// 	struct event* ev =
	// 		event_new(base_, fd_, EV_WRITE, SOCKET_CALLBACK, (void*)this);
	// 	event_add(ev, NULL);
	} else {
		Z_LOG_D("Failed to connect, rv=%d, val=%d, errno=%d\n",
			rv, val, errno);
		// event_free(read_event_);
		// read_event_ = NULL;
		// event_free(write_event_);
		// write_event_ = NULL;

		::close(fd_);
		fd_ = -1;
		state_ = STATE_DISCONNECTED;

		scheduleReconnect();
	}

	return OK;

}

void ZClient::scheduleReconnect() {
	// TODO: use timer event instead of temporary event
	state_ = STATE_DISCONNECTED;
	struct event* ev = evtimer_new(base_, SOCKET_CALLBACK, this);
	event_add(ev, &RETRY_INTERVAL);
}

void ZClient::onConnected(evutil_socket_t fd, short events) {
	assert(fd >= 0);

	int rv = recv(fd, buf_, sizeof(buf_), 0);
	if (rv == 0) {
		Z_LOG_D("peer closed\n");
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		// close();
		return;
	} else if (rv < 0) { // XXX EAGAIN
		perror("recv");
		Z_LOG_D("Failed to receive data from socket\n");
		::close(fd);
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		return;
	}

	if (handler_) {
		handler_->onRead(buf_, rv);
	}
	// // == for DEBUGGING only ==
	// if (rv >= (int)sizeof(buf_)) {
	// 	buf_[sizeof(buf_) - 1] = 0x00;
	// } else {
	// 	buf_[rv] = 0x00;
	// }
	// Z_LOG_D("Received: %s\n", buf_);
	// // == for DEBUGGING only ==
}

int ZClient::onDisconnected(evutil_socket_t fd, short events) {
	// state_ = STATE_DISCONNECTED;
	// return doLoop();
	int rv = connect();
	switch (rv) {
		case OK:
			{
				Z_LOG_D("Connected\n");
				state_ = STATE_CONNECTED;
				read_event_ =
					event_new(base_, fd_, EV_READ|EV_PERSIST, SOCKET_CALLBACK, (void*)this);
				event_add(read_event_, NULL);
				if (handler_) {
					Z_LOG_D("handler_->fd_ = %d\n", fd);
					handler_->fd_ = fd;
					handler_->onConnected();
				}
				break;
			}
		case ERR_IO_PENDING:
			{
				Z_LOG_D("Waiting for response\n");
				state_ = STATE_WAITING_FOR_CONNECT;

				// one shot event
				// struct event* ev =
				write_event_ =
					event_new(base_, fd_, EV_WRITE, SOCKET_CALLBACK, (void*)this);
				event_add(write_event_, NULL);
				break;
			}
		case FAIL:
			{
				Z_LOG_D("Failed to connect\n");
				state_ = STATE_DISCONNECTED;

				scheduleReconnect();

				break;
			}
		default:
			{
				rv = FAIL;
				break;
			}
	}

	return rv;
}


