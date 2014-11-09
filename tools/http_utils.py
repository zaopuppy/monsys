#!/usr/bin/env python

import httplib


def post(host, port, url, body=None, headers={}, timeout=None):
    conn = httplib.HTTPConnection(host, port, timeout=timeout)
    conn.request("POST", url, body, headers)
    rsp = conn.getresponse()
    print("status: [{}|{}]".format(rsp.status, rsp.reason))
    rsp_data = rsp.read()
    print("received: " + rsp_data)


def main():
    post("localhost", 8088, "/")


if __name__ == "__main__":
    main()

