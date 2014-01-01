
yield
=================================================

在正式开讲之前我必须说一下为什么我会想研究python yield

Tornado是一个基于python的异步WEB框架, 这里的异步是指像Javascript一样, 所有的调用都(需要)是非阻塞的, 实际上我们可以想象一个C程序, 调用select轮流查询端口, 依次调用对应的处理函数. 所以和其他的框架不一样, 如果主线程中的某个调用被阻塞, 会导致整个服务全部停止.

用C代码来描述的话, 差不多是这样的:

    while (1) {
        for (int i = 0; i < num_of_routine; ++i) {
            routines[i]->process();
        }
    }

那么如果实际的应用中要做耗时操作怎么办? 比如访问网络, 又比如读取文件, 调用可能会占用很长的实际, 如果同步等待的话, 整个服务都会停止. 一般的想法是实现一个状态机, 状态变迁为: init --> wait_for_read --> read_over. 但是, 如果有语言级别coroutine的支持的话, 可以有更方便的做法

(TODO: 补充coroutine的使用)

Tornado提供了tornado.gen接口, 用于提供coroutine功能.

~~yield干嘛用的我就不说了, 网上大把资料, 这里说说我耗费了几个小时学习的结果.~~

长期以来我以为yield只是一个普通的statement, 类似return, 用来把函数变成generator. 仅此而已. 直到最近使用Tornado框架做网页的时候, 才发现原来还可以用作expression.

python 2.5 (2005)对yield进行过一次修订, 将yield从原来的statement, 变成了expression. 意味着, yield可以产生值. 比如:

    v = yield 4


这一行会让当前生成的generator.next()直接返回4(别忘了使用yield之后, 函数的返回值是一个generator), 然后等待其他人调用generator.send(value), value将直接赋值给v. 如果在下次调用generator.next()之前没有任何人send值, 那么v的取值为None.

这个变化看起来没啥意义? 如果我们只是同步编程, 比如说算法什么的, 确实没有太大必要. 但是世界是异步的.

回到刚刚的例子, 由于来自网络(或者磁盘IO)的响应通常无法预知何时能够返回, 所以我们通常只能使用状态机来完成, 如果使用yield会怎样? 太简单了, 简直和同步编程没有区别:

    def get(self):
        response = (yield do_network_request())
        return self.write("response: {}".format(response))

唯一的区别就是使用了yield代替了直接的函数调用(当然需要注意do_network_request()必须是异步的, 否则整个过程依然会卡住), get方法被调用后, 产生一个generator. 而do_network_request完成操作后, 只需要调用generator.send(rsp), 就能把响应赋值给response, get方法又可以直接从原来中断的地方继续执行下去了. 原理上说, 和状态机没有本质区别, 但是写作上, 简单实在太多了, 要知道, 这只是一个最最简单的例子, 实际环境会有更复杂, 异步嵌套异步的场景

~~我原来理解的yield已经很接近coroutine的概念了, 问题是, 还不完美~~



    def p(v):
        print("P: {}".format(v))
    def p0():
        p((yield))
    def p1():
        p((yield 43))
    def px():
        v = yield
        print("v: {}".format(v))


    >>> g = t.p0()
    >>> g.next()
    >>> g.send(5)
    P: 5
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
    StopIteration
    >>> g = t.p1()
    >>> g.next()
    43
    >>> g.send(3)
    P: 3
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
    StopIteration
    >>> g = t.px()
    >>> g.next()
    >>> g.send(4)
    v: 4
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
    StopIteration

几个比较细微的点
* 无论函数是否执行yield, 只要语句中包含yield, 改函数的返回就是一个generator
* 

参考
-------------------------------------------------
http://www.tornadoweb.org/en/stable/gen.html
http://stackoverflow.com/questions/231767/the-python-yield-keyword-explained
http://www.python.org/dev/peps/pep-0342/

