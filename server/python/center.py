#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import signal
import logging

import gevent
import gevent.monkey

# from simple_server import SimpleServer, SimpleHandler
from webapi_server import WebApiServer
from fgw_server import FGWServer
from dispatcher import Dispatcher

# logging handler
logger_handler = logging.StreamHandler()
logger_handler.setLevel(logging.DEBUG)

# logger
logger = logging.getLogger("main")
logger.setLevel(logging.DEBUG)
logger.addHandler(logger_handler)

def routine():
    while True:
        gevent.sleep(1)

def main():
    logger.debug("Start")

    gevent.monkey.patch_all()

    dispatcher = Dispatcher()

    webapi_server = WebApiServer(("0.0.0.0", 1983), dispatcher)
    webapi_server.start()

    fgw_server = FGWServer(("0.0.0.0", 1984), dispatcher)
    fgw_server.start()

    jobs = [
        gevent.spawn(routine)
    ]

    gevent.joinall(jobs)

if __name__ == "__main__":
    sys.exit(main())

