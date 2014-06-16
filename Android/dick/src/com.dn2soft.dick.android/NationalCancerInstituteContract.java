package com.dn2soft.dick.android;

import android.util.Log;

import android.content.Context;
import android.provider.BaseColumns;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;

import com.dn2soft.dick.dictionary.Cambridge;
import android.database.Cursor;
import com.dn2soft.dick.utility.Jsonable;
import com.dn2soft.dick.utility.Json;
import com.dn2soft.dick.utility.JsonWalk;
import com.dn2soft.dick.utility.Joint;
import java.util.List;
import java.util.ArrayList;
import android.content.ContentValues;

import java.io.File;

/*
 * see com.dn2soft.java.utility.Derby.java
 */
public final class NationalCancerInstituteContract {
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private NationalCancerInstituteContract() {}

    // Inner class that defines the table contents
    public static abstract class TermsDef {
        public static final String TABLE_NAME = "TermsDef";
        public static final String COLUMN_NAME_SRC = "src";
        public static final String COLUMN_NAME_TERMS = "terms";
        public static final String COLUMN_NAME_MP3 = "mp3";
        public static final String COLUMN_NAME_MTD = "mtd";
        public static final String COLUMN_NAME_DEF = "def";

/*
CREATE TABLE TermsDef (
    src     SMALLINT        NOT NULL,   -- 1: genetics, 2: cancer
    terms   VARCHAR(128)    NOT NULL,   -- can be duplicated
    mp3     VARCHAR(16)     NOT NULL,   -- "": None
    mtd     VARCHAR(128)    NOT NULL,   -- "": None
    def     VARCHAR(4096)   NOT NULL,

    UNIQUE(src, terms, def)
);
CREATE INDEX IDX_TermsDef_terms ON TermsDef(terms);
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
    public static class NationalCancerInstituteJson implements Jsonable
    {
        public static class TermsDef implements Jsonable
        {
            public String src;
            public String terms;
            public String mp3;
            public String mtd;
            public String def;
        }

        public List<TermsDef> termsDefList;
    }

    static public class NationalCancerInstituteDbHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;
        public static final String DATABASE_NAME = "NationalCancerInstitute.db";

        public NationalCancerInstituteDbHelper(Context context, File rootPath) {
//          super(context, DATABASE_NAME, null, DATABASE_VERSION);
//          super(context, context.getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS) == null ? DATABASE_NAME : (context.getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath() + "/dick/" + DATABASE_NAME), null, DATABASE_VERSION);
            super(context, rootPath == null ? DATABASE_NAME : (rootPath.getAbsolutePath() + "/" + DATABASE_NAME), null, DATABASE_VERSION);
        }
        public void onCreate(SQLiteDatabase db) {
            /* just use it */
            /*
            db.execSQL(Web.SQL_CREATE);
            db.execSQL(Vocabulary.SQL_CREATE);
            db.execSQL(Link.SQL_CREATE);
             */
        }
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            /* just use it */

            /*
            db.execSQL(Web.SQL_DELETE);
            db.execSQL(Vocabulary.SQL_DELETE);
            db.execSQL(Link.SQL_DELETE);
            onCreate(db);
             */
        }
        public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            onUpgrade(db, oldVersion, newVersion);
        }

        public List<String> getTerms(String termsStr)
        {
            //String q = "SELECT DISTINCT terms FROM TermsDef WHERE terms LIKE ? ORDER BY terms";
            String q = String.format("SELECT DISTINCT %s FROM %s WHERE %s LIKE ? ORDER BY %s",
                TermsDef.COLUMN_NAME_TERMS,
                TermsDef.TABLE_NAME,
                TermsDef.COLUMN_NAME_TERMS,
                TermsDef.COLUMN_NAME_TERMS
            );
            SQLiteDatabase db = getReadableDatabase();
            String[] projection = {termsStr};
            Cursor c = db.rawQuery(q, projection);
            List<String> ra = new ArrayList<String>();
            while (c.moveToNext()) {
                ra.add(c.getString(c.getColumnIndex(TermsDef.COLUMN_NAME_TERMS)));
            }
            return ra;
        }

        public String select(String termsStr)
        throws ClassNotFoundException, IllegalAccessException
        {
            SQLiteDatabase db = getReadableDatabase();
            // Define a projection that specifies which columns from the database
            // you will actually use after this query.
            String[] projection = {
                TermsDef.COLUMN_NAME_SRC,
                TermsDef.COLUMN_NAME_TERMS,
                TermsDef.COLUMN_NAME_MP3,
                TermsDef.COLUMN_NAME_MTD,
                TermsDef.COLUMN_NAME_DEF
            };
            // Define 'where' part of query.
            String selection = TermsDef.COLUMN_NAME_TERMS + " = ?";
            // Specify arguments in placeholder order.
            String[] selectionArgs = {termsStr};
            // How you want the results sorted in the resulting Cursor
            String sortOrder = TermsDef.COLUMN_NAME_SRC;

            Cursor c = db.query(
                TermsDef.TABLE_NAME,        // The table to query
                projection,                         // The columns to return
                selection,                          // The columns for the WHERE clause
                selectionArgs,                      // The values for the WHERE clause
                null,                               // don't group the rows
                null,                               // don't filter by row groups
                sortOrder                           // The sort order
            );
            NationalCancerInstituteJson nationalCancerInstituteJson = new NationalCancerInstituteJson();
            nationalCancerInstituteJson.termsDefList = new ArrayList<NationalCancerInstituteJson.TermsDef>();
            while (c.moveToNext()) {
                NationalCancerInstituteJson.TermsDef termsDef = new NationalCancerInstituteJson.TermsDef();
                nationalCancerInstituteJson.termsDefList.add(termsDef);
                termsDef.src = String.format("%d", c.getInt(c.getColumnIndex(TermsDef.COLUMN_NAME_SRC)));
                termsDef.terms = c.getString(c.getColumnIndex(TermsDef.COLUMN_NAME_TERMS));
                termsDef.mp3 = c.getString(c.getColumnIndex(TermsDef.COLUMN_NAME_MP3));
                termsDef.mtd = c.getString(c.getColumnIndex(TermsDef.COLUMN_NAME_MTD));
                termsDef.def = c.getString(c.getColumnIndex(TermsDef.COLUMN_NAME_DEF));
            }
            return JsonWalk.walkClass(NationalCancerInstituteJson.class, nationalCancerInstituteJson, "    ");
        }
    }
}
