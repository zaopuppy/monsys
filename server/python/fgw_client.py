#!/usr/bin/env python
# -*- coding: utf-8 -*-

import log

import json

import gevent
import gevent.socket

from simple_handler import SimpleHandler
from module_define import *

# logger
logger = log.Log.get_logger(__name__)



class FGWClient():
    def __init__(self, server_address, dispatcher):
        self.server_address = server_address
        self._dispatcher = dispatcher
        self._socket = None

    def run(self):
        while True:
            if self.connect():
                logger.debug("connected")
                handler = FGWClientHandler(self._socket, self.server_address, self._dispatcher)
                handler.start()
            else:
                gevent.sleep(5)

    def connect(self):
        try:
            self._socket = gevent.socket.create_connection(
                self.server_address, timeout=20)
            return True
        except Exception as e:
            logger.error("exception happen: [{}]".format(e))
            return False

class FGWClientHandler(SimpleHandler):
    def __init__(self, socket, address, dispatcher):
        SimpleHandler.__init__(self, socket, address)
        self.type = MODULE_FGW_CLIENT
        self.id = id(self)
        self._dispatcher = dispatcher

    def start(self):
        """
        TODO: in case of sending message while other modules are still starting,
        we should save message to a message queue and send to module if they're ready.
        Or, let modules register themself at the very beginning of problem, then start
        them all
        """
        self._dispatcher.add(self)
        SimpleHandler.start(self)
        self._dispatcher.remove(self)

    def decode(self, data):
        try:
            return json.loads(data)
        except ValueError as e:
            logger.error("failed to decode message: [{}]".format(e))
            return None

    def encode(self, msg):
        return json.dumps(msg)

    def event(self, msg):
        if "cmd" not in msg.keys():
            logger.error("no cmd in msg")
            return
        cmd = msg["cmd"]

        if cmd in [ "get-dev-list",
                    "get-dev-info",
                    "set-dev-info",
                    "pre-bind",
                    "bind" ]:
            self._dispatcher.send(MODULE_SERIAL, msg)
        elif cmd in [ "get-dev-list-rsp",
                      "get-dev-info-rsp",
                      "set-dev-info-rsp",
                      "pre-bind-rsp",
                      "bind-rsp" ]:
            self.out_queue.put(msg)
        else:
            logger.error("unknown message")


