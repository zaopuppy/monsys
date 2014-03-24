#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import json

from database import Database
from session import SessionCtrl

import gevent
from gevent.server import StreamServer
from simple_server import SimpleHandler
from module_define import *

# logging handler
logger_handler = logging.StreamHandler()
logger_handler.setLevel(logging.DEBUG)

# logger
logger = logging.getLogger("fgw_server")
logger.setLevel(logging.DEBUG)
logger.addHandler(logger_handler)

class FGWServer(StreamServer):
    def __init__(self, listener, dispatcher):
        StreamServer.__init__(self, listener)
        self._dispatcher = dispatcher
        self._conn = Database.get_connection()
        self._session_ctrl = SessionCtrl()

    def handle(self, socket, address):
        handler = FGWHandler(
            socket, address, self._dispatcher, self._conn, self._session_ctrl)
        handler.start()

class FGWHandler(SimpleHandler):
    _STATE_WAIT_FOR_LOGIN   = 0
    _STATE_LOGGED_IN        = 1

    def __init__(self, socket, address, dispatcher, db, session_ctrl):
        SimpleHandler.__init__(self, socket, address)
        self.type = MODULE_FGW_SERVER
        self.id = id(self)
        self._dispatcher = dispatcher
        self._conn = db
        self._session_ctrl = session_ctrl

        self._state = self._STATE_WAIT_FOR_LOGIN

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
        logger.debug("event()")
        if self._state == self._STATE_WAIT_FOR_LOGIN:
            self.event_wait_for_login(msg)
        elif self._state == self._STATE_LOGGED_IN:
            self.event_logged_in(msg)
        else:
            self.close()

    def set_state(self, state):
        self._state = state

    def event_wait_for_login(self, msg):
        """
        TODO: 1 sencod waiting timeout
        """
        if "cmd" not in msg.keys():
            logger.error("cmd is not found")
            return

        cmd = msg["cmd"]
        if cmd != "login":
            logger.error("not login request")
            return

        st = msg["st"]
        dev_id = msg["devid"]

        sql = "SELECT count(1) from `fgw_list` where `device` = %s and `pubkey` = %s"

        cur = self._conn.cursor()
        gevent.joinall([ gevent.spawn(cur.execute, sql, (dev_id, st)) ])
        row = cur.fetchone()
        cur.close()

        if row is None:
            self.out_queue.put("bu!")
            return

        rsp = { "cmd": "login-rsp" }
        if row[0] == 1:
            rsp["result"] = 0
            # self._state = self._STATE_LOGGED_IN
            self.set_state(self._STATE_LOGGED_IN)
        else:
            rsp["result"] = -1

        self.out_queue.put(rsp)

    def event_logged_in(self, msg):
        if "cmd" not in msg or "seq" not in msg:
            logger.error("cmd or seq is not found")
            return

        cmd = msg["cmd"]

        logger.debug("cmd: [{}]".format(cmd))

        seq = msg["seq"]

        if cmd in [ "get-dev-list",
                    "get-dev-info",
                    "set-dev-info",
                    "pre-bind",
                    "bind" ]:
            # from webapi server handler
            # check if where's already a session associate it
            if self._session_ctrl.find_by_external_key(seq) is not None:
                logger.error("duplicated request")
                return
            inner_seq = self._session_ctrl.gen_key()
            session = { "ext-key": seq, "key": inner_seq }
            self._session_ctrl.add(inner_seq, seq, session)
            msg["seq"] = inner_seq
            self.out_queue.put(msg)
        elif cmd in [ "get-dev-list-rsp",
                      "get-dev-info-rsp",
                      "set-dev-info-rsp",
                      "pre-bind-rsp",
                      "bind-rsp" ]:
            session = self._session_ctrl.find(seq)
            if session is None:
                logger.error("no session is found")
                return
            ext_seq = session["ext-key"]
            msg["key"] = ext_seq
            self._session_ctrl.remove(seq)
            self._dispatcher.send(MODULE_WEBAPI_SERVER, msg)
        else:
            logger.error("unknown message")


