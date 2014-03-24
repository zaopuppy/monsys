#!/usr/bin/env python
# -*- coding: utf-8 -*-

class SessionCtrl():
    def __init__(self):
        self._ext_session_map = {}
        self._session_map = {}
        self._last_key = 0

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
        if session["key"] not in self._session_map.keys():
            print("key not in map")
            return
        self._session_map.pop(session["key"])

    def remove(self, key):
        if key not in self._session_map.keys():
            print("key not in map")
            return
        session = self._session_map.pop(key)
        if session["ext-key"] not in self._ext_session_map.keys():
            print("ext-key not in map")
            return
        self._ext_session_map.pop(session["ext-key"])

    def add(self, key, ext_key, session):
        self._session_map[key] = session
        self._ext_session_map[ext_key] = session

    def gen_key(self):
        if self._last_key >= 0xFFFFFF:
            self._last_key = 0
        self._last_key = self._last_key + 1
        return self._last_key

