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

// static
void ZClient::socket_callback(evutil_socket_t fd, short events, void *arg) {
	// Z_LOG_D("socket_callback(%d, %d)\n", fd, events);
	assert(arg);
	ZClient *m = (ZClient*)arg;
	m->event(fd, events);
}

// static
void ZClient::timeout_callback(evutil_socket_t fd, short events, void *arg)
{
	// Z_LOG_D("timeout_callback(%d, %d)\n", fd, events);
	assert(arg);
	ZClient *m = (ZClient*)arg;
	m->doTimeout();
}

void ZClient::doTimeout()
{
	Z_LOG_D("ZClient::doTimeout(): %p\n", this);

	timeout_event_proxy_.cancel();

	switch (state_) {
		case STATE_WAITING_FOR_CONNECT:
			Z_LOG_I("connecting timeout, abort, waiting for reconnect: %p", this);
			disconnect();
			break;
		case STATE_DISCONNECTED:
			onDisconnected(fd_, 1);
			break;
		default:
			break;
	}

}

int ZClient::init() {
	// XXX
	ZDispatcher::instance()->registerModule(this);
	// if (ZDispatcher::instance()->registerModule(this) != OK) {
	// 	return FAIL;
	// }

	int rv = onDisconnected(-1, 0);
	if (rv != OK && rv != ERR_IO_PENDING) {
		return FAIL;
	}

	if (handler_) {
		handler_->init();
	}

	return OK;
}

int ZClient::sendMsg(ZInnerMsg *msg)
{
	Z_LOG_D("ZClient::sendMsg(%p)\n", msg);
	if (handler_) {
		handler_->onInnerMsg(msg);
	}
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

	socket_event_proxy_.cancel();
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
	// Z_LOG_D("ZClient::event()\n");
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

	// clear timeout checker
	timeout_event_proxy_.cancel();

	int val = -1;
	socklen_t val_len = sizeof(val);
	int rv = getsockopt(fd, SOL_SOCKET, SO_ERROR, &val, &val_len);
	if (rv == 0 && val == 0) {
		Z_LOG_I("Connected: %p\n", this);

		socket_event_proxy_.registerSocket(fd, EV_READ|EV_PERSIST, (void*)this, NULL);

		state_ = STATE_CONNECTED;
		if (handler_) {
			Z_LOG_D("handler_->fd_ = %d\n", fd);
			handler_->fd_ = fd;
			handler_->onConnected();
		}
	} else if (rv == 0 && val == EINPROGRESS) { // val is errno, errno won't be set in this situation
		Z_LOG_W("still connecting, rv=%d, val=%d, errno=%d: %p\n", rv, val, errno, this);
		// one shot event
		socket_event_proxy_.registerSocket(fd, EV_WRITE, this, NULL);
	} else {
		Z_LOG_D("Failed to connect, rv=%d, val=%d, errno=%d\n", rv, val, errno);

		::close(fd_);
		fd_ = -1;
		state_ = STATE_DISCONNECTED;

		scheduleReconnect();
	}

	return OK;

}

void ZClient::scheduleReconnect() {
	Z_LOG_D("ZClient::scheduleReconnect: %p\n", this);
	state_ = STATE_DISCONNECTED;
	timeout_event_proxy_.registerTimeout(this, &RETRY_INTERVAL);
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
		// perror("recv");
		Z_LOG_D("Failed to receive data from socket: errno=%d\n", errno);
		::close(fd);
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		return;
	}

	if (handler_) {
		handler_->onRead(buf_, rv);
	}
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
				socket_event_proxy_.registerSocket(fd_, EV_READ|EV_PERSIST, (void*)this, NULL);
				if (handler_) {
					Z_LOG_D("handler_->fd_ = %d\n", fd);
					handler_->fd_ = fd;
					handler_->onConnected();
				}
				break;
			}
		case ERR_IO_PENDING:
			{
				Z_LOG_I("Waiting for response: %p\n", this);
				state_ = STATE_WAITING_FOR_CONNECT;

				// one shot write event
				socket_event_proxy_.registerSocket(fd_, EV_WRITE, (void*)this, NULL);

				// one-shot timer
				// if old timer is still there, remove it first
				timeout_event_proxy_.registerTimeout(this, &RETRY_INTERVAL);
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


