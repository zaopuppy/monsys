#!/usr/bin/env python
# -*- coding: utf-8 -*-

import random

import log


# logger
logger = log.Log.get_logger(__name__)


class Dispatcher():
    def __init__(self):
        self._handler_map = {}

    def add(self, handler):
        if handler.type not in self._handler_map.keys():
            self._handler_map[handler.type] = {}
        if handler.id in self._handler_map[handler.type].keys():
            raise Exception("handler has registered already")
        self._handler_map[handler.type][handler.id] = handler
        logger.info("handler added: {}:{}".format(handler.type, handler.id))

    def remove(self, handler):
        if handler.type not in self._handler_map.keys():
            raise Exception("handler has not been registered yet")
        if handler.id not in self._handler_map[handler.type].keys():
            raise Exception("handler has not been registered yet")
        self._handler_map[handler.type].pop(handler.id)
        logger.info("handler removed: {}:{}".format(handler.type, handler.id))

    def broadcast(self, handler_type, msg):
        if handler_type not in self._handler_map.keys():
            raise Exception("handler has not been registered yet")
        for h in self._handler_map[handler_type].values():
            h.on_inner(msg)

    # def send(self, handler_type, handler_id, msg):
    def send(self, msg):
        # dst_addr = msg.dst_addr
        handler_type = msg.dst_addr[0]
        handler_id = msg.dst_addr[1]
        if handler_id is None:
            if handler_type not in self._handler_map.keys():
                logger.error("handler type not found")
                return
            handler_count = len(self._handler_map[handler_type])
            h = self._handler_map[handler_type].values()[random.randint(0, handler_count-1)]
            h.on_inner(msg)
        else:
            self._handler_map[handler_type][handler_id].on_inner(msg)

