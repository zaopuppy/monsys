#!/usr/bin/env python
# -*- coding: utf-8 -*-

import log


import gevent
import gevent.socket

# logger
logger = log.Log.get_logger(__name__)



class SimpleClient():
    def __init__(self, server_addr, handler_class):
        self.server_addr = server_addr
        self._handler_class = handler_class
        self._socket = None

    def run(self):
        self.connect()
        handler = self._handler_class(self._socket, self.server_addr)
        handler.start()

    def connect():
        self._socket = gevent.socket.create_connection(
            address=self.server_addr, timeout=20)

