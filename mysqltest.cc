// -I/opt/local/include/mysql5
// -L/opt/local/lib/mysql5/mysql -lmysqlclient
#include <stdio.h>
#include <assert.h>
#include <string.h>

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

    return 0;
  }

  virtual int connect() {
    do {

      MYSQL *rv;

      rv = mysql_real_connect(conn_, "localhost", "root", "", "monsys_db", 0, NULL, 0);
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

		// this will cause will all rows be stored in client, instead of
		// fetching from server one by one, take care
    MYSQL_RES *result = mysql_store_result(conn_);
    if (result == NULL) {
      onError();
      return NULL;
    }

    return result;
  }

  void query() {
		const char *sql = "select count(1) from fgw_list where device = ? and pubkey = ?";
		
		MYSQL_BIND param[2];

		do {
			// allocate
			MYSQL_STMT *stmt = mysql_stmt_init(conn_);
			if (stmt == NULL) {
				printf("Failed to allocate statement\n");
				break;
			}

			// init
			memset(param, 0x00, sizeof(param));
			int rv = mysql_stmt_prepare(stmt, sql, strlen(sql));
			if (rv != 0) {
				printf("Failed to prepare statement\n");
				break;
			}

			{
				// 0
				const char *devid = "DEVID-Z";
				unsigned long devid_len = strlen(devid);
				param[0].buffer_type = MYSQL_TYPE_VARCHAR;
				param[0].buffer = (void*)devid;
				param[0].is_unsigned = 0;
				param[0].is_null = 0;
				param[0].length = &devid_len;

				// 1
				const char *pubkey = "ZHAOYIC";
				unsigned long pubkey_len = strlen(pubkey);
				param[1].buffer_type = MYSQL_TYPE_VARCHAR;
				param[1].buffer = (void*)pubkey;
				param[1].is_unsigned = 0;
				param[1].is_null = 0;
				param[1].length = &pubkey_len;
			}

			rv = mysql_stmt_bind_param(stmt, param);
			if (rv != 0) {
				printf("Failed to bind param for statement\n");
				break;
			}

      int32_t row_num;
      {
        unsigned long row_num_len;
  			MYSQL_BIND result[1];
  			result[0].buffer_type = MYSQL_TYPE_LONG;
        result[0].buffer = (void*)&row_num;
        result[0].buffer_length = sizeof(row_num);
        result[0].is_null = 0;
        result[0].length = &row_num_len;

  			rv = mysql_stmt_bind_result(stmt, result);
        if (rv != 0) {
          printf("Failed to bind result\r");
          break;
        }
      }

			rv = mysql_stmt_execute(stmt);
			if (rv != 0) {
				printf("Failed to execute statement\n");
				break;
			}

			// MYSQL_RES *result = mysql_stmt_store_result(stmt);

      rv = mysql_stmt_fetch(stmt);
      if (rv != 0) {
        printf("Failed to fetch result\n");
      } else {
        printf("count(1) = %d\n", row_num);
      }

			mysql_stmt_free_result(stmt);
      mysql_stmt_close(stmt);

      return;

		} while (false);

		onError();
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

  // MYSQL_RES *result = db.query("select * from fgw_list");
  MYSQL_RES *result = db.query("select count(1) from fgw_list");
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

  db.query();

	return 0;
}

