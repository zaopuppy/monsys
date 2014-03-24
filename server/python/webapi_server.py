#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import json

# import pymysql

from database import Database

import gevent
from gevent.server import StreamServer
from simple_server import SimpleHandler
from module_define import *

# logging handler
logger_handler = logging.StreamHandler()
logger_handler.setLevel(logging.DEBUG)

# logger
logger = logging.getLogger("webapi_server")
logger.setLevel(logging.DEBUG)
logger.addHandler(logger_handler)

class WebApiServer(StreamServer):
    def __init__(self, listener, dispatcher):
        StreamServer.__init__(self, listener)
        self._dispatcher = dispatcher
        self._conn = Database.get_connection()

    def handle(self, socket, address):
        handler = WebApiHandler(
            socket, address, self._dispatcher, self._conn)
        handler.start()

class WebApiHandler(SimpleHandler):
    def __init__(self, socket, address, dispatcher, db):
        SimpleHandler.__init__(self, socket, address)
        self.type = MODULE_WEBAPI_SERVER
        self.id = id(self)
        self._dispatcher = dispatcher
        self._conn = db

    def start(self):
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
        if cmd == "get-fgw-list":
            self.process_get_fgw_list(msg)
        elif cmd in [ "get-dev-list",
                      "get-dev-info",
                      "set-dev-info",
                      "pre-bind",
                      "bind" ]:
            self._dispatcher.send(MODULE_FGW_SERVER, msg)
        elif cmd in [ "get-dev-list-rsp",
                      "get-dev-info-rsp",
                      "set-dev-info-rsp",
                      "pre-bind-rsp",
                      "bind-rsp" ]:
            self.out_queue.put(msg)
        else:
            logger.error("unknown message")

    def process_get_fgw_list(self, msg):
        """
        { "cmd": "get-fgw-list", "account": "ztest@gmail.com" }
        """
        logger.debug("process_get_fgw_list()")

        sql = "SELECT `fgw_list` from `account_info` where `account` = %s"
        account = msg["account"]

        cur = self._conn.cursor()
        gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
        row = cur.fetchone()
        cur.close()

        if row is None:
            self.out_queue.put("bu\n")
            return

        rsp = { "cmd": "get-fgw-list-rsp" }
        rsp["fgws"] = row[0].split("|")

        self.out_queue.put(rsp)
