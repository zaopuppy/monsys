#!/usr/bin/env python
# -*- coding: utf-8 -*-

import web

urls = (
    '/', 'Index',
    '/login', 'Login',
)

web.config.debug = True
g_app = web.application(urls, globals())
g_db = web.database(
    dbn = 'mysql',
    db = 'monsys_db',
    host = '192.168.2.105',
    port = 3306,
    user = 'monsys',
    pw = 'monsys')
if web.config.get('_session') is None:
    # session.get('logged_in', False)
    g_session = web.session.Session(app, web.session.DiskStore('sessions'), {'account': None, 'logged_in': False})
    web.config._session = session
else:
    g_session = web.config._session

# db = web.database(dbn = 'mysql', db = 'monsys_db', host = '192.168.2.105', port = 3306, user = 'monsys', pw = 'monsys')

class Index:
    def GET(self):
        if 
        return "Hello, world!"

class Login:
    def POST(self):
        in_data = web.input(account = None, password = None)
        rv = g_db.where(
            'account_info',
            account = in_data.account,
            password = in_data.password)
        if len(rv) > 0:
            session.logged_in = True
            session.account = in_data.account
            raise web.seeother('/')
        else:
            return "bad:("

if __name__ == "__main__":
    app.run()


