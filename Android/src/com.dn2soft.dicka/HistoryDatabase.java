package com.dn2soft.dicka;

import java.util.ArrayList;
import com.dn2soft.dicka.HistoryContract.TableHistory;

import android.content.Context;
import java.io.File;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.Cursor;

import android.util.Log;

public final class HistoryDatabase {
    private static final String TAG = "HistoryDatabase";

    public static final String DATABASE_NAME = "history.db";

    public static class HistoryOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public HistoryOpenHelper(Context context, File rootPath) {
            super(context, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME), null, DATABASE_VERSION);
////        Log.i(TAG, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME));
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            /* just use it */
        }
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            /* just use it */
        }

        public ArrayList<String> select()
        {
            SQLiteDatabase db = getReadableDatabase();
            String q;
            Cursor c;
/*
            q = String.format(
                "SELECT %s, DATETIME(%s) FROM %s ORDER BY DATETIME(%s) DESC LIMIT %d"
                ,
                TableHistory.COLUMN_NAME_WORD,
                TableHistory.COLUMN_NAME_DATETIME,
                TableHistory.TABLE_NAME,
                TableHistory.COLUMN_NAME_DATETIME,
                TableHistory.LIMIT
            );
            c = db.rawQuery(q, null);
            while (c.moveToNext()) {
                Log.i(TAG, c.getString(0) + ", " + c.getString(1));
            }
 */
            q = String.format(
                "SELECT DISTINCT %s FROM %s ORDER BY DATETIME(%s) DESC LIMIT %d"
                ,
                TableHistory.COLUMN_NAME_WORD,
                TableHistory.TABLE_NAME,
                TableHistory.COLUMN_NAME_DATETIME,
                TableHistory.LIMIT
            );
            c = db.rawQuery(q, null);
            if (c == null) {    // an error occurred
                Log.e(TAG, "query: |" + q + "|");
                Log.e(TAG, "An error occurred");
                return null;
            }
            ArrayList<String> a = new ArrayList<String>();
            while (c.moveToNext())
                a.add(c.getString(0));
            c.close();
            return a;
        }

        public void insert(String wordStr)
        {
            SQLiteDatabase db = getReadableDatabase();
            db.execSQL(String.format(
                "INSERT INTO %s VALUES ('%s', DATETIME('NOW'))"
                ,
                TableHistory.TABLE_NAME,
                wordStr
            ));
/*
            ContentValues values = new ContentValues();
            values.put(TableHistory.COLUMN_NAME_WORD, wordStr);
            values.put(TableHistory.COLUMN_NAME_DATETIME, TableHistory.NOW);
            db.insertWithOnConflict(
                TableHistory.TABLE_NAME,
                null,
                values,
                SQLiteDatabase.CONFLICT_IGNORE
            );
 */
        }
    }
}
