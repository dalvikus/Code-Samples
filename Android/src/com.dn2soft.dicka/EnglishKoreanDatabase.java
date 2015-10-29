package com.dn2soft.dicka;

import com.dn2soft.dicka.EnglishKoreanContract.English_English0;
import com.dn2soft.dicka.EnglishKoreanContract.English_Korean;
import com.dn2soft.dicka.EnglishKoreanContract.EnglishKoreanJson;

import android.content.Context;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.Cursor;

import java.util.ArrayList;
import java.io.File;

import android.util.Log;

public final class EnglishKoreanDatabase {
    private static final String TAG = "EnglishKoreanDatabase";

    public static final String DATABASE_NAME = "EnglishKorean.db";

    public static class EnglishKoreanOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public EnglishKoreanOpenHelper(Context context, File rootPath) {
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

        public EnglishKoreanJson select(String word)
        {
            if (word == null)
                return null;
            SQLiteDatabase db = getReadableDatabase();
            // SELECT word, sense FROM English_Korean WHERE word IN (SELECT word FROM English_English0 WHERE English_English0 MATCH ?) ORDER BY word
            String  q = String.format(
                "SELECT %s, %s FROM %s WHERE %s IN (SELECT %s FROM %s WHERE %s MATCH ?) ORDER BY %s"
                ,
                English_Korean.COLUMN_NAME_WORD,
                English_Korean.COLUMN_NAME_SENSE,
                English_Korean.TABLE_NAME,
                English_Korean.COLUMN_NAME_WORD,
                English_English0.COLUMN_NAME_WORD,
                English_English0.TABLE_NAME,
                English_English0.TABLE_NAME,
                English_Korean.COLUMN_NAME_WORD
            );
            String[] projection = {word};

            Cursor c = null;
            try {
                c = db.rawQuery(q, projection);
            } catch (SQLiteException e) {e.printStackTrace();}
            if (c == null)
                return null;
            EnglishKoreanJson englishKoreanJson = new EnglishKoreanJson();
            englishKoreanJson.englishKoreanList = new ArrayList<EnglishKoreanJson.EnglishKorean>();
            while (c.moveToNext()) {
                EnglishKoreanJson.EnglishKorean englishKorean = new EnglishKoreanJson.EnglishKorean();
                englishKoreanJson.englishKoreanList.add(englishKorean);
                englishKorean.word = c.getString(c.getColumnIndex(English_Korean.COLUMN_NAME_WORD));
                englishKorean.sense = c.getString(c.getColumnIndex(English_Korean.COLUMN_NAME_SENSE));
            }
            c.close();
            return englishKoreanJson;
        }
        public String selectJsonStr(String word)
        {
            return EnglishKoreanContract.setJsonStr(select(word));
        }
    }
}
