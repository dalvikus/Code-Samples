package com.dn2soft.dicka;

import android.app.SearchManager;
import com.dn2soft.dicka.VocabularyContract.Vocabulary;

import android.content.Context;
import java.io.File;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.Cursor;

import android.util.Log;

public final class VocabularyDatabase {
    private static final String TAG = "VocabularyDatabase";

    public static final String DATABASE_NAME = "vocab.db";

    public static class VocabularyOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public VocabularyOpenHelper(Context context, File rootPath) {
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

        public Cursor getSuggestions(String selection, String[] projection)
        {
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
//              "SELECT rowid AS _id, %s AS %s, rowid AS %s, %s AS %s, %s AS %s FROM %s WHERE %s LIKE ? ORDER BY %s"
                "SELECT rowid AS _id, %s AS %s, rowid AS %s, %s AS %s, %s AS %s FROM %s WHERE " + selection + " ORDER BY %s"
                ,
                Vocabulary.COLUMN_NAME_WORD,
                SearchManager.SUGGEST_COLUMN_TEXT_1,
                SearchManager.SUGGEST_COLUMN_INTENT_DATA_ID,
                Vocabulary.COLUMN_NAME_WORD,
                SearchManager.SUGGEST_COLUMN_INTENT_EXTRA_DATA,
                Vocabulary.COLUMN_NAME_WORD,
                SearchManager.SUGGEST_COLUMN_QUERY,
                Vocabulary.TABLE_NAME,
                Vocabulary.COLUMN_NAME_WORD
            );
////        Log.i(TAG, "query: |" + q + "| with |" + query + "|");

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
                String word = c.getString(0);
                Log.i(TAG, "word: |" + word + "|");
            }
            c.moveToFirst();
 */
            return c;
        }
    }
}
