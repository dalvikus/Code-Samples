package com.dn2soft.db;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class SQLiteJDBC
{
    Connection  mConn = null;
    public Connection getConnection() {return mConn;}

    public SQLiteJDBC(String dbpath)
    throws ClassNotFoundException, SQLException
    {
        if (dbpath == null) {
            throw new IllegalArgumentException("dbpath is null");
        }
        String  urlString = "jdbc:sqlite:" + dbpath;
        Class.forName("org.sqlite.JDBC");   // ClassNotFoundException
        mConn = DriverManager.getConnection(urlString); // SQLException
    }
}
