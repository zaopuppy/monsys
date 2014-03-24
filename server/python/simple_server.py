#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging

import gevent
from gevent.server import StreamServer
from gevent.queue import Queue

# logging handler
logger_handler = logging.StreamHandler()
logger_handler.setLevel(logging.DEBUG)

# logger
logger = logging.getLogger("simple_server")
logger.setLevel(logging.DEBUG)
logger.addHandler(logger_handler)

class SimpleServer(StreamServer):
    def __init__(self, listener, handler_class):
        StreamServer.__init__(self, listener)
        self._handler_class = handler_class

    def handle(self, socket, address):
        handler = self._handler_class(socket, address)
        handler.start()

class SimpleHandler():
    def __init__(self, socket, address):
        self._socket = socket
        self.address = address
        self.in_queue = Queue()
        self.out_queue = Queue()

    def start(self):
        in_queue = Queue()
        out_queue = Queue()
        # self._dispatcher.add(self)
        try:
            jobs = [
                gevent.spawn(self._recv_loop),
                gevent.spawn(self._send_loop),
                gevent.spawn(self._event_loop),
            ]
            gevent.joinall(jobs)
        finally:
            logger.debug("task done")
            # self._dispatcher.remove(self)
            self._socket.close()
            gevent.killall(jobs)

    def close(self):
        self.in_queue.put(None)
        self.out_queue.put(None)

    def _recv_loop(self):
        while True:
            data = self._socket.recv(1024)
            if data is None or len(data) <= 0:
                logger.debug("recv broken")
                self.close();
                break
            logger.debug("received: [{}]".format(data))
            msg = self.decode(data)
            self.in_queue.put(msg)

    def _send_loop(self):
        while True:
            data = self.out_queue.get()
            if data is None:
                logger.debug("recv broken")
                break
            # socket.sendall(data)
            self._socket.send(self.encode(data))
            logger.debug("sent: [{}]".format(data))

    def _event_loop(self):
        while True:
            data = self.in_queue.get()
            if data is None:
                logger.debug("event broken")
                break
            # msg = self.decode(data)
            # if msg is not None:
            #     self.event(msg)
            self.event(data)
            
    def event(self, msg):
        raise NotImplementedError()

    def decode(self, data):
        raise NotImplementedError()

    def encode(self, msg):
        raise NotImplementedError()

    def send(self, msg):
        self.in_queue.put(msg)
        # raise NotImplementedError()


