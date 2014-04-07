#!/usr/bin/env python
# -*- coding: utf-8 -*-

import log

# logger
logger = log.Log.get_logger(__name__)

class FGWManager():
    def __init__(self):
        self._fgw_map = {}

    def find_handler(self, fgw_id):
        if fgw_id in self._fgw_map.keys():
            return self._fgw_map[fgw_id]
        else:
            return None

    def add_handler(self, fgw_id, handler_id):
        self._fgw_map[fgw_id] = handler_id

    def remove_handler(self, fgw_id):
        if fgw_id in self._fgw_map.keys():
            self._fgw_map.pop(fgw_id)
        else:
            raise Exception("fgw id({}) is not here".format(fgw_id))

