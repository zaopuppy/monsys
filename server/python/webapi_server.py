#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json

import log

from database import Database

import gevent
from gevent.server import StreamServer
from simple_handler import SimpleHandler
from module_define import *

# logger
logger = log.Log.get_logger(__name__)

class WebApiServer(StreamServer):
    def __init__(self, listener, env):
        StreamServer.__init__(self, listener)
        self.env = env
        self._conn = Database.get_connection()

    def handle(self, socket, address):
        handler = WebApiHandler(
            socket, address, self.env, self._conn)
        handler.start()

class WebApiHandler(SimpleHandler):
    def __init__(self, socket, address, env, db):
        SimpleHandler.__init__(self, socket, address)
        self.type = MODULE_WEBAPI_SERVER
        self.id = id(self)
        self.env = env
        self._conn = db

    def start(self):
        self.env.dispatcher.add(self)
        SimpleHandler.start(self)
        self.env.dispatcher.remove(self)

    def decode(self, data):
        try:
            return json.loads(data)
        except ValueError as e:
            logger.error("failed to decode message: [{}]".format(e))
            return None

    def encode(self, msg):
        return json.dumps(msg)

    def routine(self):
        # logger.debug("routine()")
        pass

    def event(self, msg):
        if "cmd" not in msg:
            logger.error("no cmd in msg")
            return
        cmd = msg["cmd"]

        logger.debug("webapi_server: cmd: [{}]".format(cmd))

        if cmd == "get-fgw-list":
            self.process_get_fgw_list(msg)
        elif cmd in [ "get-dev-list",
                      "get-dev-info",
                      "set-dev-info",
                      "pre-bind",
                      "bind" ]:
            fgw_id = msg["fgw"]
            handler_id = self.env.fgw_manager.find_handler(fgw_id)
            if handler_id is None:
                logger.error("not logged in yet")
                return
            self.env.dispatcher.send(MODULE_FGW_SERVER, handler_id, msg)
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
