// -I/opt/local/include/mysql5
// -L/opt/local/lib/mysql5/mysql -lmysqlclient
#include <stdio.h>
#include <assert.h>

#include <my_global.h>
#include <mysql.h>


class Database {
public:
  Database() {}
  virtual ~Database() {}

public:
  virtual int init() = 0;
  virtual int connect() = 0;
  virtual int onError() = 0;
};

class MySQLDatabase : public Database {
public:
  virtual ~MySQLDatabase() {
    if (conn_) {
      mysql_close(conn_);
      conn_ = NULL;
    }
  }

  virtual int init() {
    assert(conn_ == NULL);
    conn_ = mysql_init(NULL);
    assert(conn_ != NULL);

    if (!mysql_autocommit(conn_, true)) {
      onError();
      return -1;
    }

    return 0;
  }

  virtual int connect() {
    do {

      MYSQL *rv;

      rv = mysql_real_connect(conn_, "localhost", "root", "", "monsys_db", 0, NULL, 0);
      if (rv == NULL) {
        break;
      }

      return 0;
    } while (false);

    onError();

    return -1;
  }

  virtual int onError() {
    if (conn_) {
      printf("Failed to connect to database: [%s]\n", mysql_error(conn_));
    }

    return 0;
  }

  MYSQL_RES* query(const char *sql) {
    int rv = mysql_query(conn_, sql);
    if (rv != 0) {
      onError();
      return NULL;
    }

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

int main(int argc, char *argv[])
{
  printf("MySQL client version: %s\n",
         mysql_get_client_info());

  MySQLDatabase db;

  assert(db.init() == 0);
  assert(db.connect() == 0);

  MYSQL_RES *result = db.query("select * from fgw_list");
  if (result == NULL) {
    return -1;
  }

  int row_num = mysql_num_rows(result);
  printf("returned %d rows\n", row_num);

  int field_num = mysql_num_fields(result);
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    for (int i = 0; i < field_num; ++i) {
      printf("%s ", row[i] ? row[i] : "NULL");
    }
    printf("\n");
  }

  mysql_free_result(result);

	return 0;
}

