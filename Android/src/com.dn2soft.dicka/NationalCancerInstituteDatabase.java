package com.dn2soft.dicka;

import com.dn2soft.dicka.NationalCancerInstituteContract.NationalCancerInstituteJson;

import java.util.ArrayList;
import android.util.Log;

import android.content.Context;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.Cursor;
import java.io.File;

/*
 * see com.dn2soft.db.Derby.java
 */
public final class NationalCancerInstituteDatabase {
    private static final String TAG = "NationalCancerInstituteDatabase";
    public static final String DATABASE_NAME = "NationalCancerInstitute.db";

    public static class NationalCancerInstituteOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public NationalCancerInstituteOpenHelper(Context context, File rootPath) {
            super(context, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME), null, DATABASE_VERSION);
////        Log.i(TAG, "NationalCancerInstituteOpenHelper(): " + (rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME)));
        }
        @Override
        public void onCreate(SQLiteDatabase db) {
            /* just use it */
        }
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            /* just use it */
        }

        public NationalCancerInstituteJson select(String termsStr)
        {
            if (termsStr == null)
                return null;
            SQLiteDatabase db = getReadableDatabase();
            boolean termsOnly = termsStr.startsWith("!");
            String query = termsOnly ? termsStr.substring(1).trim() : termsStr;
            String  q = String.format(
                "SELECT %s,%s,%s,%s,%s,snippet(%s, '<<', '>>', '...', -1, -5) FROM %s WHERE %s MATCH ? ORDER BY %s,%s LIMIT %d"
                ,
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_SRC,
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_TERMS,
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_MP3,
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_MTD,
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_DEF,
                NationalCancerInstituteContract.TermsDef.TABLE_NAME, 
                NationalCancerInstituteContract.TermsDef.TABLE_NAME, 
                termsOnly ? NationalCancerInstituteContract.TermsDef.COLUMN_NAME_TERMS : NationalCancerInstituteContract.TermsDef.TABLE_NAME, 
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_SRC,
                NationalCancerInstituteContract.TermsDef.COLUMN_NAME_TERMS,
                NationalCancerInstituteContract.TermsDef.LIMIT + 1
            );
            String[] projection = {query};
            Cursor c = null;
            try {
                c = db.rawQuery(q, projection);
            } catch (SQLiteException e) {e.printStackTrace();}
            if (c == null)
                return null;
            NationalCancerInstituteJson nationalCancerInstituteJson = new NationalCancerInstituteJson();
            nationalCancerInstituteJson.termsDefList = new ArrayList<NationalCancerInstituteJson.TermsDef>();
            while (c.moveToNext()) {
                NationalCancerInstituteJson.TermsDef termsDef = new NationalCancerInstituteJson.TermsDef();
                nationalCancerInstituteJson.termsDefList.add(termsDef);
                termsDef.src = String.format("%d", c.getInt(0));
                termsDef.terms = c.getString(1);
                termsDef.mp3 = c.getString(2);
                termsDef.mtd = c.getString(3);
                termsDef.def = c.getString(4);
                termsDef.snippet = c.getString(5);
            }
            c.close();
            return nationalCancerInstituteJson;
        }
    }
}
