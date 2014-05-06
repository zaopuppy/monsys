#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time

import log

# logger
logger = log.Log.get_logger(__name__)

class Session():
    def __init__(self):
        self.touched_time = time.time()
        self.closed = False
        self.key = None
        self.ext_key = None

    def check_timeout(self, timeout):
        cur_time = time.time()
        logger.debug(
            "cur_time: [{}], touched_time: [{}]".format(
                cur_time, self.touched_time))        
        if self.touched_time > cur_time:
            # time skcrewed, refresh its time
            self.touched_time = cur_time
        elif (cur_time - self.touched_time) > timeout:
            logger.debug("session timeout({})".format(self.key))
            self.on_timeout()

    def on_timeout(self):
        pass

    def is_complete(self):
        return self.closed

    def close(self):
        self.closed = True;

    def event(self, msg):
        raise NotImplementedError()

class SessionCtrl():
    def __init__(self):
        self._ext_session_map = {}
        self._session_map = {}
        self._last_key = 0

    def check_timeout(self, timeout):
        session_tobe_deleted = []
        for key, session in self._session_map.items():

            session.check_timeout(timeout)
            if session.is_complete():
                session_tobe_deleted.append((key, session.ext_key))
        for key, ext_key in session_tobe_deleted:
            self._session_map.pop(key)
            self._ext_session_map.pop(ext_key)

    def find_by_external_key(self, key):
        if key in self._ext_session_map.keys():
            return self._ext_session_map[key]
        else:
            return None

    def find(self, key):
        if key in self._session_map.keys():
            return self._session_map[key]
        else:
            return None

    def remove_by_external_key(self, key):
        if key not in self._ext_session_map.keys():
            print("ext-key not in map")
            return
        session = self._ext_session_map.pop(key)
        if session.key not in self._session_map.keys():
            print("key not in map")
            return
        self._session_map.pop(session.key)

    def remove(self, key):
        if key not in self._session_map.keys():
            print("key not in map")
            return
        session = self._session_map.pop(key)
        if session.ext_key not in self._ext_session_map.keys():
            print("ext-key not in map")
            return
        self._ext_session_map.pop(session.ext_key)

    def add(self, key, ext_key, session):
        self._session_map[key] = session
        self._ext_session_map[ext_key] = session

    def gen_key(self):
        if self._last_key >= 0xFFFFFF:
            self._last_key = 0
        self._last_key = self._last_key + 1
        return self._last_key

