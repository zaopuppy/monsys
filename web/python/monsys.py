#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os.path

import torndb
import tornado.ioloop
import tornado.web
import tornado.httpserver
import tornado.gen
import tornado.concurrent
import tornado.iostream

import concurrent.futures

from tornado.options import define, options

define("port", default = 8888, help = "run on the given port", type = int)
define("mysql_host", default = "192.168.2.105:3306", help = "database host")
define("mysql_database", default = "monsys_db", help = "database name")
define("mysql_user", default = "monsys", help = "database user")
define("mysql_password", default = "monsys", help = "database password")

# home-page --> login --> fgw-list --> dev-list --> dev
#                        |<--        secure        -->|
class Application(tornado.web.Application):
    def __init__(self):
        settings = {
            "debug": True,
            "template_path": os.path.join(
                os.path.dirname(__file__), "templates"),
            "static_path": os.path.join(
                os.path.dirname(__file__), "static"),
            "cookie_secret": "monsys",
            "login_url": "/login.html",
        }
        handlers = [
            (r"/login", LoginHandler),
            (r"/logout", LogoutHandler),
            (r"/fgw-list", FGWListHandler),
            (r"/dev-list", DevListHandler),
            (r"/dev", DevHandler),
            # for debugging only
            (r"/test", TestHandler),
            (r"/print", PrintHandler),
            (r"/(.*\.html)",
             tornado.web.StaticFileHandler,
             { "path": settings['static_path'] }),
        ]
        tornado.web.Application.__init__(self, handlers, **settings)

        # Have one global connection to the blog DB across all handlers
        self.db = torndb.Connection(
            host = options.mysql_host,
            database = options.mysql_database,
            user = options.mysql_user,
            password = options.mysql_password)



class BaseHandler(tornado.web.RequestHandler):
    def initialize(self):
        pass
    @property
    def db(self):
        return self.application.db
    def get_current_user(self):
        return self.get_secure_cookie("account")

class LoginHandler(BaseHandler):
    def post(self):
        account = self.get_argument("account", None)
        password = self.get_argument("password", None)
        if not account or not password:
            self.write("wrong input")
            return
        rv = self.db.get(
            "select * from account_info where account = %s and password = %s",
            account, password)
        if not rv:
            self.write("account and password art not match")
            return
        self.set_secure_cookie("account", account)
        self.redirect("/fgw-list")

class LogoutHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        self.clear_cookie("account")
        self.redirect(self.settings["login_url"])

class FGWListHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        name = tornado.escape.xhtml_escape(self.current_user)
        self.write("FGWListHandler: " + name)

class DevListHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        name = tornado.escape.xhtml_escape(self.current_user)
        self.write("DevListHandler: " + name)

class DevHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        name = tornado.escape.xhtml_escape(self.current_user)
        self.write("DevHandler: " + name)

class PrintHandler(BaseHandler):
    def get(self):
        self.write(("arguments: [{0}]<br/>" +
                    "files: [{1}]<br/>" +
                    "path: [{2}]<br/>" +
                    "headers: [{3}<br/>").format(
                self.request.arguments,
                self.request.files,
                self.request.path,
                self.request.headers))

class TestHandler(BaseHandler):
    @tornado.gen.coroutine
    def get(self):
        if not hasattr(self, "flag"):
            self.flag = "fafafa"
            self.write("don't has flag")
        else:
            self.write("has flag: [{}]".format(self.flag))

if __name__ == "__main__":
    tornado.options.parse_command_line()
    http_server = tornado.httpserver.HTTPServer(Application())
    http_server.listen(options.port)
    tornado.ioloop.IOLoop.instance().start()

