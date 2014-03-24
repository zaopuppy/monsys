#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pymysql

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

def main():
    test_pymysql()

if __name__ == "__main__":
    main()
