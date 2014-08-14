#!/usr/bin/env python

import logging

import gevent
from gevent.server import StreamServer

class EchoServer(StreamServer):
    def handle(self, socket, address):
        while True:
            data = socket.recv(1024)
            if not data:
                logging.error("socket broken")
                break
            socket.send(data)

def routine():
    while True:
        gevent.sleep(1)

def main():
    echo_server = EchoServer(("0.0.0.0", 8383))
    echo_server.start()
    gevent.joinall([
            gevent.spawn(routine)
            ])

if __name__ == "__main__":
    main()

