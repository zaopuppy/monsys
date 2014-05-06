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

from msg import Msg

# logger
logger = log.Log.get_logger(__name__)

class GetDevListSession(Session):
    def __init__(self, env, handler):
        Session.__init__(self)
        self.env = env
        self.handler = handler
        self._state = 1

    def on_timeout(self):
        self.close()

    def event(self, msg):
        if self._state == 1:
            if msg["cmd"] != "get-dev-list":
                self.close()
                return
            msg["seq"] = self.key
            self.handler.out_queue.put(msg)
            self._state = 2
        elif self._state == 2:
            if msg["cmd"] != "get-dev-list-rsp":
                self.close()
                return
            self.env.dispatcher.send(
                Msg(self.handler.inner_addr, self.src_addr, msg))
            self.close()
        else:
            self.close()


class GetDevInfoSession(Session):
    def __init__(self, env, handler):
        Session.__init__(self)
        self.env = env
        self.handler = handler
        self._state = 1

    def on_timeout(self):
        self.close()

    def event(self, msg):
        if self._state == 1:
            if msg["cmd"] != "get-dev-info":
                self.close()
                return
            msg["seq"] = self.key
            self.handler.out_queue.put(msg)
            self._state = 2
        elif self._state == 2:
            if msg["cmd"] != "get-dev-info-rsp":
                self.close()
                return
            self.env.dispatcher.send(
                Msg(self.handler.inner_addr, self.src_addr, msg))
            self.close()
        else:
            self.close()

class SetDevInfoSession(Session):
    def __init__(self, env, handler):
        Session.__init__(self)
        self.env = env
        self.handler = handler
        self._state = 1

    def on_timeout(self):
        self.close()

    def event(self, msg):
        if self._state == 1:
            if msg["cmd"] != "set-dev-info":
                self.close()
                return
            msg["seq"] = self.key
            self.handler.out_queue.put(msg)
            self._state = 2
        elif self._state == 2:
            logger.debug("SetDevInfoSession.event(), state=1")
            if msg["cmd"] != "set-dev-info-rsp":
                self.close()
                return
            self.env.dispatcher.send(
                Msg(self.handler.inner_addr, self.src_addr, msg))
            self.close()
        else:
            self.close()

class PreBindSession(Session):
    def __init__(self, env, handler):
        Session.__init__(self)
        self.env = env
        self.handler = handler
        self._state = 1

    def on_timeout(self):
        self.close()

    def event(self, msg):
        if self._state == 1:
            if msg["cmd"] != "pre-bind":
                self.close()
                return

            account = msg["account"]
            fgw = msg["fgw"]

            sql = "SELECT `fgw_list` from `account_info` where `account` = %s"

            cur = self.handler.conn.cursor()
            # XXX if pymysql support multi-session in the same connection
            # we need a connection pool
            gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
            row = cur.fetchone()
            cur.close()

            if row is None:
                self.env.dispatcher.send(
                    Msg(self.handler.inner_addr, self.src_addr, { "cmd": "pre-bind-rsp", "result": -1 }))
                self.close()
                return

            fgw_list = row[0].split("|")

            logger.debug("fgw-list: " + row[0])

            # if fgw in fgw_list:
            #     self.env.dispatcher.send(
            #         MODULE_WEBAPI_SERVER, None, { "cmd": "pre-bind-rsp", "result": -1 })
            #     return

            self.account = account
            self.fgw = fgw

            msg["seq"] = self.key
            self.handler.out_queue.put(msg)
            self._state = 2
        elif self._state == 2:
            if msg["cmd"] != "pre-bind-rsp":
                self.close()
                return
            self.env.dispatcher.send(
                Msg(self.handler.inner_addr, self.src_addr, msg))
            self.close()
        else:
            self.close()

class BindSession(Session):
    def __init__(self, env, handler):
        Session.__init__(self)
        self.env = env
        self.handler = handler
        self._state = 1

    def on_timeout(self):
        self.close()

    def event(self, msg):
        if self._state == 1:
            if msg["cmd"] != "bind":
                self.close()
                return

            account = msg["account"]
            fgw = msg["fgw"]

            sql = "SELECT `fgw_list` from `account_info` where `account` = %s"

            cur = self.handler.conn.cursor()
            # XXX if pymysql support multi-session in the same connection
            # we need a connection pool
            gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
            row = cur.fetchone()
            cur.close()

            if row is None:
                self.env.dispatcher.send(
                    Msg(self.handler.inner_addr, self.src_addr, { "cmd": "bind-rsp", "result": -1 }))
                self.close()
                return

            fgw_list = row[0].split("|")

            logger.debug("fgw-list: " + row[0])

            # if fgw in fgw_list:
            #     self.env.dispatcher.send(
            #         MODULE_WEBAPI_SERVER, None, { "cmd": "pre-bind-rsp", "result": -1 })
            #     return

            self.account = account
            self.fgw = fgw

            msg["seq"] = self.key
            self.handler.out_queue.put(msg)
            self._state = 2
        elif self._state == 2:
            if msg["cmd"] != "bind-rsp":
                self.close()
                return

            account = self.account
            fgw = self.fgw
            
            sql = "SELECT `fgw_list` from `account_info` WHERE `account` = %s"

            # 1. query
            cur = self.handler.conn.cursor()
            gevent.joinall([ gevent.spawn(cur.execute, sql, account) ])
            row = cur.fetchone()

            if row is None:
                logger.error("no record")
                cur.close()
                self.env.dispatcher.send(
                    Msg(self.inner_addr, session.src_addr, { "cmd": "pre-bind-rsp", "result": -1 }))
                self.close()
                return
            
            fgw_list = row[0].split("|")
            
            logger.debug("fgw-list: " + row[0])
            
            if fgw not in fgw_list:
                fgw_list.append(fgw)
            
            sql = "UPDATE `account_info` SET `fgw_list` = %s WHERE `account` = %s"
            gevent.joinall([
                gevent.spawn(cur.execute, sql, ("|".join(fgw_list), account))
                ])
            
            cur.close()
            
            self.handler.conn.commit()
            
            msg["key"] = self.ext_key
            self.env.dispatcher.send(
                Msg(self.handler.inner_addr, self.src_addr, msg))

            self.close()
        else:
            self.close()

class FGWServer(StreamServer):
    def __init__(self, listener, env):
        StreamServer.__init__(self, listener)
        self.env = env
        self.conn = Database.get_connection()

    def handle(self, socket, address):
        handler = FGWHandler(
            socket, address, self.env, self.conn)
        handler.start()

class FGWHandler(SimpleHandler):
    _STATE_WAIT_FOR_LOGIN   = 0
    _STATE_LOGGED_IN        = 1

    def __init__(self, socket, address, env, db):
        SimpleHandler.__init__(self, socket, address)
        self.type = MODULE_FGW_SERVER
        self.id = id(self)
        self.env = env
        self.conn = db
        self.inner_addr = (self.type, self.id)

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

    def create_session(self, key, ext_key, src_addr, cmd):
        if cmd == "get-dev-list":
            session = GetDevListSession(self.env, self)
        elif cmd == "get-dev-info":
            session = GetDevInfoSession(self.env, self)
        elif cmd == "set-dev-info":
            session = SetDevInfoSession(self.env, self)
        elif cmd == "pre-bind":
            session = PreBindSession(self.env, self)
        elif cmd == "bind":
            session = BindSession(self.env, self)
        else:
            return None

        session.ext_key = ext_key
        session.key = key
        session.src_addr = src_addr

        return session

    def on_inner(self, msg):
        logger.debug("on_inner()")
        inner_msg = msg.data;
        cmd = inner_msg["cmd"]
        seq = inner_msg["seq"]
        if self._state == self._STATE_WAIT_FOR_LOGIN:
            logger.error("not logged in yet")
        elif self._state == self._STATE_LOGGED_IN:
            logger.debug("cmd: {}".format(cmd))
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
                session = self.create_session(inner_seq, seq, msg.src_addr, cmd)
                if not session:
                    logger.error("failed to create session")
                    return
                session.event(inner_msg)
                if not session.is_complete():
                    self._session_ctrl.add(inner_seq, seq, session)
        else:
            logger.error("abnormal state: [{}]".format(self._state))
            self.close()

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

        cur = self.conn.cursor()
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
        seq = msg["seq"]

        logger.debug("cmd: [{}], seq: [{}]".format(cmd, seq))

        # session = self.find_or_create_session(cmd, seq);
        if cmd in [ "get-dev-list-rsp",
                    "get-dev-info-rsp",
                    "set-dev-info-rsp",
                    "pre-bind-rsp",
                    "bind-rsp" ]:
            session = self._session_ctrl.find(seq)
            session.event(msg)
            if session.is_complete():
                self._session_ctrl.remove(seq)
        else:
            logger.error("unknown message")

    def event_logged_in_old(self, msg):
        if "cmd" not in msg or "seq" not in msg:
            logger.error("cmd or seq is not found")
            return

        cmd = msg["cmd"]

        logger.debug("fgw_server: cmd: [{}]".format(cmd))

        seq = msg["seq"]

        if cmd in [ "get-dev-list-rsp",
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
            self.env.dispatcher.send(
                Msg(self.inner_addr, session.src_addr, msg))
        elif cmd == "bind-rsp":
            self.bind_fgw_rsp(msg)
        else:
            logger.error("unknown message")



