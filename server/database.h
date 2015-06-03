#ifndef _Z_DATABASE_H__
#define _Z_DATABASE_H__

#include <assert.h>

#include <my_global.h>
#include <mysql.h>
#include <errmsg.h>


enum {
  SQL_IDX_INVALID = -1,
  SQL_IDX_CHECK_DEVICE,
};

typedef struct {
  const char *sql;
  int param_num;
} sql_data_t;

// TODO: use prepared statement instead of hard coded sql
const sql_data_t SQLS[] = {
  { "SELECT count(1) from `fgw_list` where `device` = '%s' and `pubkey` = '%s'",
    2 },
};


class MySQLDatabase;

class Database {
public:
  Database() {}
  virtual ~Database() {}

public:
  virtual int init() = 0;
  virtual int connect() = 0;
  virtual int onError() = 0;

};

/*
Basically, you call mysql_store_result() or mysql_use_result() to access
the result set, the former loads all the rows into memory on the client side,
the latter accesses rows one at a time from the server. If you use
mysql_use_result(), you need to call mysql_fetch_row() to access each row
until the function returns NULL. Each successful call to mysql_fetch_row()
will return a MYSQL_ROW which you can use to access the individual field
values.
*/

class MySQLDatabase : public Database {
public:
  static MySQLDatabase* instance() {
    static MySQLDatabase instance;
    return &instance;
  }

public:
  virtual ~MySQLDatabase() {
    if (conn_) {
      mysql_close(conn_);
      conn_ = NULL;
    }
  }

  virtual int init() {
    Z_LOG_D("MySQLDatabase::init()");

    assert(conn_ == NULL);
    conn_ = mysql_init(NULL);

    if (conn_ == NULL) {
      Z_LOG_D("Failed to initialized MySQL database");
      return -1;
    }

    Z_LOG_D("MySQL database initialized");

    return 0;
  }

  virtual int connect() {
    const char *db_host = "192.168.2.105";
    const char *db_name = "monsys_db";
    unsigned int db_port = 3306;
    const char *db_user = "monsys";
    const char *db_passwd = "monsys";

    Z_LOG_D("MySQLDatabase::connect()");

    do {

      MYSQL *rv;

      rv = mysql_real_connect(
        conn_, db_host, db_user, db_passwd, db_name, db_port, NULL, 0);
      if (rv == NULL) {
        break;
      }

      /**
       * Please note that mysql_autocommit() must be called AFTER
       * mysql_connect(). When called before mysql_connect,
       * mysql_autocommit() may cause a segmentation violation. If you are
       * having a problem with segmentation faults, check your order of
       * operations. Cheers.
       */
      if (mysql_autocommit(conn_, 1) != 0) {
        break;
      }
  
      Z_LOG_D("MySQL database connected");

      return 0;
  
    } while (false);

    onError();

    Z_LOG_D("Failed to connect to MySQL database");

    return -1;
  }

  virtual int onError() {
    if (conn_) {
      printf("Failed to connect to database: [%u:%s]\n",
        mysql_errno(conn_), mysql_error(conn_));
    }

    return 0;
  }

  // FIXME: too ugly...
  MYSQL_RES* query(const char *sql) {
    int rv = mysql_query(conn_, sql);
    unsigned int err = 0;
    if (rv != 0 ) {
      err = mysql_errno(conn_);
      if (err != CR_SERVER_GONE_ERROR && err != CR_SERVER_LOST) {
        Z_LOG_E("exception while query database: %d, %u", rv, err);
        onError();
        return NULL;
      }
    }

    if (err == CR_SERVER_GONE_ERROR || err == CR_SERVER_LOST) {
      Z_LOG_E("database connection lost, try to re-connect");
      rv = connect();
      if (rv != 0) {
        Z_LOG_E("failed to re-connect to database: %d, %u", rv, err);
        onError();
        return NULL;
      }

      rv = mysql_query(conn_, sql);
      if (rv != 0) {
        Z_LOG_E("failed to query database again, fail");
        onError();
        return NULL;
      }
    }

    // FIXME: take care, this will query and save all result from server
    // don't do this if lots of result will be produced
    MYSQL_RES *result = mysql_store_result(conn_);
    if (result == NULL) {
      onError();
      return NULL;
    }

    return result;
  }

private:
  MYSQL *conn_;
};


#endif
