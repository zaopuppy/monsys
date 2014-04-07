#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import signal
import log

import gevent
import gevent.monkey

from dispatcher import Dispatcher
from fgw_client import FGWClient
from core_wireless_control import CoreWirelessControl

# logger
logger = log.Log.get_logger(__name__)


# def routine():
#     while True:
#         gevent.sleep(1)

def main():
    logger.debug("Start")

    gevent.monkey.patch_all()

    # dispatcher
    dispatcher = Dispatcher()

    # fgw client
    fgw_client = FGWClient(("127.0.0.1", 1984), dispatcher)

    # serial
    wireless_ctrl = CoreWirelessControl("/dev/tty.usbserial-ftDX0P76", dispatcher)

    jobs = [
        gevent.spawn(fgw_client.run),
        gevent.spawn(wireless_ctrl.run),
    ]

    gevent.joinall(jobs)

if __name__ == "__main__":
    sys.exit(main())

