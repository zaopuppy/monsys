#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging

from logging import StreamHandler


class StdoutHandler(StreamHandler):
    def __init__(self):
        StreamHandler.__init__(self)
        StreamHandler.setLevel(self, logging.DEBUG)
        StreamHandler.setFormatter(self,
            logging.Formatter('%(asctime)s|%(filename)s:%(lineno)d|%(message)s'))

class Log():
    _stdout_handler = StdoutHandler()
    @staticmethod
    def get_logger(name):
        logger = logging.getLogger(name)
        if Log._stdout_handler not in logger.handlers:
            # print("No handler")
            logger.addHandler(Log._stdout_handler)
            logger.setLevel(logging.DEBUG)
        return logger

