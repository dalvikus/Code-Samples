package com.dn2soft.dick.utility;

import com.dn2soft.dick.utility.Derby;

import com.dn2soft.dick.dictionary.Cambridge;

import java.util.List;
import java.util.ArrayList;

import java.sql.SQLException;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

public class WordSmartDerby extends Derby {
    public WordSmartDerby(String dbpath) {
        super(dbpath, false);
    }
    public WordSmartDerby(String dbpath, boolean createDatabase) {
        super(dbpath, createDatabase);
    }
/**
 * tables
 *
CREATE TABLE Web (
    href0   VARCHAR(128)    NOT NULL,
    page    VARCHAR(8192)   NOT NULL,   -- Page::getJSONStr()
    more    VARCHAR(4096),              -- More::getJSONStr()
                                        --      NULL:   not retrieved yet
                                        --      "":     none

    UNIQUE(href0)       -- used as INDEX
);

CREATE TABLE Link (
    id      INT GENERATED ALWAYS AS IDENTITY,               -- Derby specific
    href0   VARCHAR(128)    NOT NULL,
    href    VARCHAR(128)    NOT NULL,

    UNIQUE (href0, href)--,
--  FOREIGN KEY (href) REFERENCES Web (href0),
--  FOREIGN KEY (href0) REFERENCES Web (href0)
);
CREATE INDEX IDX_Link_href0 ON Link(href0);

CREATE TABLE Vocabulary (
    id      INT GENERATED ALWAYS AS IDENTITY,               -- Derby specific
    word    VARCHAR(64)     NOT NULL,
    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),  -- Derby specific
    href0   VARCHAR(128)    NOT NULL,
    wordStr SMALLINT        NOT NULL DEFAULT 0, -- is wordStr a variance of word
                                                --  0: given wordStr is one of words
                                                --  1: otherwise

    UNIQUE(word, href0)
);
CREATE INDEX IDX_Vocabulary_UWORD ON Vocabulary(UWORD);     -- Derby specific

CREATE TABLE NotFound(
    word    VARCHAR(64)     NOT NULL,
    UWORD   VARCHAR(64) GENERATED ALWAYS AS (UPPER(word)),  -- Derby specific
    refer   VARCHAR(64),    -- NULL: refer not found
    UREFER  VARCHAR(64) GENERATED ALWAYS AS (UPPER(refer)), -- Derby specific

    UNIQUE(word)
);
CREATE INDEX IDX_NotFound_UREFER ON NotFound(UREFER);       -- Derby specific
 */
/*
 * update: "Vocabulary" heavily used with support of NotFound table - 02/15/2014
 *
 */
/*
 *
 * In a normal usage, "Vocabulary" is not used;
 * its purpose is to speed up looking for word.
 * However, the preferred method is the "direct" check through url in Cambridge::get_href0: 1) spelling check 2) change of the contents of web page
 * For the history and/or unavaliablity of internet,
 * the "Vocabulary" is still maintained.
 *
 * procedure:
 *  step 1. call to Cambridge::get_href0
 *  step 2. inquiry to db with "href0"
 *  step 3. if succeeded, fetch "glob" from "Web" and
 *          "href" from "Link". With "-a" option,
 *          further inquiry to "Web" for each "href",
 *          in case of no "glob" for each "href",
 *          inquiry to web
 *          if failed, i.e., no data, inquery to web
 *  step 4. after display the result, new "glob" is "inserted" into db.
 *
 *  "-f" option bypass all db inquries and
 *  every entry in tables will be "updated" if any.
 */
    List<String> getLinks(String href0)
    throws SQLException
    {
        List<String> links = new ArrayList<String>();
        String q = "SELECT href FROM Link WHERE href0 = ? ORDER BY id";
        PreparedStatement pstmt = conn.prepareStatement(q);
        pstmt.setString(1, href0);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            links.add(rs.getString("href"));
        }
        rs.close();
        pstmt.close();
        return links;
    }
    String[] getPageMore(String href)
    throws SQLException
    {
        String[] r = new String[2];
        String q = "SELECT page, more FROM Web WHERE href0 = ?";
        PreparedStatement pstmt = conn.prepareStatement(q);
        pstmt.setString(1, href);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            r[0] = rs.getString("page");
            r[1] = rs.getString("more");
            break;  // only one guaranteed
        }
        rs.close();
        pstmt.close();
        return r;
    }
    public String href0(String wordStr)
    throws SQLException
    {
        String q;
        boolean isURL = wordStr.startsWith("http://");
        if (isURL)
            q = "SELECT href0 FROM Web WHERE href0 = ?";
        else
            q = "SELECT href0 FROM Vocabulary WHERE UWORD LIKE ? ORDER BY id";
        PreparedStatement pstmt = conn.prepareStatement(q);
        pstmt.setString(1, isURL ? wordStr : wordStr.toUpperCase());
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
    public String select0(String href0, boolean all)
    throws SQLException
    {
        List<String> page_a = new ArrayList<String>();
        String q;
        if (all)
            q = "SELECT Link.href AS href0, Web.page, Web.more FROM Link LEFT OUTER JOIN Web ON Link.href = Web.href0 WHERE Link.href0 = ? ORDER BY Link.id";
        else
            q = "SELECT href0, page, more FROM Web WHERE href0 = ?";
        PreparedStatement pstmt = conn.prepareStatement(q);
        pstmt.setString(1, href0);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            page_a.add(String.format("{'href': '%s', 'content': %s, 'more': %s}", rs.getString("href0"), rs.getString("page"), rs.getString("more")));
        }
        rs.close();
        pstmt.close();
        return page_a.isEmpty() ? null : String.format("[%s]", Joint.join(page_a, ","));
    }

    public void dump()
    throws SQLException
    {
    {
        System.out.println("Web");
        String q = "SELECT href0,page,more FROM Web";
        PreparedStatement pstmt = conn.prepareStatement(q);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            String href0 = rs.getString("href0");
            String page = rs.getString("page");
            String more = rs.getString("more");
            System.out.printf("INSERT INTO Web (href0, page, more) VALUES('%s', '%s', '%s');\n", href0.replace("'", "''"), page.replace("'", "''"), more.replace("'", "''"));
        }
        rs.close();
        pstmt.close();
    }
    {
        System.out.println("Vocabulary");
        String q = "SELECT word, href0 FROM Vocabulary WHERE wordStr = ? ORDER BY id";
        PreparedStatement pstmt = conn.prepareStatement(q);
        pstmt.setInt(1, 0); // exclude a variance
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            String word = rs.getString("word");
            String href0 = rs.getString("href0");
            System.out.printf("INSERT INTO Vocabulary (word, href0) VALUES('%s', '%s');\n", word.replace("'", "''"), href0.replace("'", "''"));
        }
        rs.close();
        pstmt.close();
    }
    {
        System.out.println("Link");
        String q = "SELECT href0, href FROM Link ORDER BY href0, id";
        PreparedStatement pstmt = conn.prepareStatement(q);
        ResultSet rs = pstmt.executeQuery();
        while (rs.next()) {
            String href0 = rs.getString("href0");
            String href = rs.getString("href");
            System.out.printf("INSERT INTO Link (href0, href) VALUES('%s', '%s');\n", href0.replace("'", "''"), href.replace("'", "''"));
        }
        rs.close();
        pstmt.close();
    }
    }

    void deleteFromWeb(String href0)
    throws SQLException
    {
        PreparedStatement deleteStmt = conn.prepareStatement("DELETE FROM Web WHERE href0 = ?");
        deleteStmt.setString(1, href0);
        deleteStmt.executeUpdate();
        deleteStmt.close();
    }
    void deleteFromVocabulary(String href0)
    throws SQLException
    {
        PreparedStatement deleteStmt = conn.prepareStatement("DELETE FROM Vocabulary WHERE href0 = ?");
        deleteStmt.setString(1, href0);
        deleteStmt.executeUpdate();
        deleteStmt.close();
    }
    void deleteFromLink(String href0)
    throws SQLException
    {
        PreparedStatement deleteStmt = conn.prepareStatement("DELETE FROM Link WHERE href0 = ?");
        deleteStmt.setString(1, href0);
        deleteStmt.executeUpdate();
        deleteStmt.close();
    }
    void insertIntoWeb(List<String[]> web_a)
    throws SQLException
    {
        //q = "INSERT INTO Web (href, page, more) VALUES (?, ?, ?)";
        String  q = "INSERT INTO Web (href0, page, more) (SELECT ? AS href0, ? AS page, ? AS more FROM Web WHERE href0 = ? AND page = ? AND more = ? HAVING COUNT(*) = 0)";
        PreparedStatement insertStmt = conn.prepareStatement(q);
        for (String[] a: web_a) {
            insertStmt.setString(1, a[0]);
            insertStmt.setString(2, a[1]);
            insertStmt.setString(3, a[2]);
            insertStmt.setString(4, a[0]);
            insertStmt.setString(5, a[1]);
            insertStmt.setString(6, a[2]);
            insertStmt.executeUpdate();
        }
        insertStmt.close();
    }
    void insertIntoVocabulay(String href0, List<String[]> word_a)
    throws SQLException
    {
        String  q;
        //q = "INSERT INTO Vocabulary (word, href) VALUES (?, ?)";
        q = "INSERT INTO Vocabulary (word, href0, wordStr) (SELECT ? AS word, ? AS href0, ? AS wordStr FROM Vocabulary WHERE word = ? AND href0 = ? AND wordStr = ? HAVING COUNT(*) = 0)";
        PreparedStatement insertStmt = conn.prepareStatement(q);
        for (String[] a: word_a) {
            int i = a[1] == null ? 0 : 1;
            insertStmt.setString(1, a[0]);
            insertStmt.setString(2, href0);
            insertStmt.setInt(3, i);
            insertStmt.setString(4, a[0]);
            insertStmt.setString(5, href0);
            insertStmt.setInt(6, i);
            insertStmt.executeUpdate();
        }
        insertStmt.close();
    }
    void insertIntoLink(String href0, List<String> link_a)
    throws SQLException
    {
        String  q;
        //q = "INSERT INTO Link (href0, href) VALUES (?, ?)";
        q = "INSERT INTO Link (href0, href) (SELECT ? AS href0, ? AS href FROM Link WHERE href0 = ? AND href = ? HAVING COUNT(*) = 0)";
        PreparedStatement insertStmt = conn.prepareStatement(q);
        for (String link: link_a) {
            insertStmt.setString(1, href0);
            insertStmt.setString(2, link);
            insertStmt.setString(3, href0);
            insertStmt.setString(4, link);
            insertStmt.executeUpdate();
        }
        insertStmt.close();
    }
    public void insert(Cambridge.Result result, boolean force)
    throws SQLException
    {
    /*
        String  rjs = result.getJSONStr();
        //System.err.println(rjs);
        Json.BasicType  rj = Json.setJson(rjs);
        if (rj != null) {
            rj.print(1, "....");
        }
     */
        String  href0 = result.doc.get(0).href;
        //println("HREF0: |" + href0 + "|");
        try {
            conn.setAutoCommit(false);
        } catch (SQLException e) {
            printSQLException(e);
        }

        boolean okay = false;
        try {
            if (force) {
                List<String> links = getLinks(href0);
                deleteFromLink(href0);
                deleteFromWeb(href0);
                deleteFromVocabulary(href0);
                for (String href: links) {
                    deleteFromWeb(href);
                    deleteFromVocabulary(href);
                }
            }

            List<String[]>  web_a = new ArrayList<String[]>();
            for (Cambridge.Result.Page page: result.doc) {
                String[] a = new String[3];
                a[0] = page.href;
                a[1] = page.contentJSONStr();
                a[2] = page.more.getJSONStr();
    ////        Json.BasicType pj = Json.setJson(a[1]);
    ////        pj.print(0, ">");
                web_a.add(a);
            }
            insertIntoWeb(web_a);

            String wordStr = result.wordStr;
            boolean isURL = wordStr.startsWith("http://");
            if (isURL)
                wordStr = wordStr.trim().replaceAll("\\s+", " ");
            boolean done = false;   // only for the first page
            boolean isNotAVariance = isURL; // !isURL case only
            for (Cambridge.Result.Page page: result.doc) {
                List<String[]> word_a = new ArrayList<String[]>();
                for (Cambridge.Block block: page.content) {
    ////            String  bjs = block.getJSONStr();
    ////            Json.BasicType  bj = Json.setJson(bjs);
    ////            bj.print(0, ">>>>");
                    String[] a = new String[2];
                    a[0] = block.head.word;
                    a[1] = null;
                    word_a.add(a);
                    if (!done && !isURL && !isNotAVariance && wordStr.toUpperCase().equals(block.head.word.toUpperCase()))
                        isNotAVariance = true;
                }
                if (!done) {
                    if (!isNotAVariance) {
                        String[] a = new String[2];
                        a[0] = wordStr; // as it is
                        a[1] = "";  // not null
                        word_a.add(a);
                    }
                    done = true;
                }
                insertIntoVocabulay(page.href, word_a);
            }

            List<String>  link_a = new ArrayList<String>();
            link_a.add(href0);  // for left outer join
            for (Cambridge.More.Info info: result.doc.get(0).more.infoList)
                link_a.add(info.href);
            insertIntoLink(href0, link_a);

            okay = true;
        } catch (SQLException e) {
            printSQLException(e);
        } finally {
            try {
                if (okay)
                    conn.commit();
                else
                    conn.rollback();
            } catch (SQLException e) {
                printSQLException(e);
            }
        }

        try {
            conn.setAutoCommit(true);
        } catch (SQLException e) {
            printSQLException(e);
        }
    }
}
