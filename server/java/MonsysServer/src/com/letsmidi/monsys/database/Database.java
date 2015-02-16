package com.letsmidi.monsys.database;

import javax.sql.DataSource;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import com.mysql.jdbc.jdbc2.optional.MysqlDataSource;


/**
 * JDBC
 * JNDI
 *
 * @author zero
 */
public class Database {
    public static final Database INSTANCE = new Database();

    // private static final String DB_URL = "jdbc:mysql://localhost:3306/sakila?profileSQL=true";
    private static final String DB_URL = "jdbc:mysql://192.168.2.105:3306/monsys_db";
    private static final String DB_USER = "monsys";
    private static final String DB_PWD = "monsys";

    private DataSource mDataSource = null;

    static {
        try {
            Class.forName("com.mysql.jdbc.Driver").newInstance();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    private Database() {
        mDataSource = getDataSource();
    }

    private DataSource getDataSource() {
        MysqlDataSource datasource = new MysqlDataSource();

        datasource.setUrl(DB_URL);
        datasource.setUser(DB_USER);
        datasource.setPassword(DB_PWD);

        return datasource;
    }

    public Connection getConnection() {
        try {
            return mDataSource.getConnection();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return null;
    }

    private void execute(String sql) {
        Connection conn = Database.INSTANCE.getConnection();
        Statement stmt = null;
        ResultSet result = null;
        try {
            stmt = conn.createStatement();
            result = stmt.executeQuery("select * from account_info");
            while (result.next()) {
                System.out.println(result.getString("account"));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            if (result != null) {
                try {
                    result.close();
                } catch (SQLException e) {
                }
            }
            if (stmt != null) {
                try {
                    stmt.close();
                } catch (SQLException e) {
                }
            }
            if (conn != null) {
                try {
                    conn.close();
                } catch (SQLException e) {
                }
            }
        }
    }

    public static void main(String[] args) {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        String line;
        try {
            for (line = in.readLine().trim();
                 line != null && !line.equals("exit");
                 line = in.readLine().trim()) {
                Database.INSTANCE.execute("select * from account_info");
            }
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}

