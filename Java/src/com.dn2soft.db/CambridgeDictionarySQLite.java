package com.dn2soft.db;

import com.dn2soft.util.Jsonable;
import com.dn2soft.util.Joint;
import com.dn2soft.dickc.dictionary.Cambridge;
import com.dn2soft.db.SQLiteJDBC;

import java.sql.SQLException;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

import java.util.List;
import java.util.ArrayList;

public class CambridgeDictionarySQLite extends SQLiteJDBC
{
    public CambridgeDictionarySQLite(String dbpath)
    throws ClassNotFoundException, SQLException
    {
        super(dbpath);
    }
/*
CREATE TABLE Web (
    href0   VARCHAR(128)    NOT NULL,
    page    VARCHAR(8192)   NOT NULL,   -- Page::getJSONStr()
    more    VARCHAR(4096),              -- More::getJSONStr()
                                        --      NULL:   not retrieved yet
                                        --      "":     none

    UNIQUE(href0)       -- used as INDEX
);
---- sqlite
-- http://www.sqlite.org/lang_createtable.html
-- In most cases, UNIQUE and PRIMARY KEY constraints are implemented by creating a unique index in the database. (The exceptions are INTEGER PRIMARY KEY and PRIMARY KEYs on WITHOUT ROWID tables.) Hence, the following schemas are logically equivalent:
--
--      CREATE TABLE t1(a, b UNIQUE);
--
--      CREATE TABLE t1(a, b PRIMARY KEY);
--
--      CREATE TABLE t1(a, b);
--      CREATE UNIQUE INDEX t1b ON t1(b);
----

---- Derby
-- http://db.apache.org/derby/docs/10.1/ref/rrefsqlj20937.html
-- If a column or set of columns has a UNIQUE or PRIMARY KEY constraint on it, you can not create an index on those columns.
--      WARNING 01504: The new index is a duplicate of an existing index: SQL140214093425020.
----
BEGIN TRANSACTION;                                      -- sqlite specific
AUTOCOMMIT OFF;                                             -- Derby specific
......
COMMIT;
 */
/*
CREATE TABLE Vocabulary (
    id      INT GENERATED ALWAYS AS IDENTITY,               -- Derby specific
    _id     INTEGER PRIMARY KEY,                        -- sqlite specific
    word    VARCHAR(64)     NOT NULL,
    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),  -- Derby specific
    href0   VARCHAR(128)    NOT NULL,
    wordStr SMALLINT        NOT NULL DEFAULT 0, -- is wordStr a variance of word
                                                --  0: given wordStr is one of words
                                                --  1: otherwise

    UNIQUE(word, href0)
);
CREATE INDEX IDX_Vocabulary_word ON Vocabulary(word);   -- sqlite specific
CREATE INDEX IDX_Vocabulary_UWORD ON Vocabulary(UWORD);     -- Derby specific
BEGIN TRANSACTION;                                      -- sqlite specific
AUTOCOMMIT OFF;                                             -- Derby specific
......
COMMIT;
 */
/*
CREATE TABLE Link (
    id      INT GENERATED ALWAYS AS IDENTITY,               -- Derby specific
    _id     INTEGER PRIMARY KEY,                        -- android specific
    href0   VARCHAR(128)    NOT NULL,
    href    VARCHAR(128)    NOT NULL,

    UNIQUE (href0, href)--,
--  FOREIGN KEY (href) REFERENCES Web (href0),
--  FOREIGN KEY (href0) REFERENCES Web (href0)
);
CREATE INDEX IDX_Link_href0 ON Link(href0);
BEGIN TRANSACTION;                                      -- sqlite specific
AUTOCOMMIT OFF;                                             -- Derby specific
......
 */
    public String href0(String wordStr)
    throws SQLException
    {
        String q;
        boolean isURL = wordStr.startsWith("http://");
        if (isURL)
            q = "SELECT href0 FROM Web WHERE href0 = ?";
        else
            q = "SELECT href0 FROM Vocabulary WHERE word LIKE ? ORDER BY _id";
        PreparedStatement pstmt = mConn.prepareStatement(q);
        pstmt.setString(1, wordStr);
        ResultSet rs = pstmt.executeQuery();
        String href0 = null;
        while (rs.next()) {
            href0 = rs.getString("href0");
            break;
        }
        rs.close();
        pstmt.close();
        return href0;
    }

    class Audio implements Jsonable {
        public List<Cambridge.Block>  content = new ArrayList<Cambridge.Block>();
    }
    public void dumpAudio()
    throws SQLException
    {
        String  q;
        q = "SELECT href0, page, more FROM Web";
        PreparedStatement pstmt = mConn.prepareStatement(q);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            System.out.println(rs.getString("href0"));
            System.out.println(rs.getString("page"));
/*
                String.format(
                    "{'href': '%s', 'content': %s, 'more': %s}"
                    ,
                    rs.getString("href0"),
                    rs.getString("page"),
                    rs.getString("more")
                )
            );
 */
        }
        rs.close();
        pstmt.close();
    }
    public String select0(String href0, boolean all)
    throws SQLException
    {
        String  q;
        if (all)
            q = "SELECT Link.href AS href0, Web.page, Web.more FROM Link LEFT OUTER JOIN Web ON Link.href = Web.href0 WHERE Link.href0 = ? ORDER BY Link._id";
        else
            q = "SELECT href0, page, more FROM Web WHERE href0 = ?";
        PreparedStatement pstmt = mConn.prepareStatement(q);
        pstmt.setString(1, href0);
        ResultSet rs = pstmt.executeQuery();
        List<String>    page_a = new ArrayList<String>();
        while (rs.next()) {
            page_a.add(
                String.format(
                    "{'href': '%s', 'content': %s, 'more': %s}"
                    ,
                    all ? rs.getString("href0") : href0,
                    rs.getString("page"),
                    rs.getString("more")
                )
            );
        }
        rs.close();
        pstmt.close();
        return page_a.isEmpty() ? null : String.format("[%s]", Joint.join(page_a, ","));
    }
    public void insert(final Cambridge.Result result, final boolean force)
    throws SQLException
    {
        if (result == null)
            throw new IllegalArgumentException("result is null");

//      String wordStr = result.wordStr.trim().replaceAll("\\s+", " ");
//      boolean isURL = result.wordStr.startsWith("http://");
//      boolean isNotAVariance = isURL; // !isURL case only
        String wordStr = result.wordStr;
        boolean isURL = wordStr.startsWith("http://");
        if (isURL)
            wordStr = wordStr.trim().replaceAll("\\s+", " ");
        boolean isNotAVariance = isURL; // !isURL case only

        PreparedStatement pstmt = null;
        String  q = null;

        mConn.setAutoCommit(false);
        boolean okay = false;
        try {
            for (int ipage = 0; ipage < result.doc.size(); ++ipage) {
                Cambridge.Result.Page page = result.doc.get(ipage);
                String  href0 = page.href;

                // Web
                q = "INSERT OR " + (force ? "REPLACE" : "IGNORE") + " INTO Web (href0, page, more) VALUES (?, ?, ?)";
                pstmt = mConn.prepareStatement(q);
                pstmt.setString(1, href0);
                pstmt.setString(2, page.contentJSONStr());
                pstmt.setString(3, page.more.getJSONStr());
                pstmt.executeUpdate();

                // Vocabulary
                for (Cambridge.Block block: page.content) {
                    q = "INSERT OR " + (force ? "REPLACE" : "IGNORE") + " INTO Vocabulary (word, href0, wordStr) VALUES (?, ?, ?)";
                    pstmt = mConn.prepareStatement(q);
                    pstmt.setString(1, block.head.word);
                    pstmt.setString(2, href0);
                    pstmt.setShort(3, (short) 0);
                    pstmt.executeUpdate();
                    if (
                        ipage == 0 &&
                        !isNotAVariance &&
                        !isURL &&
                        wordStr.toUpperCase().equals(block.head.word.toUpperCase())
                    ) {
                        isNotAVariance = true;
                    }
                }
                if (ipage == 0) {
                    if (!isNotAVariance) {
                        q = "INSERT OR " + (force ? "REPLACE" : "IGNORE") + " INTO Vocabulary (word, href0, wordStr) VALUES (?, ?, ?)";
                        pstmt = mConn.prepareStatement(q);
                        pstmt.setString(1, wordStr);
                        pstmt.setString(2, href0);
                        pstmt.setInt(3, (short) 1);
                        pstmt.executeUpdate();
                    }
                }

                // Link
                if (ipage == 0) {
                    List<String>    links = new ArrayList<String>();
                    links.add(href0);   // for INNER OUTER JOIN (See select0)
                    for (Cambridge.More.Info info: result.doc.get(0).more.infoList)
                        links.add(info.href);
                    for (String link: links) {
                        q = "INSERT OR " + (force ? "REPLACE" : "IGNORE") + " INTO Link (href0, href) VALUES (?, ?)";
                        pstmt = mConn.prepareStatement(q);
                        pstmt.setString(1, href0);
                        pstmt.setString(2, link);
                        pstmt.executeUpdate();
                    }
                }
            }

            okay = true;
        } finally {
            if (okay)
                mConn.commit();
            else
                mConn.rollback();
        }
    }



    public static void main(String args[])
    {
        System.out.printf("# of arguments is %d%n", args.length);
        if (args.length < 2) {
            System.out.println("Usage:");
            System.out.println("\tdbpath word|url");
            System.exit(-1);
        }
        String  dbpath = args[0];
        String  wordStr = args[1];
        CambridgeDictionarySQLite   cd = null;
        try {
            cd = new CambridgeDictionarySQLite(dbpath);
        } catch (ClassNotFoundException e) {
            System.err.println(e.getClass().getName() + ": " + e.getMessage());
            System.err.println("TIP: Do you include \"sqlite-jdbc-#.#.#.jar\"");
            System.err.println("TIP:     -cp sqlite-jdbc-#.#.#.jar");
            System.err.println("TIP: Or in CLASSPATH");
            System.exit(-1);
        } catch (SQLException e) {
            System.err.println(e.getClass().getName() + ": " + e.getMessage());
        }

        String  href0 = null;
        try {
            href0 = cd.href0(wordStr);
            System.out.println("href0 for \"" + wordStr + "\":");
            System.out.printf("    ");
            System.out.println(href0 == null ? "NULL" : ("\"" + href0 + "\""));
        } catch (SQLException e) {
            System.err.println(e.getClass().getName() + ": " + e.getMessage());
        }

        Cambridge.Flag  flag = new Cambridge.Flag();
        if (href0 == null) {
            try {
                href0 = Cambridge.getHref0(wordStr, flag);
            } catch (java.io.IOException e) {
                System.err.println(e.getClass().getName() + ": " + e.getMessage());
            }
            if (href0 == null) {
                System.err.println("ERROR: Cambridge.getHref0(\"" + wordStr + "\"): failed");
                System.exit(-1);
            }
        }
        String JSONStr = null;
        try {
            JSONStr = cd.select0(href0, flag.showAll);
        } catch (SQLException e) {
            System.err.println(e.getClass().getName() + ": " + e.getMessage());
        }
        System.out.println("JSONStr for \"" + href0 + "\"");
        System.out.println("    " + (JSONStr == null ? "NULL" : JSONStr));
    }
}
