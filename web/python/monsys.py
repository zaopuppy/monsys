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

from tornado.options import define, options

import json

import tcpclient

define("port", default = 8888, help = "run on the given port", type = int)
define("mysql_host", default = "192.168.2.105:3306", help = "database host")
define("mysql_database", default = "monsys_db", help = "database name")
define("mysql_user", default = "monsys", help = "database user")
define("mysql_password", default = "monsys", help = "database password")
define("center_host", default = "192.168.2.105", help = "center host")
define("center_port", default = 1983, help = "center host", type = int)

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
            "login_url": "/",
        }
        handlers = [
            (r"/login", LoginHandler),
            (r"/logout", LogoutHandler),
            (r"/interface", InterfaceHandler),
            (r"/interface/login", InterfaceLoginHandler),
            # for debugging only
            (r"/", HomeHandler),
            (r"/test", TestHandler),
            (r"/print", PrintHandler),
            (r"/(.*\..*)",
             tornado.web.StaticFileHandler,
             { "path": settings['static_path'] }),
        ]
        tornado.web.Application.__init__(self, handlers, **settings)

        # Have one global connection to the blog DB across all handlers
        self.db = torndb.Connection(
            host     = options.mysql_host,
            database = options.mysql_database,
            user     = options.mysql_user,
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
        # authentication
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

        # save user name
        self.set_secure_cookie("account", account)

        # save fgw-list for later use
        fgw_list = rv.get("fgw_list").strip() or ""
        # fgw_list = list(filter(lambda x: True if x else False, fgw_list.split("|")))
        # print("fgw_list: [{}]".format(fgw_list))
        self.set_secure_cookie("fgw-list", fgw_list)

        # TODO: redirect to last page
        self.redirect("/")

class LogoutHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        # self.clear_cookie("account")
        self.clear_all_cookies()
        self.redirect(self.settings["login_url"])

class InterfaceHandler(BaseHandler):
    @tornado.web.authenticated
    @tornado.gen.coroutine
    def post(self):
        if not self.request.body:
            self.set_status(404)
            self.write("bad request")
            return
        # check length
        if len(self.request.body) > 1024*2:
            self.set_status(404)
            self.write("request is too long")
            return
        client = tcpclient.TCPClient()
        connected = yield tornado.gen.Task(
            client.connect, (options.center_host, options.center_port))
        if not connected:
            self.set_status(808, "Blahblah")
            self.write("backend is not available")
            return
        client.write(self.request.body)
        rsp = yield tornado.gen.Task(client.read)
        self.write("{}".format(rsp))

# --- for debugging only ---
class HomeHandler(BaseHandler):
    def get(self):
        self.render("index.html")

class InterfaceHandler(BaseHandler):
    @tornado.gen.coroutine
    def post(self):
        if not self.request.body:
            self.write("bad request")
            return
        client = tcpclient.TCPClient()
        connected = yield tornado.gen.Task(
            client.connect, (options.center_host, options.center_port))
        if not connected:
            self.write("backend is not available")
            return
        client.write(self.request.body)
        rsp = yield tornado.gen.Task(client.read)
        self.write("{}".format(rsp))

class InterfaceLoginHandler(BaseHandler):
    def post(self):
        # authentication
        account = self.get_argument("account", None)
        password = self.get_argument("password", None)
        if not account or not password:
            self.set_status(404)
            self.write("wrong input")
            return
        rv = self.db.get(
            "select * from account_info where account = %s and password = %s",
            account, password)
        if not rv:
            self.set_status(404)
            self.write("account and password art not match")
            return
        # save user name
        self.set_secure_cookie("account", account)
        # save fgw-list for later use
        fgw_list = rv.get("fgw_list").strip() or ""
        self.set_secure_cookie("fgw-list", fgw_list)

# --- for debugging only ---
class HomeHandler(BaseHandler):
    def get(self):
        self.render("index.html")

class InterfaceHandler(BaseHandler):
    @tornado.gen.coroutine
    def post(self):
        if not self.request.body:
            self.write("bad request")
            return
        client = tcpclient.TCPClient()
        connected = yield tornado.gen.Task(
            client.connect, (options.center_host, options.center_port))
        if not connected:
            self.write("backend is not available")
            return
        client.write(self.request.body)
        rsp = yield tornado.gen.Task(client.read)
        self.write("{}".format(rsp))

# --- for debugging only ---
class HomeHandler(BaseHandler):
    def get(self):
        self.render("index.html")

class PrintHandler(BaseHandler):
    def get(self):
        self.write(("arguments: [{}]<br/>" +
                    "files: [{}]<br/>" +
                    "path: [{}]<br/>" +
                    "headers: [{}]<br/>" +
                    "uri: [{}]<br/>").format(
                self.request.arguments,
                self.request.files,
                self.request.path,
                self.request.headers,
                self.request.uri))

class TestHandler(BaseHandler):
    @tornado.gen.coroutine
    def get(self):
        client = tcpclient.TCPClient()
        connected = yield tornado.gen.Task(client.connect, ("192.168.2.146", 1983))
        if not connected:
            self.write("Failed to connect to backend server, man")
            return
        client.write(json.dumps({ "cmd": "get-dev-list", "uid": "uid" }))
        rsp = yield tornado.gen.Task(client.read)
        self.write("Jesus, we got a response: [{}]".format(rsp))

if __name__ == "__main__":
    tornado.options.parse_command_line()
    http_server = tornado.httpserver.HTTPServer(Application())
    http_server.listen(options.port)
    tornado.ioloop.IOLoop.instance().start()

