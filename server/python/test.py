#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pymysql
import gevent

def test_pymysql():
    conn = pymysql.connect(host='192.168.2.105', port=3306, user='monsys', passwd='monsys', db='monsys_db')
    cur = conn.cursor()
    cur.execute("SELECT sleep(3)")
    print(cur.description)
    print()
    for row in cur:
        print(row)
    cur.close()
    conn.close()

def routine():
    timeout = gevent.Timeout(1)
    timeout.start()
    try:
        while True:
            print("routine()")
            gevent.sleep(1)
    except gevent.Timeout as e:
        print("timeout~")
    finally:
        timeout.cancel()

def main():
    # test_pymysql()
    gevent.joinall([
            gevent.spawn(routine)
            ])

if __name__ == "__main__":
    main()
