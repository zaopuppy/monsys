#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import sys
import select

import gevent
import gevent.queue
import gevent.monkey
import pymysql

conn = pymysql.connect(host='192.168.2.105',
                       port=3306,
                       user='monsys',
                       passwd='monsys',
                       db='monsys_db')

def test(*args):
    print("test")
    pass

def query(*args):
    # conn = pymysql.connect(host='192.168.2.105',
    #                        port=3306,
    #                        user='monsys',
    #                        passwd='monsys',
    #                        db='monsys_db')
    sql = """SELECT * from `account_info"""
    cur = conn.cursor()
    gevent.joinall([ gevent.spawn(cur.execute, sql) ])
    rows = cur.fetchall()
    cur.close()

    for row in rows:
        print(row)

cmd_map = {
    "test": test,
    "query": query
    }

def loop(queue):
    while True:
        cmd = queue.get()
        if not cmd:
            print("Null command")
            break
        print("received: [{}]".format(cmd))
        func = cmd_map.get(cmd[0], lambda *args: print("bad command"))
        func(*cmd[1])

def get_console_input(prompt):
    # can't make it async
    # return raw_input(prompt)
    sys.stdout.write(prompt)
    sys.stdout.flush()
    select.select([sys.stdin], [], [])
    return sys.stdin.readline()

def read_line(queue):
    while True:
        line = get_console_input("> ").strip()
        if line is None or len(line) <= 0:
            continue
        print("line: [{}]".format(line))
        fields = line.strip().split()
        queue.put((fields[0], fields[1:]))
        gevent.sleep(0)

def routine():
    while True:
        gevent.sleep(1)

def main():
    print("start")
    gevent.monkey.patch_all()
    queue = gevent.queue.Queue()
    gevent.joinall([
            # gevent.spawn(routine),
            gevent.spawn(loop, queue),
            gevent.spawn(read_line, queue),
            ])

if __name__ == "__main__":
    main()

