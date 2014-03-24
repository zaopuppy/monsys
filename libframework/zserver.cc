#include "zserver.h"

#include <assert.h>
// ::close
#include <unistd.h>
// inet_ntoa
#include <arpa/inet.h>

#include "zerrno.h"
#include "zdispatcher.h"

// static
void ZServer::socket_callback(evutil_socket_t fd, short events, void *arg)
{
  assert(arg);
  ZServer *p = (ZServer*)arg;
  p->event(fd, events);
}

void ZServer::onTimeout(int handler_id)
{
  // if (handler_id == timer_routine_id_) {
  //   routine(routine_interval_);
  // }
}

// int ZServer::sendMsg(ZInnerMsg *msg)
// {
//   Z_LOG_D("ZServer::sendMsg");
//   return onInnerMsg(msg);
// }

int ZServer::onInnerMsg(ZInnerMsg *msg)
{
  Z_LOG_D("ZServer::onInnerMsg");
  return 0;
}

int ZServer::init() {
  int rv;

  rv = ZDispatcher::instance()->registerModule(this);
  if (rv != OK) {
    return FAIL;
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

  socket_event_proxy_.registerSocket(fd_, EV_READ|EV_PERSIST, this, NULL);

  // // setup a timer
  // Z_LOG_D("Setting up a timer, interval: %d", routine_interval_);
  // timer_routine_id_ = timer_.setTimer(routine_interval_, true);

  return OK;
}

void ZServer::close() {
  // XXX: use event_del to remove event
  ::close(fd_);
  fd_ = -1;
}

void ZServer::event(evutil_socket_t fd, short events) {
  acceptClient(fd, events);
}

void ZServer::acceptClient(evutil_socket_t fd, short events) {
  Z_LOG_D("ZServer::accept()");

  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int clifd = accept(fd, (struct sockaddr*) (&ss), &slen);
  if (clifd < 0) {           // XXX EAGAIN?
    perror("accept");
    ::close(clifd);
    return;
  // } else if (clifd > FD_SETSIZE) {
  //  Z_LOG_D("Maximum size of fd has reached.");
  //  ::close(clifd); // XXX evutil_closesocket
  } else {
    //
    struct sockaddr_in* addr = (struct sockaddr_in*)(&ss);
    unsigned short port = ntohs(addr->sin_port);
    Z_LOG_D("accepted connection from: %s:%u",
        inet_ntoa(addr->sin_addr), port);

    evutil_make_socket_nonblocking(clifd);

    onAccept(clifd, addr, port);
  }
}

