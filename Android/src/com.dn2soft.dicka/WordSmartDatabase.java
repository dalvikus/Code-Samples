package com.dn2soft.dicka;

import java.util.Collections;
import java.util.Comparator;
import android.app.SearchManager;
import com.dn2soft.dicka.WordSmartContract.WordSmartJson;
import com.dn2soft.dicka.WordSmartContract.RootListJson;

import android.util.Log;

import android.content.Context;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.Cursor;
import java.util.List;
import java.util.ArrayList;
import java.io.File;

public final class WordSmartDatabase {
    private static final String TAG = "WordSmartDatabase";
    public static final String DATABASE_NAME = "WordSmart.db";

    public static class WordSmartOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public WordSmartOpenHelper(Context context, File rootPath) {
            super(context, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME), null, DATABASE_VERSION);
////    Log.i(TAG, "WordSmartOpenHelper(): " + (rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME)));
        }
        @Override
        public void onCreate(SQLiteDatabase db) {
            /* just use it */
        }
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            /* just use it */
        }

        // String[]: ["word", "sense", ...]
        public List<List<String>> selectBook(int book)
        {
            SQLiteDatabase db = getReadableDatabase();
            Cursor c = db.query(
                WordSmartContract.TheWords.TABLE_NAME,
                new String[] {
                    WordSmartContract.TheWords.COLUMN_NAME_UNIT,
                    WordSmartContract.TheWords.COLUMN_NAME_WORD
                },
                String.format(
                    "%s = CAST(? AS INTEGER)"
                    ,
                    WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO
                ),
                new String[] {Integer.toString(book)},
                null,
                null,
                String.format(
                    "%s, %s",
                    WordSmartContract.TheWords.COLUMN_NAME_UNIT,
                    WordSmartContract.TheWords.COLUMN_NAME_WORD
                ),
                null
            );
            List<List<String>> aa = new ArrayList<List<String>>();
            int unit = -1;
            List<String> a = null;
            while (c.moveToNext()) {
                int i = c.getInt(0);
                if (a == null || unit != i) {
                    a = new ArrayList<String>();
                    aa.add(a);
                    unit = i;
                }
                a.add(c.getString(1));
            }
            c.close();
            return aa;
        }
        public List<String> lookupUnit(String bookNoStr, String unitStr)
        {
            List<String>    word_a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();
/*
            String  q = String.format(
//              "SELECT %s FROM %s WHERE %s = ? AND %s = ? ORDER BY %s"
                "SELECT %s FROM %s WHERE %s = CAST(? AS INTEGER) AND %s = CAST(? AS INTEGER) ORDER BY %s"
                ,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                WordSmartContract.TheWords.TABLE_NAME,
                WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO,
                WordSmartContract.TheWords.COLUMN_NAME_UNIT,
                WordSmartContract.TheWords.COLUMN_NAME_WORD
            );
            String[] projection = {bookNoStr, unitStr};

            Cursor c = db.rawQuery(q, projection);
 */
            Cursor c = db.query(
                WordSmartContract.TheWords.TABLE_NAME,
                new String[] {
                    WordSmartContract.TheWords.COLUMN_NAME_WORD
                },
                String.format(
                    "%s = CAST(? AS INTEGER) AND %s = CAST(? AS INTEGER)"
                    ,
                    WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO,
                    WordSmartContract.TheWords.COLUMN_NAME_UNIT
                ),
                new String[] {bookNoStr, unitStr},
                null,
                null,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                null
            );
            while (c.moveToNext()) {
                String word = c.getString(0);
                word_a.add(word);
            }
            c.close();
            return word_a;
        }
        public List<String> suggestWord(String wordPattern)
        {
            List<String>    word_a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
                "SELECT DISTINCT %s FROM %s WHERE %s LIKE ? ORDER BY %s"
                ,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                WordSmartContract.TheWords.TABLE_NAME,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                WordSmartContract.TheWords.COLUMN_NAME_WORD
            );
            String[] projection = {wordPattern};

            Cursor c = db.rawQuery(q, projection);
            while (c.moveToNext()) {
                String word = c.getString(0);
                word_a.add(word);
            }
            c.close();
            return word_a;
        }

        public Cursor getSuggestions(String selection, String[] projection)
        {
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
//              "SELECT rowid AS _id, %s AS %s,%s AS %s, rowid AS _id, %s AS %s, %s AS %s FROM %s WHERE %s MATCH ? ORDER BY %s,%s"
//              "SELECT rowid AS _id, %s AS %s,%s AS %s, rowid AS _id, %s AS %s, %s AS %s FROM %s WHERE %s LIKE ? ORDER BY %s,%s"
                "SELECT rowid AS _id, %s AS %s,%s AS %s, rowid AS _id, %s AS %s, %s AS %s FROM %s WHERE " + selection + " ORDER BY %s,%s"
                ,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                SearchManager.SUGGEST_COLUMN_TEXT_1,
                WordSmartContract.TheWords.COLUMN_NAME_SENSE,
                SearchManager.SUGGEST_COLUMN_TEXT_2,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                SearchManager.SUGGEST_COLUMN_INTENT_EXTRA_DATA,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                SearchManager.SUGGEST_COLUMN_QUERY,
                WordSmartContract.TheWords.TABLE_NAME,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO
            );

            Cursor c = db.rawQuery(q, projection);
            if (c == null) {    // an error occurred
                Log.e(TAG, "query: |" + q + "| with |" + projection + "|");
                Log.e(TAG, "An error occurred");
                return null;
            }
            if (!c.moveToFirst()) { // no data
                c.close();
                return null;
            }
            // debug
/*
            while (c.moveToNext()) {
                String rowid = c.getString(0);
                String word = c.getString(1);
                String sense = c.getString(2);
                Log.i(TAG, "rowid: " + rowid);
                Log.i(TAG, "word: |" + word + "|");
                Log.i(TAG, "    sense: |" + sense + "|");
            }
            c.moveToFirst();
 */
            return c;
        }

        private List<WordSmartJson.TheWords> selectTheWords(String selection, String[] projection)
        {
            SQLiteDatabase db = getReadableDatabase();
            //String  q = "SELECT book_no,unit,word,pos,pron,sense,examples FROM TheWords WHERE word LIKE ? OR sense LIKE ? OR examples LIKE ? ORDER BY book_no,unit,word";
            String  q = String.format(
//              "SELECT %s,%s,%s,%s,%s,%s,%s FROM %s WHERE %s LIKE ? OR %s LIKE ? OR %s LIKE ? ORDER BY %s,%s,%s LIMIT %d"
////            "SELECT %s,%s,%s,%s,%s,%s,%s,snippet(%s, '<<', '>>', '...', -1, -5),offsets(%s) FROM %s WHERE %s MATCH ? ORDER BY %s,%s,%s LIMIT %d"
                "SELECT %s,%s,%s,%s,%s,%s,%s,snippet(%s, '<<', '>>', '...', -1, -5),offsets(%s) FROM %s WHERE " + selection + " ORDER BY %s,%s,%s LIMIT %d"
                ,
                WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO,
                WordSmartContract.TheWords.COLUMN_NAME_UNIT,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                WordSmartContract.TheWords.COLUMN_NAME_POS,
                WordSmartContract.TheWords.COLUMN_NAME_PRON,
                WordSmartContract.TheWords.COLUMN_NAME_SENSE,
                WordSmartContract.TheWords.COLUMN_NAME_EXAMPLES,
                WordSmartContract.TheWords.TABLE_NAME,  // snippet
                WordSmartContract.TheWords.TABLE_NAME,  // offsets
                WordSmartContract.TheWords.TABLE_NAME,  // FROM
////            WordSmartContract.TheWords.TABLE_NAME,
                WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO,
                WordSmartContract.TheWords.COLUMN_NAME_UNIT,
                WordSmartContract.TheWords.COLUMN_NAME_WORD,
                WordSmartContract.TheWords.LIMIT + 1
            );
            //Log.i(TAG, q);
//          String[] projection = {"%" + word + "%", "%" + word + "%", "%" + word + "%"};
//          String[] projection = {word};

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
                //theWords.example = c.getString(6).split("\n");
                theWords.examples = c.getString(6);
                theWords.snippet = c.getString(7);
                theWords.offsets = c.getString(8);
                theWordsList.add(theWords);
            }
            c.close();
            return theWordsList;
        }

        private List<WordSmartJson.TheHitParade> selectTheHitParade(String word)
        {
            SQLiteDatabase db = getReadableDatabase();
            //String  q = "SELECT type,book,word,sense FROM TheHitParade WHERE word LIKE ? OR sense LIKE ? ORDER BY type,book,word";
            String  q = String.format(
//              "SELECT %s,%s,%s,%s FROM %s WHERE %s LIKE ? OR %s LIKE ? ORDER BY %s,%s,%s"
                "SELECT %s,%s,%s,%s FROM %s WHERE %s MATCH ? ORDER BY %s,%s,%s"
                ,
                WordSmartContract.TheHitParade.COLUMN_NAME_TYPE,
                WordSmartContract.TheHitParade.COLUMN_NAME_BOOK,
                WordSmartContract.TheHitParade.COLUMN_NAME_WORD,
                WordSmartContract.TheHitParade.COLUMN_NAME_SENSE,
                WordSmartContract.TheHitParade.TABLE_NAME,
                WordSmartContract.TheHitParade.TABLE_NAME,
//              WordSmartContract.TheHitParade.COLUMN_NAME_WORD,
//              WordSmartContract.TheHitParade.COLUMN_NAME_SENSE,
                WordSmartContract.TheHitParade.COLUMN_NAME_TYPE,
                WordSmartContract.TheHitParade.COLUMN_NAME_BOOK,
                WordSmartContract.TheHitParade.COLUMN_NAME_WORD
            );
//          String[] projection = {"%" + word + "%", "%" + word + "%"};
            String[] projection = {word};

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
            c.close();
            return theHitParadeList;
        }

        public RootListJson getRootWord(String rootMatchStr)
        {
            //Log.i(TAG, "rootMatchStr: |" + rootMatchStr + "|");
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
                ////"SELECT %s, %s, %s FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s WHERE %s LIKE ?) ORDER BY %s, %s"
                "SELECT %s,%s,%s FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s WHERE %s MATCH ?) ORDER BY %s, %s"
                ,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.COLUMN_NAME_SENSE,
                WordSmartContract.Root.COLUMN_NAME_WORD,
                WordSmartContract.Root.TABLE_NAME,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.TABLE_NAME,
                ////WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.TABLE_NAME,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.COLUMN_NAME_WORD
            );
            String[] projection = {rootMatchStr};

            Cursor c = db.rawQuery(q, projection);
            RootListJson rootListJson = new RootListJson();
            rootListJson.rootList = new ArrayList<WordSmartJson.Root>();
            String  rootStr = null;
            WordSmartJson.Root root = null;
            while (c.moveToNext()) {
                String r = c.getString(0);
                String s = c.getString(1);
                String w = c.getString(2);
                if (rootStr == null || !rootStr.equals(r)) {
                    rootStr = r;
                    root = new WordSmartJson.Root();
                    rootListJson.rootList.add(root);
                    root.root = r;
                    root.sense = s;
                    root.wordList = new ArrayList<String>();
                    root.wordList.add(w);
                } else if (rootStr.equals(r)) {
                    root.wordList.add(w);
                }
            }
            c.close();
            return rootListJson;
        }
        private List<WordSmartJson.Root> selectRoot(String word)
        {
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
//              "SELECT DISTINCT A.%s,A.%s,A.%s from %s AS A LEFT OUTER JOIN %s AS B ON A.%s = B.%s WHERE B.%s LIKE ? ORDER BY A.%s,A.%s"
                "SELECT %s, %s, %s FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s WHERE %s LIKE ?) ORDER BY %s, %s"
                ,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.COLUMN_NAME_SENSE,
                WordSmartContract.Root.COLUMN_NAME_WORD,
                WordSmartContract.Root.TABLE_NAME,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.TABLE_NAME,
                WordSmartContract.Root.COLUMN_NAME_WORD,
                WordSmartContract.Root.COLUMN_NAME_ROOT,
                WordSmartContract.Root.COLUMN_NAME_WORD
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
                    root.sense = s;
                    root.wordList = new ArrayList<String>();
                    root.wordList.add(w);
                } else if (rootStr.equals(r)) {
                    root.wordList.add(w);
                }
            }
            c.close();
            return rootList;
        }

        public WordSmartJson select(String word)
        {
            if (word == null)
                return null;
            WordSmartJson wordSmartJson = new WordSmartJson();
////        wordSmartJson.theWordsList = selectTheWords(word);
            wordSmartJson.theWordsList = selectTheWords(
                String.format(
                    "%s MATCH ?"
                    ,
                    WordSmartContract.TheWords.TABLE_NAME
                ),
                new String[] {word}
            );
            wordSmartJson.theHitParadeList = selectTheHitParade(word);
            wordSmartJson.rootList = selectRoot(word);
            return wordSmartJson;
        }
        public String selectJsonStr(String word)
        {
            return WordSmartContract.setJsonStr(select(word));
        }

        public List<WordSmartJson.TheWords> select(List<String> words)
        {
            return select(words, false);
        }
        public List<WordSmartJson.TheWords> select(List<String> words, boolean wordOnlyInCaseOfNotFound)
        {
            if (words == null || words.size() == 0)
                return null;
            List<WordSmartJson.TheWords> theWordsList = new ArrayList<WordSmartJson.TheWords>();
            for (String word: words) {
                List<WordSmartJson.TheWords> a = selectTheWords(
                    String.format(
                        "%s LIKE ? AND %s MATCH ?"  // to highlight word itself in an example
                        ,
                        WordSmartContract.TheWords.COLUMN_NAME_WORD,
                        WordSmartContract.TheWords.TABLE_NAME
                    ),
                    new String[] {word, word}
                );
                if (a.size() == 0) {
                    if (wordOnlyInCaseOfNotFound) {
                        WordSmartJson.TheWords theWords = new WordSmartJson.TheWords();
                        theWords.word = word;
                        theWordsList.add(theWords);
                    }
                } else {
                    for (WordSmartJson.TheWords w: a)
                        theWordsList.add(w);
                }
            }
            Collections.sort(
                theWordsList,
                new Comparator<WordSmartJson.TheWords>() {
                    @Override
                    public int compare(WordSmartJson.TheWords a, WordSmartJson.TheWords b) {
                        return a.word.toUpperCase().compareTo(b.word.toUpperCase());
                    }
                }
            );
            return theWordsList;
        }

        public List<WordSmartJson.TheWords> select(String bookStr, String unitStr)
        {
            List<WordSmartJson.TheWords> theWordsList = selectTheWords(
                String.format(
                    "%s = CAST(? AS INTEGER) AND %s = CAST(? AS INTEGER)"
                    ,
                    WordSmartContract.TheWords.COLUMN_NAME_BOOK_NO,
                    WordSmartContract.TheWords.COLUMN_NAME_UNIT
                ),
                new String[] {bookStr, unitStr}
            );
            return theWordsList;
        }

        public List<WordSmartJson.TheWords> selectByRootStr(String rootStr)
        {
            SQLiteDatabase db = getReadableDatabase();
            Cursor c = db.query(
                WordSmartContract.Root.TABLE_NAME,
                new String[] {WordSmartContract.Root.COLUMN_NAME_WORD},
                String.format(
                    "%s LIKE ?"
                    ,
                    WordSmartContract.Root.COLUMN_NAME_ROOT
                ),
                new String[] {rootStr},
                null,
                null,
                WordSmartContract.Root.COLUMN_NAME_WORD,
                null
            );
            List<String> words = new ArrayList<String>();
            while (c.moveToNext()) {
                words.add(c.getString(0));
            }
            c.close();
            return select(words, true);
        }
        public ArrayList<String> getRootList()
        {
            SQLiteDatabase db = getReadableDatabase();
            Cursor c = db.query(
                true,   // DISTINCT
                WordSmartContract.Root.TABLE_NAME,
                new String[] {
                    WordSmartContract.Root.COLUMN_NAME_ROOT,
                    WordSmartContract.Root.COLUMN_NAME_SENSE
                },
                null,
                null,
                null,
                null,
                "rowid",
                null
            );
            ArrayList<String> sal = new ArrayList<String>();
            while (c.moveToNext()) {
                sal.add(c.getString(0));
                sal.add(c.getString(1));
            }
            c.close();
            return sal;
        }
    }
}
