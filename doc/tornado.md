连接数据库:
-------------------------------------------------
tornado.database已经被废弃了, 除非绝对必要, 否则不要再用了

    Warning The tornado.database module is deprecated and will be removed in Tornado 3.0. It is now available separately as the torndb module.

初始化参数:
    __init__(self, host, database, user=None, password=None, max_idle_time=25200)

用法:

    >>> import tornado.database
    >>> db = tornado.database.Connection(host = "192.168.2.105", database = 'monsys_db', user = 'monsys', password = 'monsys')
    >>> db.query("show tables")
    [{'Tables_in_monsys_db': u'account_info'}, {'Tables_in_monsys_db': u'fgw_list'}]

建议使用torndb:

    class torndb.Connection(host, database, user=None, password=None, max_idle_time=25200, connect_timeout=0, time_zone='+0:00')
    >>> import torndb
    >>> db = torndb.Connection(host = "192.168.2.105", database = 'monsys_db', user = 'monsys', password = 'monsys')
    >>> db.query("select * from fgw_list where account = %s", "ztest@gmail.com")
    [{'device': u'DEVID-Z', 'account': u'ztest@gmail.com', 'pubkey': u'ZHAOYIC'}]
    # query( "SELECT * FROM table WHERE name = %(name)s AND location = %(location)s", name="george", location="jungle" )

顺便说一句, 网上一些乱七八糟的教程只能提供你最最最最基本的使用, 基本上也就是用来测试模块是否能够工作, 对于接口的其他参数, 从来不说明, 坑爹啊...这些在性能和安全编码中还是很重要的

tornado.concurrent
-------------------------------------------------
注意: 这里仅仅是做技术上的可行性研究, 除非绝对必要, 否则不要自行启动额外的线程或者进程, 所有的处理都应该以异步的方式进行

直接上代码

    class TestHandler(BaseHandler):
        executor = concurrent.futures.ThreadPoolExecutor(2)
        @tornado.gen.coroutine
        def get(self):
            # result = yield tornado.gen.Task(long_time_opera)
            result = yield self.long_time_opera()
            self.write("result: [{}]".format(result))
        @tornado.concurrent.run_on_executor
        def long_time_opera(self):
            import time
            time.sleep(10)
            return "long_time_opera"

主要注意几点:

* executor必须存在self中, 且必须是一个Executor, 应该是提供某些方法即可, 未追究
* 阻塞方法必须在self中
* ThreadPoolExecutor(或者其他Executor)并发的个数决定了不相互影响的请求个数, 此例中, 同时只能异步处理的请求个数, 超过的会在队列中, 等待之前的执行完毕. 但是都不影响其他请求的处理

参考:
http://stackoverflow.com/questions/16860829/tornado-thread-does-not-start-using-run-on-executor-in-coroutine
http://www.dongwm.com/archives/shi-yong-tornadorang-ni-de-qing-qiu-yi-bu-fei-zu-sai/

Tornado 中实现异步的非HTTP协议
-------------------------------------------------
一个残酷的现实是 Tornado 只提供了HTTP协议的异步接口, 私有协议必须要自己写. 我还挺不相信的搜了好几天...敢情都没人干过用其他协议向后端发送请求的么?

这么想着我挺崩溃的, 不过我才不要因为没有现成库去改成HTTP协议呢, 还好TCP的异步接口已经提供了, 自己写一个就好了, 不然从异步socket开始封装得麻烦死.



需要注意一点, 调用tornado.gen.Task提供的回调时, 提供的参数会立即提供给调用方, 然后才会继续执行下一步

    # 这是错误的示例
    def on_read(self, data):
        print("on_read({})".format(data))
        if self.callback:
            self.callback(data)
            # data已经返回给了调用方, 如果调用方又调用了其他异步接口, 那么下面的语句会导致tornado.gen.Task提供的callback被清除
            self.callback = None

正确的做法应该是:

    def on_read(self, data):
        print("on_read({})".format(data))
        if self.callback:
            callback = self.callback
            self.callback = None
            callback(data)


参考:
http://lbolla.info/blog/2012/10/03/asynchronous-programming-with-tornado

Tornado 记录运行日志
-------------------------------------------------
python monsys.py --log\_file\_prefix=log/access.log --log\_to\_stderr=true



