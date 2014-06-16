package com.dn2soft.dick.android;

import com.dn2soft.dick.utility.Jsonable;
import com.dn2soft.dick.utility.Json;
import com.dn2soft.dick.utility.JsonWalk;
import com.dn2soft.dick.utility.Joint;
import android.util.Log;

import android.content.Context;
import android.provider.BaseColumns;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;

import android.database.Cursor;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

import java.io.File;
public final class WordSmartContract {
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private WordSmartContract() {}

    public static abstract class TheWords {
        public static final String TABLE_NAME = "TheWords";
        public static final String COLUMN_NAME_BOOK_NO = "book_no";
        public static final String COLUMN_NAME_UNIT = "unit";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_POS = "pos";
        public static final String COLUMN_NAME_PRON = "pron";
        public static final String COLUMN_NAME_SENSE = "sense";
        public static final String COLUMN_NAME_EXAMPLES = "examples";

        public static final int LIMIT = 10;

/*
CREATE TABLE TheWords (
    book_no     INTEGER,    -- 1: Word Smart I
                            -- 2: Word Smart II
                            -- 3: Word Smart for the GRE
    unit        INTEGER,
    word        TEXT,
    pos         TEXT,
    pron        TEXT,
    sense       TEXT,
    examples    TEXT,   -- "\t" used for indentation
                        -- "        " (eight blanks) used for item in ALLEGE, BOVINE, EPIGRAM

    PRIMARY KEY (book_no, word)
);
 */
        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }
    public static abstract class TheHitParade {
        public static final String TABLE_NAME = "TheHitParade";
        public static final String COLUMN_NAME_TYPE = "type";
        public static final String COLUMN_NAME_BOOK = "book";
        public static final String COLUMN_NAME_PAGE = "page";
        public static final String COLUMN_NAME_LINE = "line";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_SENSE = "sense";

/*
CREATE TABLE TheHitParade (
    type    INTEGER,    -- 1: SAT, 2: GRE
    book    INTEGER,    -- 1: Word Smart I, 2: Word Smart II
    -- in a book, words are not sorted
    -- (page, line) shows the words at they are
    page    INTEGER,
    line    INTEGER,
    word    TEXT,
    sense   TEXT,

    PRIMARY KEY (type, book, word)
);
 */

        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }
    public static abstract class Root {
        public static final String TABLE_NAME = "Root";
        public static final String COLUMN_NAME_ROOT = "root";
        public static final String COLUMN_NAME_SENSE = "sense";
        public static final String COLUMN_NAME_WORD = "word";

/*
CREATE TABLE Root (
    root    TEXT,
    sense   TEXT,
    word    TEXT,

    PRIMARY KEY (root, word)
);
 */

        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }

    /**
     * Wrapper class of Word Smart database
     *
     * It is used to get json string(JsonWalk.walkClass) and to set class(JsonWalk.setWalkClassJson) by JsonWalk class.
     * All member should be public for JsonWalk.setWalkClassJson to access.
     */
    public static class WordSmartJson implements Jsonable
    {
        public static class TheWords implements Jsonable
        {
            public String book_no;
            public String unit;
            public String word;
            public String pos;
            public String pron;
            public String sense;
            public String[] example;
        }
        public List<TheWords>  theWordsList;
        public static class TheHitParade implements Jsonable
        {
            public String type;
            public String book;
            public String word;
            public String sense;
        }
        public List<TheHitParade> theHitParadeList;
        public static class Root implements Jsonable
        {
            public String root;
            public String sense;
            public List<String> wordList;
        }
        public List<Root>  rootList;
    }

    public static class WordSmartDbHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;
        public static final String DATABASE_NAME = "WordSmart.db";

        public WordSmartDbHelper(Context context, File rootPath) {
            super(context, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME), null, DATABASE_VERSION);
        }
        public void onCreate(SQLiteDatabase db) {
            /* just use it */
        }
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            /* just use it */
        }
        public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            onUpgrade(db, oldVersion, newVersion);
        }

        public List<String> lookupUnit(String bookNoStr, String unitStr)
        {
            List<String>    word_a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
                "SELECT %s FROM %s WHERE %s = ? AND %s = ? ORDER BY %s"
                ,
                TheWords.COLUMN_NAME_WORD,
                TheWords.TABLE_NAME,
                TheWords.COLUMN_NAME_BOOK_NO,
                TheWords.COLUMN_NAME_UNIT,
                TheWords.COLUMN_NAME_WORD
            );
            String[] projection = {bookNoStr, unitStr};

            Cursor c = db.rawQuery(q, projection);
            while (c.moveToNext()) {
                String word = c.getString(0);
                word_a.add(word);
            }
            return word_a;
        }
        public List<String> suggestWord(String wordPattern)
        {
            List<String>    word_a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
                "SELECT DISTINCT %s FROM %s WHERE %s LIKE ? ORDER BY %s"
                ,
                TheWords.COLUMN_NAME_WORD,
                TheWords.TABLE_NAME,
                TheWords.COLUMN_NAME_WORD,
                TheWords.COLUMN_NAME_WORD
            );
            String[] projection = {wordPattern};

            Cursor c = db.rawQuery(q, projection);
            while (c.moveToNext()) {
                String word = c.getString(0);
                word_a.add(word);
            }
            return word_a;
        }
        private List<WordSmartJson.TheWords> selectTheWords(String word)
        {
            SQLiteDatabase db = getReadableDatabase();
            //String  q = "SELECT book_no,unit,word,pos,pron,sense,examples FROM TheWords WHERE word LIKE ? OR sense LIKE ? OR examples LIKE ? ORDER BY book_no,unit,word";
            String  q = String.format(
                "SELECT %s,%s,%s,%s,%s,%s,%s FROM %s WHERE %s LIKE ? OR %s LIKE ? OR %s LIKE ? ORDER BY %s,%s,%s LIMIT %d"
                ,
                TheWords.COLUMN_NAME_BOOK_NO,
                TheWords.COLUMN_NAME_UNIT,
                TheWords.COLUMN_NAME_WORD,
                TheWords.COLUMN_NAME_POS,
                TheWords.COLUMN_NAME_PRON,
                TheWords.COLUMN_NAME_SENSE,
                TheWords.COLUMN_NAME_EXAMPLES,
                TheWords.TABLE_NAME,
                TheWords.COLUMN_NAME_WORD,
                TheWords.COLUMN_NAME_SENSE,
                TheWords.COLUMN_NAME_EXAMPLES,
                TheWords.COLUMN_NAME_BOOK_NO,
                TheWords.COLUMN_NAME_UNIT,
                TheWords.COLUMN_NAME_WORD,
                TheWords.LIMIT + 1
            );
            String[] projection = {"%" + word + "%", "%" + word + "%", "%" + word + "%"};

            Cursor c = db.rawQuery(q, projection);
            List<WordSmartJson.TheWords> theWordsList = new ArrayList<WordSmartJson.TheWords>();
            while (c.moveToNext()) {
                WordSmartJson.TheWords theWords = new WordSmartJson.TheWords();
                theWords.book_no = String.format("%d", c.getInt(0));
                theWords.unit = String.format("%d", c.getInt(1));
                theWords.word = c.getString(2);
                theWords.pos = c.getString(3);
                theWords.pron = c.getString(4);
                theWords.sense = c.getString(5);
                theWords.example = c.getString(6).split("\n");
                theWordsList.add(theWords);
            }
            return theWordsList;
        }

        private List<WordSmartJson.TheHitParade> selectTheHitParade(String word)
        {
            SQLiteDatabase db = getReadableDatabase();
            //String  q = "SELECT type,book,word,sense FROM TheHitParade WHERE word LIKE ? OR sense LIKE ? ORDER BY type,book,word";
            String  q = String.format(
                "SELECT %s,%s,%s,%s FROM %s WHERE %s LIKE ? OR %s LIKE ? ORDER BY %s,%s,%s"
                ,
                TheHitParade.COLUMN_NAME_TYPE,
                TheHitParade.COLUMN_NAME_BOOK,
                TheHitParade.COLUMN_NAME_WORD,
                TheHitParade.COLUMN_NAME_SENSE,
                TheHitParade.TABLE_NAME,
                TheHitParade.COLUMN_NAME_WORD,
                TheHitParade.COLUMN_NAME_SENSE,
                TheHitParade.COLUMN_NAME_TYPE,
                TheHitParade.COLUMN_NAME_BOOK,
                TheHitParade.COLUMN_NAME_WORD
            );
            String[] projection = {"%" + word + "%", "%" + word + "%"};

            Cursor c = db.rawQuery(q, projection);
            List<WordSmartJson.TheHitParade> theHitParadeList = new ArrayList<WordSmartJson.TheHitParade>();
            while (c.moveToNext()) {
                WordSmartJson.TheHitParade theHitParade = new WordSmartJson.TheHitParade();
                theHitParade.type = String.format("%d", c.getInt(0));
                theHitParade.book = String.format("%d", c.getInt(1));
                theHitParade.word = c.getString(2);
                theHitParade.sense = c.getString(3);
                theHitParadeList.add(theHitParade);
            }
            return theHitParadeList;
        }

        public List<String> getRootWord(String word)
        {
            String  q = String.format(
                "SELECT DISTINCT A.%s,A.%s,A.%s from %s AS A LEFT OUTER JOIN %s AS B ON A.%s = B.%s WHERE B.%s LIKE ? ORDER BY A.%s,A.%s"
                ,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_SENSE,
                Root.COLUMN_NAME_WORD,
                Root.TABLE_NAME,
                Root.TABLE_NAME,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_WORD
            );
            String[] projection = {word};

            SQLiteDatabase db = getReadableDatabase();
            Cursor c = db.rawQuery(q, projection);
            List<String>    row_a = new ArrayList<String>();
            while (c.moveToNext()) {
                String r = c.getString(0);
                String s = c.getString(1);
                String w = c.getString(2);
                row_a.add(String.format("%s|%s|%s", r, s, w));
            }
            return row_a;
        }
        private List<WordSmartJson.Root> selectRoot(String word)
        {
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
                "SELECT DISTINCT A.%s,A.%s,A.%s from %s AS A LEFT OUTER JOIN %s AS B ON A.%s = B.%s WHERE B.%s LIKE ? ORDER BY A.%s,A.%s"
                ,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_SENSE,
                Root.COLUMN_NAME_WORD,
                Root.TABLE_NAME,
                Root.TABLE_NAME,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_WORD,
                Root.COLUMN_NAME_ROOT,
                Root.COLUMN_NAME_WORD
            );
            String[] projection = {"%" + word + "%"};

            Cursor c = db.rawQuery(q, projection);
            List<WordSmartJson.Root> rootList = new ArrayList<WordSmartJson.Root>();
            String  rootStr = null;
            WordSmartJson.Root root = null;
            while (c.moveToNext()) {
                String r = c.getString(0);
                String s = c.getString(1);
                String w = c.getString(2);
                if (rootStr == null || !rootStr.equals(r)) {
                    rootStr = r;
                    root = new WordSmartJson.Root();
                    rootList.add(root);
                    root.root = r;
                    root.sense = w;
                    root.wordList = new ArrayList<String>();
                    root.wordList.add(w);
                } else if (rootStr.equals(r)) {
                    root.wordList.add(w);
                }
            }
            return rootList;
        }

        public WordSmartJson select(String word)
        {
            WordSmartJson wordSmartJson = new WordSmartJson();
            wordSmartJson.theWordsList = selectTheWords(word);
            wordSmartJson.theHitParadeList = selectTheHitParade(word);
            wordSmartJson.rootList = selectRoot(word);
            return wordSmartJson;
        }
        public String selectJSONStr(String word)
        throws ClassNotFoundException, IllegalAccessException
        {
            return JsonWalk.walkClass(WordSmartJson.class, select(word), "    ");
        }
    }

    public static WordSmartJson setWordSmart(String jsonStrWordSmart)
    throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        return (WordSmartJson) JsonWalk.setWalkClassJson(WordSmartJson.class, Json.setJson(jsonStrWordSmart), "    ");
    }
    public static void print(String jsonStrWordSmart)
    {
        WordSmartJson wordSmartJson = null;
        try {
            wordSmartJson = setWordSmart(jsonStrWordSmart);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        if (wordSmartJson == null)
            return;
        Log.i("Word Smart", "...");
        if (wordSmartJson.theWordsList.size() > 0) {
            Log.i("Word Smart", "The Words: ...");
            for (WordSmartJson.TheWords theWords: wordSmartJson.theWordsList) {
                Log.i("MAP", TheWords.COLUMN_NAME_BOOK_NO + ": " + theWords.book_no);
                Log.i("MAP", TheWords.COLUMN_NAME_UNIT + ": " + theWords.unit);
                Log.i("MAP", TheWords.COLUMN_NAME_WORD + ": " + theWords.word);
                Log.i("MAP", TheWords.COLUMN_NAME_POS + ": " + theWords.pos);
                Log.i("MAP", TheWords.COLUMN_NAME_PRON + ": " + theWords.pron);
                Log.i("MAP", TheWords.COLUMN_NAME_SENSE + ": " + theWords.sense);
                for (String ex: theWords.example) 
                    Log.i("MAP", "example: |" + ex + "|");
            }
        } else {
            Log.i("Word Smart", "The Words: NULL");
        }
        if (wordSmartJson.theHitParadeList.size() > 0) {
            Log.i("Word Smart", "The Hit Parade: ...");
            for (WordSmartJson.TheHitParade theHitParade: wordSmartJson.theHitParadeList) {
                Log.i("MAP", TheHitParade.COLUMN_NAME_TYPE + ": " + theHitParade.type);
                Log.i("MAP", TheHitParade.COLUMN_NAME_BOOK + ": " + theHitParade.book);
                Log.i("MAP", TheHitParade.COLUMN_NAME_WORD + ": " + theHitParade.word);
                Log.i("MAP", TheHitParade.COLUMN_NAME_SENSE + ": " + theHitParade.sense);
            }
        } else {
            Log.i("Word Smart", "The Hit Parade: NULL");
        }
        if (wordSmartJson.rootList.size() > 0) {
            Log.i("Word Smart", "Root: ...");
            for (WordSmartJson.Root root: wordSmartJson.rootList) {
                Log.i("MAP", Root.COLUMN_NAME_ROOT + ": " + root.root);
                Log.i("MAP", Root.COLUMN_NAME_SENSE + ": " + root.sense);
                Log.i("MAP", Root.COLUMN_NAME_WORD + ": " + Joint.join(root.wordList, ", "));
            }
        } else {
            Log.i("Word Smart", "Root: NULL");
        }
    }
}
