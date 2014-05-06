#!/usr/bin/env python
# -*- coding: utf-8 -*-

TYPE_UNKNOWN    = -1
TYPE_INNER      = 1
TYPE_JSON       = 2

class Msg(object):
    """docstring for Msg"""
    def __init__(self, src_addr, dst_addr, data):
        super(Msg, self).__init__()
        # self.type = type
        self.src_addr = src_addr
        self.dst_addr = dst_addr
        self.data = data

