#!/usr/bin/env python
# -*- coding: utf-8 -*-

import tornado
import tornado.ioloop
import tornado.iostream
import tornado.gen

import socket
import time

class TCPClient:
    """
    """
    def connect(self, server_info_pair, callback = None, timeout = 3):
        self.server_info_pair = server_info_pair
        self.callback = callback

        print("begin to connect to: [{}]".format(self.server_info_pair))

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        self.stream = tornado.iostream.IOStream(s)
        self.stream.set_close_callback(self.on_close)
        self.stream.connect(self.server_info_pair, self.on_connect)
        tornado.ioloop.IOLoop.instance().add_timeout(
            time.time() + timeout,
            self.on_timeout)

    def write(self, data):
        self.stream.write(data)
        print("sent")

    def read(self, callback = None):
        self.callback = callback
        self.stream.read_bytes(5*1024, callback = None, streaming_callback = self.on_read)
        print("waiting data...")

    def on_connect(self):
        print("on_connect()")
        if self.callback:
            callback = self.callback
            self.callback = None
            callback(True)

    def on_read(self, data):
        print("on_read({})".format(data))
        if self.callback:
            callback = self.callback
            self.callback = None
            callback(data)

    def on_timeout(self):
        print("on_timeout()")

    def close(self):
        if not self.stream.closed():
            self.stream.close()

    def on_close(self, data = None):
        print("on_close({})".format(data))
        if self.callback:
            callback = self.callback
            self.callback = None
            callback(data)

@tornado.gen.coroutine
def echo_test():
    client = TCPClient()
    connected = yield tornado.gen.Task(client.connect, ("192.168.2.146", 4444))
    if not connected:
        print("Failed to connect, quit")
        return
    client.write(b"echo_test()")
    rsp = yield tornado.gen.Task(client.read)
    print("rsp: [{}]".format(rsp))
    client.close()

# testing code
if __name__ == "__main__":
    tornado.ioloop.IOLoop.instance().add_callback(echo_test)
    tornado.ioloop.IOLoop.instance().start()

