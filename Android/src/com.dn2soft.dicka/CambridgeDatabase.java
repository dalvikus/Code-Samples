package com.dn2soft.dicka;

import com.dn2soft.dickc.dictionary.Cambridge;
import com.dn2soft.util.Json;
import com.dn2soft.util.Joint;

import android.util.Log;

import android.content.Context;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;

import android.database.Cursor;
import java.util.List;
import java.util.ArrayList;
import android.content.ContentValues;

import java.io.File;
import java.io.IOException;

/*
 * see com.dn2soft.java.utility.Derby.java
 */
public final class CambridgeDatabase {
    private final static String TAG = "CambridgeDatabase";
    public static final String DATABASE_NAME = "Cambridge.db";

    // Inner class that defines the table contents
    static public class CambridgeOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public CambridgeOpenHelper(Context context, File rootPath) {
            super(context, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME), null, DATABASE_VERSION);
////        Log.i(TAG, "CambridgeOpenHelper(): " + (rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME)));
        }
        @Override
        public void onCreate(SQLiteDatabase db) {
            /* just use it */
        }
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            /* just use it */
        }

        private String[] getPageMore(String href)
        {
            SQLiteDatabase db = getReadableDatabase();
            String[] r = new String[2];

            // Define a projection that specifies which columns from the database
            // you will actually use after this query.
            String[] projection = {
                CambridgeContract.Web.COLUMN_NAME_PAGE,
                CambridgeContract.Web.COLUMN_NAME_MORE
            };
            // Define 'where' part of query.
            String selection = CambridgeContract.Web.COLUMN_NAME_HREF0 + " = ?";
            // Specify arguments in placeholder order.
            String[] selectionArgs = {href};
            // How you want the results sorted in the resulting Cursor

            Cursor c = db.query(
                CambridgeContract.Web.TABLE_NAME,  // The table to query
                projection,                         // The columns to return
                selection,                          // The columns for the WHERE clause
                selectionArgs,                      // The values for the WHERE clause
                null,                               // don't group the rows
                null,                               // don't filter by row groups
                null                                // The sort order
            );
            while (c.moveToNext()) {
                r[0] = c.getString(c.getColumnIndex(CambridgeContract.Web.COLUMN_NAME_PAGE));
                r[1] = c.getString(c.getColumnIndex(CambridgeContract.Web.COLUMN_NAME_MORE));
                break;
            }
            c.close();
            return r;
        }
        private List<String> getLinks(String href0)
        {
            List<String>    a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();

            // Define a projection that specifies which columns from the database
            // you will actually use after this query.
            String[] projection = {CambridgeContract.Link.COLUMN_NAME_HREF};
            // Define 'where' part of query.
            String selection = CambridgeContract.Link.COLUMN_NAME_HREF0 + " = ?";
            // Specify arguments in placeholder order.
            String[] selectionArgs = {href0};
            // How you want the results sorted in the resulting Cursor
            String sortOrder = CambridgeContract.Link._ID;

            Cursor c = db.query(
                CambridgeContract.Link.TABLE_NAME, // The table to query
                projection,                         // The columns to return
                selection,                          // The columns for the WHERE clause
                selectionArgs,                      // The values for the WHERE clause
                null,                               // don't group the rows
                null,                               // don't filter by row groups
                sortOrder                           // The sort order
            );
            while (c.moveToNext()) {
                a.add(c.getString(c.getColumnIndex(CambridgeContract.Link.COLUMN_NAME_HREF)));
            }
            c.close();
            return a;
        }
        public String select(String href0, boolean all)
        {
            List<String> page_a = new ArrayList<String>();
            String[] r = getPageMore(href0);
            if (r[0] == null) {
                return null;
            }
            page_a.add(String.format("{'href': '%s', 'content': %s, 'more': %s}", href0, r[0], r[1]));
            if (all) {
                for (String link: getLinks(href0)) {
                    r = getPageMore(link);
                    if (r[0] != null)
                        page_a.add(String.format("{'href': '%s', 'content': %s, 'more': %s}", link, r[0], r[1]));
                }
            }
            return String.format("[%s]", Joint.join(page_a, ","));
        }

        /**
         * get Web.href for "page" from given wordStr(query or href)
         *
         * if the page is saved in a database, it returns non-null,
         * otherwise it returns null
         */
        public String getSavedHref0(String wordStr)
        {
            boolean isURL = wordStr.startsWith("http://");
            String q;
            if (isURL)
                q = String.format("SELECT %s FROM %s WHERE %s = ?",
                    CambridgeContract.Web.COLUMN_NAME_HREF0, CambridgeContract.Web.TABLE_NAME, CambridgeContract.Web.COLUMN_NAME_HREF0);
            else
                q = String.format("SELECT %s FROM %s WHERE %s LIKE ? ORDER BY _id",
                    CambridgeContract.Vocabulary.COLUMN_NAME_HREF0, CambridgeContract.Vocabulary.TABLE_NAME, CambridgeContract.Vocabulary.COLUMN_NAME_WORD);
            String[] projection = {wordStr};
            String href0 = null;
            SQLiteDatabase db = getReadableDatabase();
            Cursor c = db.rawQuery(q, projection);
            while (c.moveToNext()) {
                if (isURL)
                    href0 = c.getString(c.getColumnIndex(CambridgeContract.Web.COLUMN_NAME_HREF0));
                else
                    href0 = c.getString(c.getColumnIndex(CambridgeContract.Vocabulary.COLUMN_NAME_HREF0));
                break;
            }
            c.close();
            return href0;
        }
        private String select0(String href0, boolean all)
        {
            List<String>    page_a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();
            String  q_one = "SELECT " + CambridgeContract.Web.COLUMN_NAME_PAGE + ", " + CambridgeContract.Web.COLUMN_NAME_MORE + " FROM " + CambridgeContract.Web.TABLE_NAME + " WHERE " + CambridgeContract.Web.COLUMN_NAME_HREF0 + " = ?";
            String  q_all = "SELECT " + (CambridgeContract.Link.TABLE_NAME+"."+CambridgeContract.Link.COLUMN_NAME_HREF) + ", " + CambridgeContract.Web.COLUMN_NAME_PAGE + ", " + CambridgeContract.Web.COLUMN_NAME_MORE + " FROM " + CambridgeContract.Link.TABLE_NAME + " LEFT OUTER JOIN " + CambridgeContract.Web.TABLE_NAME + " ON " + (CambridgeContract.Link.TABLE_NAME+"."+CambridgeContract.Link.COLUMN_NAME_HREF) + " = " + (CambridgeContract.Web.TABLE_NAME+"."+CambridgeContract.Web.COLUMN_NAME_HREF0) + " WHERE " + (CambridgeContract.Link.TABLE_NAME+"."+CambridgeContract.Link.COLUMN_NAME_HREF0) + " = ? ORDER BY " + (CambridgeContract.Link.TABLE_NAME+"."+CambridgeContract.Link._ID);
            //Log.i("DB", q_one);
            //Log.i("DB", q_all);
            String[] projection = {href0};

            Cursor c = db.rawQuery(all ? q_all : q_one, projection);
            while (c.moveToNext()) {
                page_a.add(
                    String.format("{'href': '%s', 'content': %s, 'more': %s}",
                    all ? c.getString(c.getColumnIndex(CambridgeContract.Link.COLUMN_NAME_HREF)) : href0,
                    c.getString(c.getColumnIndex(CambridgeContract.Web.COLUMN_NAME_PAGE)),
                    c.getString(c.getColumnIndex(CambridgeContract.Web.COLUMN_NAME_MORE))));
            }
            c.close();
            return page_a.isEmpty() ? null : String.format("[%s]", Joint.join(page_a, ","));
        }
        private void insert(final Cambridge.Result result, final boolean force)
        {
            String wordStr = result.wordStr.trim().replaceAll("\\s+", " ");
            boolean isURL = result.wordStr.startsWith("http://");
            boolean isNotAVariance = isURL; // !isURL case only

            SQLiteDatabase db = getWritableDatabase();

            db.beginTransaction();
            try {
                if (force) {
                    // not implemented yet
                }

                ContentValues values;
                for (int ipage = 0; ipage < result.doc.size(); ++ipage) {
                    Cambridge.Result.Page page = result.doc.get(ipage);
                    String  href0 = page.href;
                    values = new ContentValues();
                    values.put(CambridgeContract.Web.COLUMN_NAME_HREF0, href0);
                    values.put(CambridgeContract.Web.COLUMN_NAME_PAGE, page.contentJSONStr());
                    values.put(CambridgeContract.Web.COLUMN_NAME_MORE, page.more.getJSONStr());
                    long href0_id = db.insertWithOnConflict(
                        CambridgeContract.Web.TABLE_NAME,
                        null,
                        values,
                        SQLiteDatabase.CONFLICT_IGNORE
                    );

                    for (Cambridge.Block block: page.content) {
                        values = new ContentValues();
                        values.put(CambridgeContract.Vocabulary.COLUMN_NAME_WORD, block.head.word);
                        values.put(CambridgeContract.Vocabulary.COLUMN_NAME_HREF0, href0);
                        values.put(CambridgeContract.Vocabulary.COLUMN_NAME_WORDSTR, 0);
                        db.insertWithOnConflict(
                            CambridgeContract.Vocabulary.TABLE_NAME,
                            null,
                            values,
                            SQLiteDatabase.CONFLICT_IGNORE
                        );
                        if (ipage == 0 && !isURL && !isNotAVariance && wordStr.toUpperCase().equals(block.head.word.toUpperCase()))
                            isNotAVariance = true;
                    }
                    if (ipage == 0) {
                        if (!isNotAVariance) {
                            values = new ContentValues();
                            values.put(CambridgeContract.Vocabulary.COLUMN_NAME_WORD, wordStr);   // as it is
                            values.put(CambridgeContract.Vocabulary.COLUMN_NAME_HREF0, href0);
                            values.put(CambridgeContract.Vocabulary.COLUMN_NAME_WORDSTR, 1);
                            db.insertWithOnConflict(
                                CambridgeContract.Vocabulary.TABLE_NAME,
                                null,
                                values,
                                SQLiteDatabase.CONFLICT_IGNORE
                            );
                        }
                    }

                    if (ipage == 0) {
                        List<String>    links = new ArrayList<String>();
                        links.add(href0);   // for INNER OUTER JOIN (See select0)
                        for (Cambridge.More.Info info: result.doc.get(0).more.infoList)
                            links.add(info.href);
                        for (String link: links) {
                            values = new ContentValues();
                            values.put(CambridgeContract.Link.COLUMN_NAME_HREF0, href0);
                            values.put(CambridgeContract.Link.COLUMN_NAME_HREF, link);
                            db.insertWithOnConflict(
                                CambridgeContract.Link.TABLE_NAME,
                                null,
                                values,
                                SQLiteDatabase.CONFLICT_IGNORE
                            );
                        }
                    }
                }
                // all is done successfully!
                db.setTransactionSuccessful();
            } finally {
                db.endTransaction();
            }
        }


        /**
         *
         * ErrorMessageList != null     there is an exception
         * WarnMessage != null          not found
         * tryWordList != null          Try these words
         * otherwise                    result
         *  if (ErrorMessageList != null) {
         *      // there is an exception
         *      // e.printStackTrace() with ErrorMessageList
         *      return;
         * }
         * if (WarnMessage != null) {
         *      // not found
         *      return;
         * }
         * if (tryWordList != null) {
         *      // not found but
         *      // try these words
         *      return;
         * }
         * // show result
         *
         *
         * if savedHref0 is not null,
         * use it as href0, so there is no network inquiry
         */
        public Cambridge.Result lookup(String wordStr, String savedHref0)
        {
            Cambridge.Flag      flag = new Cambridge.Flag();
            flag.uk             = false;    // configurable
            flag.showAll        = false;    // configurable
            flag.force          = false;    // configurable
            flag.playPron       = true;     // always
            flag.showLink       = true;     // always
            flag.ansi           = false;    // always
            flag.verbose        = false;    // RELEASE: always, DEBUG: configurable
            flag.verbose        = false;    // RELEASE: always, DEBUG: configurable
            flag.showHref0      = false;    // RELEASE: always, DEBUG: configurable
            flag.showHref0Only  = false;    // RELEASE: always, DEBUG: configurable
            flag.followWord     = false;    // configurable
            flag.resultOnly     = true;     // always

            String href0 = null;
            if (savedHref0 == null) {
                try {
                    href0 = Cambridge.getHref0(wordStr, flag);
                } catch (IOException e) {
                    Cambridge.Result r = new Cambridge.Result();
                    r.wordStr = wordStr;
                    r.ErrorMessageList = Cambridge.getStackTrace(e);
                    return r;
                }
            } else
                href0 = savedHref0;
            if (href0 == null) {
                Cambridge.Result r = new Cambridge.Result();
                r.wordStr = wordStr;
                r.WarnMessage = wordStr + ": no luck";
                return r;
            }
            if (href0.contains("spellcheck")) {
                Cambridge.Result r = new Cambridge.Result();
                try {
                    r.wordStr = wordStr;
                    r.tryWordList = Cambridge.spellCheck(href0);
                } catch (IOException e) {
                    r.ErrorMessageList = Cambridge.getStackTrace(e);
                }
                return r;
            }
            // sanitize href0
            int index = href0.indexOf("?"); // index == -1: URL given, index > -1: word given
            href0 = index == -1 ? href0 : href0.substring(0, index);

            // try to get info from database with href0
            String jsonStr = select0(href0, flag.showAll);
            //Log.i("DB", jsonStr == null ? "null" : jsonStr);

            Cambridge.Result    result = null;
            int nAddedPage = 0;
            if (jsonStr == null) {
                try {
                    result = Cambridge.getResultFromWeb(href0, flag);
                } catch (IOException e) {
                    Cambridge.Result r = new Cambridge.Result();
                    r.wordStr = wordStr;
                    r.ErrorMessageList = Cambridge.getStackTrace(e);
                    return r;
                }
            } else {
                Json.BaseType  json = Json.setJson(jsonStr);
                result = new Cambridge.Result();
                result.setDoc(json, flag.showAll);
                for (Cambridge.Result.Page page: result.doc) {
                    if (page.content.size() == 0) { // not filled yet, i.e., no record in "Web" table
                        try {
                            ++nAddedPage;
                            Cambridge.lookup0(flag, page);
                        } catch (IOException e) {
                            Cambridge.Result r = new Cambridge.Result();
                            r.wordStr = wordStr;
                            r.ErrorMessageList = Cambridge.getStackTrace(e);
                            return r;
                        }
                    }
                }
            }
            result.wordStr = wordStr;
            if (jsonStr == null || nAddedPage > 0) {
                //Log.i("DB", "insert...");
                insert(result, flag.force);
            }
            return result;
        }
    }
}
