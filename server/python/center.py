#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import signal

import gevent
import gevent.monkey

from webapi_server import WebApiServer
from fgw_server import FGWServer

from env import Env
from dispatcher import Dispatcher
from fgw_manager import FGWManager

import log

# logger
logger = log.Log.get_logger(__name__)


def webapi_server(msg):
    if msg.type is MSG_INNER:
        pass
    elif msg.type is MSG_OUTER:
        pass
    else:
        pass

def routine():
    while True:
        gevent.sleep(1)

def main():
    logger.debug("Start")

    gevent.monkey.patch_all()

    env = Env()

    env.dispatcher = Dispatcher()
    env.fgw_manager = FGWManager()

    webapi_server = WebApiServer(("0.0.0.0", 1983), env)
    webapi_server.start()

    fgw_server = FGWServer(("0.0.0.0", 1984), env)
    fgw_server.start()

    jobs = [
        gevent.spawn(routine)
    ]

    gevent.joinall(jobs)

if __name__ == "__main__":
    sys.exit(main())

