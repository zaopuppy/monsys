#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json

# py-serial module
import serial

import gevent
import gevent.socket

import log


from module_define import *

from simple_handler import SimpleHandler

# logger
logger = log.Log.get_logger(__name__)


class CoreWirelessControl():
    def __init__(self, port, dispatcher):
        self.port = port
        self._dispatcher = dispatcher
        self._stream = None

    def run(self):
        while True:
            if self.connect():
                print("Connected")
                handler = WirelessControlHandler(self._stream, self.port, self._dispatcher)
                handler.start()
            else:
                gevent.sleep(5)

    def connect(self):
        try:
            self._stream = serial.Serial(self.port, baudrate=38400, timeout=1.0)
            return True
        except Exception as e:
            logger.error("Exception happen: [{}]".format(e))
            return False

class WirelessControlHandler(SimpleHandler):
    def __init__(self, stream, port, dispatcher):
        self._stream = stream
        self.port = port
        self._dispatcher = dispatcher

    def event(self, msg):
        raise NotImplementedError()

    def decode(self, data):
        try:
            return json.loads(data)
        except ValueError as e:
            logger.error("failed to decode message: [{}]".format(e))
            return None

    def encode(self, msg):
        return json.dumps(msg)

    def event(self, msg):
        pass

