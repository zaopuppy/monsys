#include "zserver_handler.h"

#include "errno.h"

#include "unistd.h"

void ZServerHandler::close()
{
	::close(fd_);
	fd_ = -1;
	event_del(read_event_);
}

int ZServerHandler::event(evutil_socket_t fd, short events)
{
	fd_ = fd;

	unsigned int buf_idx = 0;
	ssize_t len;

	do {
		len = recv(fd, buf_ + buf_idx, sizeof(buf_), 0);
		if (len > 0)
			buf_idx += len;
	} while (buf_idx <= sizeof(buf_) && len > 0);

	printf("len: %ld\n", len);
	if (len < 0 && errno != EAGAIN) {
		perror("recv");
		// freeSession(session);
		close();
		return -1;
	} else if (len == 0) {
		printf("peer closed.\n");
		close();
		return -1;
	}else {
		// XXX, may write beyond the bound.
		if (buf_idx > 0) {
			buf_[buf_idx] = 0x00;
			onRead(buf_, buf_idx);
		}
	}

	return 0;
}




