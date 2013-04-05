#include "zserial.h"

#include <assert.h>
#include <fcntl.h>
#include <termios.h>

#include "zerrno.h"
#include "zlog.h"

static const struct timeval RETRY_INTERVAL = { 5, 0 };

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	// printf("SOCKET_CALLBACK\n");
	assert(arg);
	ZTask *task = (ZTask*)arg;
	task->event(fd, events);
}


int ZSerial::init()
{
	int rv = super_::init();
	if (rv != OK) {
		return rv;
	}

	rv = onDisconnected(-1, 0);
	if (rv != OK && rv != ERR_IO_PENDING) {
		return FAIL;
	}
	
	return OK;
}

void ZSerial::close()
{
}

void ZSerial::event(evutil_socket_t fd, short events)
{
	printf("ZSerial::event()");
	switch (state_) {
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

void ZSerial::doTimeout()
{
	printf("ZSerial::onTimeout()\n");
}

bool ZSerial::isComplete()
{
	printf("ZSerial::isComplete()\n");
	return false;
}

int ZSerial::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZSerial::onInnerMsg()\n");
	return OK;
}

void ZSerial::onConnected(evutil_socket_t fd, short events)
{
	int rv = read(fd, buf_, sizeof(buf_));
	if (rv == 0) {
		printf("peer closed\n");
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		return;
	} else if (rv < 0) { // XXX EAGAIN
		perror("read");
		printf("failed to read from serial port.\n");
		::close(fd);
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		return;
	}

	if (rv >= (int)sizeof(buf_)) {
		printf("Receved too many bytes...no.\n");
	}

	printf("Received:\n");

	trace_bin(buf_, rv);
}

int ZSerial::connect()
{
	const char* serial_dev = "/dev/tty.usbmodemfd141";
	fd_ = open(serial_dev, O_RDWR|O_NONBLOCK);
	// fd_ = open(serial_dev, O_RDONLY|O_NONBLOCK);
	if (fd_ < 0) {
		perror(serial_dev);
		return FAIL;
	}

	// TODO:
	//	struct sigaction saio;
	// 	saio.sa_handler = signal_handler_IO;
	// 	saio.sa_mask = 0;
	// 	saio.sa_flags = 0;
	// 	saio.sa_restorer = NULL;
	// 	sigaction(SIGIO, &saio, NULL);
  //	fcntl(fd_, F_SETOWN, getpid());
	// TODO:
	fcntl(fd_, F_SETFL, FASYNC);

	// ┌─────────[Comm Parameters]──────────┐ 
	// │                                    │ 
	// │     Current:  9600 8N1             │ 
	// │ Speed            Parity      Data  │ 
	// │ A: <next>        L: None     S: 5  │ 
	// │ B: <prev>        M: Even     T: 6  │ 
	// │ C:   9600        N: Odd      U: 7  │ 
	// │ D:  38400        O: Mark     V: 8  │ 
	// │ E: 115200        P: Space          │ 
	// │                                    │ 
	// │ Stopbits                           │ 
	// │ W: 1             Q: 8-N-1          │ 
	// │ X: 2             R: 7-E-1          │ 
	// │                                    │ 
	// │                                    │ 
	// │ Choice, or <Enter> to exit?        │ 
	// └────────────────────────────────────┘ 
	// setting
	struct termios oldtio, newtio;
	// save first
	tcgetattr(fd_, &oldtio);
	// newtio.c_cflag = BAUDRATE|CRTSCTS|CS8|CLOCAL|CREAD;
	// 8N1: 8 bits| no-parity | 1 stop bit
	newtio.c_cflag = B9600|CRTSCTS|CS8|CLOCAL|CREAD;
	newtio.c_iflag = IGNPAR|ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	tcsetattr(fd_, TCSANOW, &newtio);

	state_ = STATE_CONNECTED;

	printf("serial port initialized.\n");

	read_event_ =
		event_new(base_, fd_, EV_READ|EV_PERSIST, SOCKET_CALLBACK, (void*)this);
	if (read_event_ == NULL) {
		printf("failed to new event\n");
		return FAIL;
	}
	
	int rv = event_add(read_event_, NULL);
	if (rv != 0) {
		printf("failed to add new event\n");
		return FAIL;
	}

	return OK;
}

int ZSerial::onDisconnected(evutil_socket_t fd, short events)
{
	int rv = connect();
	switch (rv) {
	case OK:
		{
			state_ = STATE_CONNECTED;
			
			return OK;
		}
	case FAIL:
		{
			state_ = STATE_DISCONNECTED;
			scheduleReconnect();
			return ERR_IO_PENDING;
		}
	default:
		break;
	}

	return FAIL;

}
void ZSerial::onRead(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
	printf("ZSerial::onRead\n");

	// TODO: should be a loop...

	uint8_t msg_type = ZZigBeeMsg::getMsgType(buf, buf_len);
	switch (msg_type) {
	case Z_ID_ZB_GET_REQ:
		{
			printf("Z_ID_ZB_GET_REQ\n");
			ZZBGetReq msg;
			int rv = msg.decode(buf, buf_len);
			if (rv < 0) {
				printf("Failed to decode message\n");
			}
			break;
		}
	case Z_ID_ZB_GET_RSP:
		{
			printf("Z_ID_ZB_GET_RSP\n");
			ZZBGetRsp msg;
			int rv = msg.decode(buf, buf_len);
			if (rv < 0) {
				printf("Failed to decode message\n");
			}
			break;
		}
	case Z_ID_ZB_SET_REQ:
		{
			printf("Z_ID_ZB_SET_REQ\n");
			ZZBSetReq msg;
			int rv = msg.decode(buf, buf_len);
			if (rv < 0) {
				printf("Failed to decode message\n");
			}
			break;
		}
	case Z_ID_ZB_SET_RSP:
		{
			printf("Z_ID_ZB_SET_RSP\n");
			ZZBSetRsp msg;
			int rv = msg.decode(buf, buf_len);
			if (rv < 0) {
				printf("Failed to decode message\n");
			}
			break;
		}
	default:
		{
			printf("Unknow message: %u\n", msg_type);
			break;
		}
	}
}

void ZSerial::scheduleReconnect()
{
	struct event* ev = evtimer_new(base_, SOCKET_CALLBACK, this);
	event_add(ev, &RETRY_INTERVAL);
}

