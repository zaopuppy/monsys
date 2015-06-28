
使用macports安装的mysql5

启动mysql服务(Mac)

    sudo mysqld_safe5

停止mysql服务

    sudo mysqladmin5 shutdown

连接数据库

    mysql5 -h localhost -u root

查询表结构

    describe <table>

查询所有的用户

    describe mysql.user;
    select host, user from mysql.user;

查询授权

    show grants for 'root'@'localhost'
    show grants;
    show grants for current_user;
    show grants for current_user();

查询所有的权限(不是授权)

    show privileges;

一个奇怪的特性, 如果直接使用

    mysql -h localhost -u xxx
    mysql -u xxx

无论是什么用户都能直接login, 但是权限是错误的, 只能查询一些基本信息, 不带权限的数据库什么的. 只有下面这样干才能使用正常的权限:

    mysql -h 192.168.2.105 -u monsys -pmonsys

查询/设置连接超时

    mysql> show global variables like '%time%' ;

update it make it something like 28800

    mysql> SET GLOBAL wait_timeout = 28800;


无法远程连接到mysql, 错误号111(Connection Refused), 因为

    # /etc/mysql/my.cnf
    #
    # Instead of skip-networking the default is now to listen only on
    # localhost which is more compatible and is not less secure.
    bind-address            = 127.0.0.1


