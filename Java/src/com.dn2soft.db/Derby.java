package com.dn2soft.db;

import java.sql.DriverManager;
import java.sql.Connection;
import java.util.Properties;
import java.sql.SQLException;
import java.sql.Statement;

/**
 *
 * java -Dij.outfile=y -Djdbc.drivers=org.apache.derby.jdbc.EmbeddedDriver org.apache.derby.tools.ij < x
 * x:
-- start of x
connect 'jdbc:derby:/home/icentrik/db/Derby/Cambridge';

SELECT * FROM Vocabulary;
-- end of x

 *
From ij:
connect 'jdbc:derby:/home/icentrik/db/Derby/Cambridge';
 */
public class Derby {
    Connection  conn;
    public Connection getConnection() {return conn;}
    public void createTables()
    throws SQLException
    {
        // see db/Derby.NOTE for details
/*
CREATE TABLE Link (
    id      INT GENERATED ALWAYS AS IDENTITY,
    href0   VARCHAR(128)    NOT NULL,
    href    VARCHAR(128)    NOT NULL,

    UNIQUE (href0, href)
);
CREATE INDEX IDX_Link_href0 ON Link(href0);
CREATE TABLE NotFound(
    word    VARCHAR(64)     NOT NULL,
    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),
    refer   VARCHAR(64),    -- NULL: refer not found
    UREFER  VARCHAR(64) GENERATED ALWAYS AS (UPPER(refer)),

    UNIQUE(word)
);
CREATE INDEX IDX_NotFound_UREFER ON NotFound(UREFER);
CREATE TABLE Vocabulary (
    id      INT GENERATED ALWAYS AS IDENTITY,
    word    VARCHAR(64)     NOT NULL,
    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),
    href0   VARCHAR(128)    NOT NULL,
    wordStr SMALLINT        NOT NULL DEFAULT 0, -- is wordStr a variance of word
                                                --  0: given wordStr is one of words
                                                --  1: otherwise

    UNIQUE(word, href0)
);
CREATE INDEX IDX_Vocabulary_UWORD ON Vocabulary(UWORD);
CREATE TABLE Web (
    href0   VARCHAR(128)    NOT NULL,
    page    VARCHAR(8192)   NOT NULL,   -- Page::getJSONStr()
    more    VARCHAR(4096),              -- More::getJSONStr()
                                        --      NULL:   not retrieved yet
                                        --      "":     none

    UNIQUE(href0)       -- used as INDEX
);
 */
        String q = "";
        q += "CREATE TABLE Link (\n";
        q += "    id      INT GENERATED ALWAYS AS IDENTITY,\n";
        q += "    href0   VARCHAR(128)    NOT NULL,\n";
        q += "    href    VARCHAR(128)    NOT NULL,\n";
        q += "\n";
        q += "    UNIQUE (href0, href)\n";
        q += ")\n";
        Statement stmt = conn.createStatement(); 
        stmt.executeUpdate(q);

        q = "";
        q += "CREATE INDEX IDX_Link_href0 ON Link(href0)\n";
        stmt.executeUpdate(q);

        q = "";
        q += "CREATE TABLE NotFound(\n";
        q += "    word    VARCHAR(64)     NOT NULL,\n";
        q += "    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),\n";
        q += "    refer   VARCHAR(64),    -- NULL: refer not found\n";
        q += "    UREFER  VARCHAR(64) GENERATED ALWAYS AS (UPPER(refer)),\n";
        q += "\n";
        q += "    UNIQUE(word)\n";
        q += ")\n";
        stmt.executeUpdate(q);

        q = "";
        q += "CREATE INDEX IDX_NotFound_UREFER ON NotFound(UREFER)\n";
        stmt.executeUpdate(q);

        q = "";
        q += "CREATE TABLE Vocabulary (\n";
        q += "    id      INT GENERATED ALWAYS AS IDENTITY,\n";
        q += "    word    VARCHAR(64)     NOT NULL,\n";
        q += "    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),\n";
        q += "    href0   VARCHAR(128)    NOT NULL,\n";
        q += "    wordStr SMALLINT        NOT NULL DEFAULT 0, -- is wordStr a variance of word\n";
        q += "                                                --  0: given wordStr is one of words\n";
        q += "                                                --  1: otherwise\n";
        q += "\n";
        q += "    UNIQUE(word, href0)\n";
        q += ")\n";
        stmt.executeUpdate(q);

        q = "";
        q += "CREATE INDEX IDX_Vocabulary_UWORD ON Vocabulary(UWORD)\n";
        stmt.executeUpdate(q);

        q = "";
        q += "CREATE TABLE Web (\n";
        q += "    href0   VARCHAR(128)    NOT NULL,\n";
        q += "    page    VARCHAR(8192)   NOT NULL,   -- Page::getJSONStr()\n";
        q += "    more    VARCHAR(4096),              -- More::getJSONStr()\n";
        q += "                                        --      NULL:   not retrieved yet\n";
        q += "                                        --      \"\":     none\n";
        q += "\n";
        q += "    UNIQUE(href0)       -- used as INDEX\n";
        q += ")\n";
        stmt.executeUpdate(q);

        stmt.close();        
    }
    public Derby(String dbpath, boolean createDatabase) {
        try {
            Class.forName("org.apache.derby.jdbc.EmbeddedDriver").newInstance();
        } catch (ClassNotFoundException e) {
            System.out.println(e.toString());
        } catch (InstantiationException e) {
            System.out.println(e.toString());
        } catch (IllegalAccessException e) {
            System.out.println(e.toString());
        }
        String      urlString = "jdbc:derby:" + dbpath;
        if (createDatabase) {
            urlString += ";create=true";
        }
//      println(urlString);
        // to prevent "derby.log" from being created,
        // -Dderby.stream.error.file=/dev/null
        try {
            Properties  connectionProps = new Properties();
    //      connectionProps.put("user", "username");
    //      connectionProps.put("password", "password");
            conn = DriverManager.getConnection(urlString, connectionProps);
            //System.out.println("Connected to database");
        } catch (SQLException e) {
            // http://db.apache.org/derby/docs/10.4/ref/rrefexcept71493.html
            // SQLSTATE: XJ004
            // Message Text: Database '<databaseName>' not found.
            if (e.getSQLState().equalsIgnoreCase("XJ004")) {
                System.err.printf("SQLException: Database '%s' not found.", dbpath);
                System.err.println();
                System.err.printf("Create it with options |-d \"%s\" -c|", dbpath);
                System.err.println();
            } else {
                printf("oops: |%s|\n", e.getSQLState());
                printSQLException(e);
            }
        }
    }
    static boolean ignoreSQLException(String sqlState) {
        System.out.println(sqlState);
        if (sqlState == null) {
            System.out.println("The SQL state is not defined!");
            return false;
        }
        // X0Y32: Jar file already exists in schema
//        if (sqlState.equalsIgnoreCase("X0Y32"))
 //           return true;
        // 42Y55: Table already exists in schema
//        if (sqlState.equalsIgnoreCase("42Y55"))
//            return true;
        // XJ015: Derby system shutdown
        if (sqlState.equalsIgnoreCase("XJ015"))
            return true;
        return false;
    }
    public static void printSQLException(SQLException ex) {
        for (Throwable e : ex) {
            if (e instanceof SQLException) {
                if (ignoreSQLException(((SQLException)e).getSQLState()) == false) {
                    e.printStackTrace(System.err);
                    System.err.println("SQLState: " + ((SQLException)e).getSQLState());
                    System.err.println("Error Code: " + ((SQLException)e).getErrorCode());
                    System.err.println("Message: " + e.getMessage());
                    Throwable t = ex.getCause();
                    while (t != null) {
                        System.out.println("Cause: " + t);
                        t = t.getCause();
                    }
                }
            }
        }
    }
    static void printf(String format, Object... args)
    {
        System.out.printf(format, args);
    }
    static void println(Object o)
    {
        System.out.printf("%s%n", o);
    }
}
