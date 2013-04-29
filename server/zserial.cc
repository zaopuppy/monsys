#include "zserial.h"

#include <assert.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
// #include <unistd.h>
#include <sys/ioctl.h>

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

	ZData *data = (ZData*)msg->data;

	printf("Data.data: %d\n", data->data);

	if (fd_ < 0) {
		printf("not connected yet\n");
		return OK;
	}

	switch (data->data) {
	case 1:
		{
			ZZBGetReq msg;
			int rv = msg.encode(buf_out_, sizeof(buf_out_));
			if (rv < 0) {
				printf("Failed to encode\n");
			} else {
				trace_bin(buf_out_, rv);
				// rv = send(fd_, buf_, rv, 0);

				// // RTS
				// {
				// 	int status;
				// 	ioctl(fd_, TIOCMGET, &status);
				// 	status |= TIOCM_RTS;
				// 	ioctl(fd_, TIOCMSET, &status);
				// }

				sleep(1);

				rv = write(fd_, buf_out_, rv);
				if (rv <= 0) {
					perror("send");
					printf("Failed to send\n");
				} else {
					printf("write over.\n");
				}

				// // DTR
				// {
				// 	int status;

				// 	ioctl(fd_, TIOCMGET, &status);
				// 	status |= TIOCM_DTR;
				// 	ioctl(fd_, TIOCMSET, &status);
				// }
			}
			break;
		}
	case 2:
		break;
	default:
		break;
	}
	
	return OK;
}

void ZSerial::onConnected(evutil_socket_t fd, short events)
{
	printf("ZSerial::onConnected\n");
	
	int rv;
	int offset = 0;
	size_t buf_len = sizeof(buf_);

	do {
		rv = read(fd, buf_ + offset, buf_len);
		printf("read %d bytes\n", rv);
		if (rv > 0) {
			buf_len -= rv;
			offset += rv;
		}
	} while (rv > 0 && buf_len > 0);

	if (buf_len <= 0) {
		printf("Receved too many bytes...no.\n");
	}

	if (offset > 0) {
		printf("Received:\n");
		trace_bin(buf_, offset);
	}

	if (rv == 0) {
		printf("peer closed\n");
		::close(fd);
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		return;
	} else if (rv < 0 && errno != EAGAIN) {
		perror("read");
		printf("failed to read from serial port, errno: [%d]\n", errno);
		::close(fd);
		state_ = STATE_DISCONNECTED;
		scheduleReconnect();
		return;
	}	
}

int ZSerial::connect()
{
	// const char* serial_dev = "/dev/tty.usbmodemfd141";
	// const char* serial_dev = "/dev/cu.usbmodemfa121";
	// const char* serial_dev = "/dev/cu.usbserial-ftDX0P76";
	const char* serial_dev = "/dev/tty.usbserial-FTG5WHHL";
	fd_ = open(serial_dev, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
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

  fcntl(fd_, F_SETOWN, getpid());

	fcntl(fd_, F_SETFL, FASYNC | FNDELAY);

	// setting
	{
		struct termios opts;

		tcgetattr(fd_, &opts);

		// cfsetispeed(&opts, B38400);
		// cfsetospeed(&opts, B38400);
		cfsetispeed(&opts, B9600);
		cfsetospeed(&opts, B9600);

		// control
		// with hardware flow control
		// opts.c_cflag |= (CLOCAL | CREAD | CRTSCTS);
		// without hardware flow control
		opts.c_cflag |= (CLOCAL | CREAD);
		// 8N1
		// set character size
		opts.c_cflag &= ~PARENB;
		opts.c_cflag &= ~CSTOPB;
		opts.c_cflag &= ~CSIZE;
		opts.c_cflag |= CS8;

		// local options
		opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

		opts.c_iflag = IGNPAR;
		opts.c_oflag = 0;
		opts.c_cc[VMIN] = 1;
		opts.c_cc[VTIME] = 0;

		tcsetattr(fd_, TCSANOW, &opts);
	}

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

