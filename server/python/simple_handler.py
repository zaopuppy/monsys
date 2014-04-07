#!/usr/bin/env python
# -*- coding: utf-8 -*-

import log

import gevent
from gevent.queue import Queue

# logger
logger = log.Log.get_logger(__name__)

class SimpleHandler():
    def __init__(self, socket, address):
        self._socket = socket
        self.address = address
        self.in_queue = Queue()
        self.out_queue = Queue()
        self._is_running = False

    def start(self):
        in_queue = Queue()
        out_queue = Queue()
        # self._dispatcher.add(self)
        self._is_running = True
        try:
            jobs = [
                gevent.spawn(self._recv_loop),
                gevent.spawn(self._send_loop),
                gevent.spawn(self._event_loop),
                gevent.spawn(self._routine_loop),
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
        self._is_running = False

    def _routine_loop(self):
        while self._is_running:
            self.routine()
            gevent.sleep(1)
        logger.debug("routine broken")

    def routine(self):
        raise NotImplementedError()

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
        logger.debug("send()")
        self.in_queue.put(msg)
        logger.debug("send() done")
        # raise NotImplementedError()



