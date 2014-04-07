#!/usr/bin/env python
# -*- coding: utf-8 -*-

import log


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


