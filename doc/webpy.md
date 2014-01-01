
连接数据库(mysql)

    >>> import web.db
    >>> db = web.database(dbn = 'mysql', db = 'monsys_db', host = '192.168.2.105', port = 3306, user = 'monsys', pw = 'monsys')
    >>> db.query("select * from fgw_list")
    0.0 (1): select * from fgw_list
    <web.utils.IterBetter instance at 0x5be2d8>
    >>>



