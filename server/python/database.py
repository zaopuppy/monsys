#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pymysql

class Database():
    @staticmethod
    def get_connection():
        return pymysql.connect(host='192.168.2.105',
                               port=3306,
                               user='monsys',
                               passwd='monsys',
                               db='monsys_db')

