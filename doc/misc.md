tornado和gevent哪一个更适合用来做聊天服务器？
-------------------------------------------------
http://www.zhihu.com/question/21419487
郭煜
eventlet——无它，能在pypy上跑的支持greenlet的io框架
gevent——其次的选择，在CPython上性能不错，聊天逻辑也好实现
twisted——如果有一定设计经验，其实它应该排老二甚至老一，设计好的程序可以不用借助greenlet就能比较完美地体现业务逻辑，同样支持pypy
tornado——应该用在它专注的领域，它的核心设计其实和twisted差不多，只是有些组件的设计思路不同，但twisted有更多现成的工具可以干这个，起码现在来说tornado应该用在web服务上。


