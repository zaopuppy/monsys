#!/usr/bin/env python
# -*- coding: utf-8 -*-

import log

import json

from database import Database
from session import SessionCtrl, Session

import gevent
from gevent.server import StreamServer
from simple_handler import SimpleHandler
from module_define import *

# logger
logger = log.Log.get_logger(__name__)


class FGWServer(StreamServer):
    def __init__(self, listener, env):
        StreamServer.__init__(self, listener)
        self.env = env
        self._conn = Database.get_connection()

    def handle(self, socket, address):
        handler = FGWHandler(
            socket, address, self.env, self._conn)
        handler.start()

class FGWHandler(SimpleHandler):
    _STATE_WAIT_FOR_LOGIN   = 0
    _STATE_LOGGED_IN        = 1

    def __init__(self, socket, address, env, db):
        SimpleHandler.__init__(self, socket, address)
        self.type = MODULE_FGW_SERVER
        self.id = id(self)
        self.env = env
        self._conn = db
        self._session_ctrl = SessionCtrl()

        self._state = self._STATE_WAIT_FOR_LOGIN
        self._fgw_id = None

    def start(self):
        self.env.dispatcher.add(self)
        SimpleHandler.start(self)
        self.env.dispatcher.remove(self)
        self.env.fgw_manager.remove_handler(self._fgw_id)

    def decode(self, data):
        try:
            return json.loads(data)
        except ValueError as e:
            logger.error("failed to decode message: [{}]".format(e))
            return None

    def encode(self, msg):
        return json.dumps(msg)

    def routine(self):
        self._session_ctrl.check_timeout(1)

    def event(self, msg):
        logger.debug("event()")
        if self._state == self._STATE_WAIT_FOR_LOGIN:
            self.event_wait_for_login(msg)
        elif self._state == self._STATE_LOGGED_IN:
            self.event_logged_in(msg)
        else:
            logger.error("abnormal state: [{}]".format(self._state))
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
            self.env.fgw_manager.add_handler(dev_id, self.id)
            self._fgw_id = dev_id
            self.set_state(self._STATE_LOGGED_IN)
        else:
            rsp["result"] = -1

        self.out_queue.put(rsp)

    def event_logged_in(self, msg):
        if "cmd" not in msg or "seq" not in msg:
            logger.error("cmd or seq is not found")
            return

        cmd = msg["cmd"]

        logger.debug("fgw_server: cmd: [{}]".format(cmd))

        seq = msg["seq"]

        if cmd in [ "get-dev-list",
                    "get-dev-info",
                    "set-dev-info" ]:
            # from webapi server handler
            # check if where's already a session associate it
            if self._session_ctrl.find_by_external_key(seq) is not None:
                logger.error("duplicated request")
                return
            inner_seq = self._session_ctrl.gen_key()
            # session = { "ext-key": seq, "key": inner_seq }
            session = Session()
            session.ext_key = seq
            session.key = inner_seq
            self._session_ctrl.add(inner_seq, seq, session)
            msg["seq"] = inner_seq
            self.out_queue.put(msg)
        elif cmd in [ "get-dev-list-rsp",
                      "get-dev-info-rsp",
                      "set-dev-info-rsp",
                      "pre-bind-rsp" ]:
            session = self._session_ctrl.find(seq)
            if session is None:
                logger.error("no session is found")
                return
            ext_seq = session.ext_key
            msg["key"] = ext_seq
            self._session_ctrl.remove(seq)
            self.env.dispatcher.send(MODULE_WEBAPI_SERVER, None, msg)
        elif cmd == "pre-bind":
            self.prebind_fgw(msg)
        elif cmd == "bind":
            self.bind_fgw(msg)
        elif cmd == "bind-rsp":
            self.bind_fgw_rsp(msg)
        else:
            logger.error("unknown message")

    def bind_fgw_rsp(self, msg):
        logger.debug("bind_fgw_rsp()")

        seq = msg["seq"]
        session = self._session_ctrl.find(seq)
        if session is None:
            logger.error("no session is found")
            return

        account = session.account
        fgw = session.fgw

        sql = "SELECT `fgw_list` from `account_info` WHERE `account` = %s"

        # 1. query
        cur = self._conn.cursor()
        gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
        row = cur.fetchone()

        if row is None:
            logger.error("no record")
            cur.close()
            self.env.dispatcher.send(
                MODULE_WEBAPI_SERVER, None, { "cmd": "pre-bind-rsp", "result": -1 })

        fgw_list = row[0].split("|")

        logger.debug("fgw-list: " + row[0])

        if fgw not in fgw_list:
            fgw_list.append(fgw)

        sql = "UPDATE `account_info` SET `fgw_list` = %s WHERE `account` = %s"
        gevent.joinall([
            gevent.spawn(cur.execute, sql, ("|".join(fgw_list), account))
            ])

        cur.close()

        self._conn.commit()

        ext_seq = session.ext_key
        msg["key"] = ext_seq
        self._session_ctrl.remove(seq)
        self.env.dispatcher.send(MODULE_WEBAPI_SERVER, None, msg)

    def bind_fgw(self, msg):
        logger.debug("bind_fgw()")
        account = msg["account"]
        fgw = msg["fgw"]

        sql = "SELECT `fgw_list` from `account_info` where `account` = %s"

        cur = self._conn.cursor()
        gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
        row = cur.fetchone()
        cur.close()

        if row is None:
            self.env.dispatcher.send(
                MODULE_WEBAPI_SERVER, None, { "cmd": "bind-rsp", "result": -1 })
            return

        fgw_list = row[0].split("|")

        logger.debug("fgw-list: " + row[0])

        # if fgw in fgw_list:
        #     self.env.dispatcher.send(
        #         MODULE_WEBAPI_SERVER, None, { "cmd": "bind-rsp", "result": -1 })
        #     return

        seq = msg["seq"]
        # from webapi server handler
        # check if where's already a session associate it
        if self._session_ctrl.find_by_external_key(seq) is not None:
            logger.error("duplicated request")
            return
        inner_seq = self._session_ctrl.gen_key()
        # session = { "ext-key": seq, "key": inner_seq }
        session = Session()
        session.account = account
        session.fgw = fgw
        session.ext_key = seq
        session.key = inner_seq
        self._session_ctrl.add(inner_seq, seq, session)
        msg["seq"] = inner_seq
        self.out_queue.put(msg)

    def prebind_fgw(self, msg):
        logger.debug("prebind_fgw()")
        account = msg["account"]
        fgw = msg["fgw"]

        sql = "SELECT `fgw_list` from `account_info` where `account` = %s"

        cur = self._conn.cursor()
        gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
        row = cur.fetchone()
        cur.close()

        if row is None:
            self.env.dispatcher.send(
                MODULE_WEBAPI_SERVER, None, { "cmd": "pre-bind-rsp", "result": -1 })
            return

        fgw_list = row[0].split("|")

        logger.debug("fgw-list: " + row[0])

        # if fgw in fgw_list:
        #     self.env.dispatcher.send(
        #         MODULE_WEBAPI_SERVER, None, { "cmd": "pre-bind-rsp", "result": -1 })
        #     return

        seq = msg["seq"]
        # from webapi server handler
        # check if where's already a session associate it
        if self._session_ctrl.find_by_external_key(seq) is not None:
            logger.error("duplicated request")
            return
        inner_seq = self._session_ctrl.gen_key()
        # session = { "ext-key": seq, "key": inner_seq }
        session = Session()
        session.account = account
        session.fgw = fgw
        session.ext_key = seq
        session.key = inner_seq
        self._session_ctrl.add(inner_seq, seq, session)
        msg["seq"] = inner_seq
        self.out_queue.put(msg)

