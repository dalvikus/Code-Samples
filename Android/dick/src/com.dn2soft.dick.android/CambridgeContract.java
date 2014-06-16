package com.dn2soft.dick.android;

import android.util.Log;

import android.content.Context;
import android.provider.BaseColumns;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;

import com.dn2soft.dick.dictionary.Cambridge;
import android.database.Cursor;
import com.dn2soft.dick.utility.Joint;
import java.util.List;
import java.util.ArrayList;
import android.content.ContentValues;

import java.io.File;

/*
 * see com.dn2soft.java.utility.Derby.java
 */
public final class CambridgeContract {
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private CambridgeContract() {}

    // Inner class that defines the table contents
    public static abstract class Web {
        public static final String TABLE_NAME = "Web";
        public static final String COLUMN_NAME_HREF0 = "href0";
        public static final String COLUMN_NAME_PAGE = "page";
        public static final String COLUMN_NAME_MORE = "more";

/*
CREATE TABLE Web (
    href0   VARCHAR(128)    NOT NULL,
    page    VARCHAR(8192)   NOT NULL,   -- Page::getJSONStr()
    more    VARCHAR(4096),              -- More::getJSONStr()
                                        --      NULL:   not retrieved yet
                                        --      "":     none

    UNIQUE(href0)
);
 */
/*
        public static final String SQL_CREATE = String.format(
            "CREATE TABLE %s (\n" +
            "    %s VARCHAR(128) NOT NULL,\n" +
            "    %s VARCHAR(8192) NOT NULL,\n" +
            "    %s VARCHAR(4096),\n" +
            "\n" +
            "    UNIQUE(%s)\n" +
            ")"
            ,
            TABLE_NAME,
            COLUMN_NAME_HREF0,
            COLUMN_NAME_PAGE,
            COLUMN_NAME_MORE,
            COLUMN_NAME_HREF0
        );
 */
        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }
    public static abstract class Vocabulary implements BaseColumns {
        public static final String TABLE_NAME = "Vocabulary";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_HREF0 = "href0";
        public static final String COLUMN_NAME_WORDSTR = "wordStr";

/*
CREATE TABLE Vocabulary (
    id      INT GENERATED ALWAYS AS IDENTITY,
--  id      INTEGER PRIMARY KEY,
    word    VARCHAR(64)     NOT NULL,
    href0   VARCHAR(128)    NOT NULL,

    UNIQUE(word, href0)
----------------------------------------
--  ABASH: not found
--          abashed
--  ABSCISSION: NOT FOUND
--  AGGRANDIZE: not found
--          aggrandizement
--  AGGRIEVE: not found
--          aggrieved
--  ANTIPODAL: NOT FOUND
--  AUSPICE: not found
--          auspices
--  BACCHANAL: not found
--          bacchanalian
--  BEDIZEN: NOT FOUND
--  BELEAGUER: not found
--          beleaguered
--  BLANDISH: not found
--          blandishments
--  BLANDISHMENT: not found
--          blandishments
--  CALUMNIATE: not found
--          calumny
--  CASTIGATION: not found
--          castigate
--  CHURL: not found
--          churlish
--  CLOY: not found
--          cloying
--  CONTEMN: NOT FOUND
--  CONTUMELY: NOT FOUND
--  CORRIGIBLE: not found
--          incorrigible
--  COZEN: NOT FOUND
--  DEPREDATE: not found
--          depredation
--  DESICCATE: not found
--          desiccated
--  DESUETUDE: NOT FOUND
--  DETRACTION: not found
--          detract
--  DISAFFECT: not found
--          disaffected
--  DISGRUNTLE: not found
--          disgruntled
--  DISPIRIT: not found
--          dispirited
--  DISTRAIT: NOT FOUND
--  EMACIATE: not found
--          emancipated
--  ENCOMIUM: NOT FOUND
--  ESTRANGE: not found
--          estranged
--  FOREBODE: not founD
--          foreboding
--  FROWARD: NOT FOUND
--  HABITUATE: not found
--          habituated
--  HALLOW: not found
--          hallowed
--  IDOLATROUS: not found
--          idolatry
--  INELUCTABLE: NOT FOUND
--  ITINERATE: not found
--          itinerant
--          itinerary
--  LABILE: NOT FOUND
--  LIMN: NOT FOUND
--  MACHINATION: not found
--          machinations
--  MAUNDER: NOT FOUND
--  METTLESOME: not found
--          mettle
--  MINATORY: NOT FOUND
--  NOSTRUM: NOT FOUND
--  PALLIATE: not found
--          palliative
--  PENURIOUS: not found
--          penury
--  PICARESQUE: NOT FOUND
--  PLANGENT: NOT FOUND
--  PREPOSSESS: not found
--          prepossessing
--  PROPINQUITY: NOT FOUND
--  PURBLIND: NOT FOUND
--  QUIESCENCE: not found
--          quiescent
--  RAMIFY: not found
--          ramification
--  RAREFY: not found
--          rarefied
--  RATIOCINATION: NOT FOUND
--  REBUS: NOT FOUND
--  REFULGENT: NOT FOUND
--  SANCTIMONY: not found
--          sanctimonious
--  SEINE: NOT FOUND
--  SERE: NOT FOUND
--  SPLENETIC: not found
--      splenic
--  SUPPLIANT: NOT FOUND
--  TUMID: NOT FOUND
--  UNFEIGNED: not found
--          feigned
--  VAGARY: not found
--          vagaries
--  VAUNT: not found
--          vaunted
--  VICISSITUDE: not found
--          vicissitudes
--  VITUPERATE: not found
--          vituperative
----------------------------------------
);
 */
/*
        public static final String SQL_CREATE = String.format(
            "CREATE TABLE %s (\n" +
            "    %s INTEGER PRIMARY KEY,\n" +
            "    %s VARCHAR(64) NOT NULL,\n" +
            "    %s VARCHAR(128) NOT NULL,\n" +
            "\n" +
            "    UNIQUE(%s, %s)\n" +
            ")"
            ,
            TABLE_NAME,
            _ID,
            COLUMN_NAME_WORD,
            COLUMN_NAME_HREF0,
            COLUMN_NAME_WORD,
            COLUMN_NAME_HREF0
        );
 */
        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }
    public static abstract class Link implements BaseColumns {
        public static final String TABLE_NAME = "Link";
        public static final String COLUMN_NAME_HREF0 = "href0";
        public static final String COLUMN_NAME_HREF = "href";

/*
CREATE TABLE Link (
    id      INT GENERATED ALWAYS AS IDENTITY,
--  id      INTEGER PRIMARY KEY,
    href0   VARCHAR(128)    NOT NULL,
    href    VARCHAR(128)    NOT NULL,

    UNIQUE (href0, href)--,
--  FOREIGN KEY (href) REFERENCES Web (href0),
--  FOREIGN KEY (href0) REFERENCES Web (href0)
);
 */
/*
        public static final String SQL_CREATE = String.format(
            "CREATE TABLE %s (\n" +
            "    %s INTEGER PRIMARY KEY,\n" +
            "    %s VARCHAR(128) NOT NULL,\n" +
            "    %s VARCHAR(128) NOT NULL,\n" +
            "\n" +
            "    UNIQUE(%s, %s)\n" +
            ")"
            ,
            TABLE_NAME,
            _ID,
            COLUMN_NAME_HREF0,
            COLUMN_NAME_HREF,
            COLUMN_NAME_HREF0,
            COLUMN_NAME_HREF
        );
 */
        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }
/*
    public static abstract class Audio {
        public static final String TABLE_NAME = "Audio";
        public static final String COLUMN_NAME_AUDIO_URL = "audio_url";
        public static final String COLUMN_NAME_AUDIO_BINARY = "audio_binary";

        public static final String SQL_CREATE = String.format(
            "CREATE TABLE %s (\n" +
            "    %s VARCHAR(128) NOT NULL,\n" +
            "    %s BLOB NOT NULL,\n" +
            "\n" +
            "    UNIQUE(%s)\n" +
            ")"
            ,
            TABLE_NAME,
            COLUMN_NAME_AUDIO_URL,
            COLUMN_NAME_AUDIO_BINARY,
            COLUMN_NAME_AUDIO_URL
        );
        public static final String SQL_DELETE =
            "DROP TABLE IF EXISTS " + TABLE_NAME;
    }
 */

    static public class CambridgeDbHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;
        public static final String DATABASE_NAME = "CambridgeDictionary.db";

        public CambridgeDbHelper(Context context, File rootPath) {
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

        private String[] getPageMore(String href)
        {
            SQLiteDatabase db = getReadableDatabase();
            String[] r = new String[2];

            // Define a projection that specifies which columns from the database
            // you will actually use after this query.
            String[] projection = {
                Web.COLUMN_NAME_PAGE,
                Web.COLUMN_NAME_MORE
            };
            // Define 'where' part of query.
            String selection = Web.COLUMN_NAME_HREF0 + " = ?";
            // Specify arguments in placeholder order.
            String[] selectionArgs = {href};
            // How you want the results sorted in the resulting Cursor

            Cursor c = db.query(
                Web.TABLE_NAME,  // The table to query
                projection,                         // The columns to return
                selection,                          // The columns for the WHERE clause
                selectionArgs,                      // The values for the WHERE clause
                null,                               // don't group the rows
                null,                               // don't filter by row groups
                null                                // The sort order
            );
            while (c.moveToNext()) {
                r[0] = c.getString(c.getColumnIndex(Web.COLUMN_NAME_PAGE));
                r[1] = c.getString(c.getColumnIndex(Web.COLUMN_NAME_MORE));
                break;
            }
            return r;
        }
        private List<String> getLinks(String href0)
        {
            List<String>    a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();

            // Define a projection that specifies which columns from the database
            // you will actually use after this query.
            String[] projection = {Link.COLUMN_NAME_HREF};
            // Define 'where' part of query.
            String selection = Link.COLUMN_NAME_HREF0 + " = ?";
            // Specify arguments in placeholder order.
            String[] selectionArgs = {href0};
            // How you want the results sorted in the resulting Cursor
            String sortOrder = Link._ID;

            Cursor c = db.query(
                Link.TABLE_NAME, // The table to query
                projection,                         // The columns to return
                selection,                          // The columns for the WHERE clause
                selectionArgs,                      // The values for the WHERE clause
                null,                               // don't group the rows
                null,                               // don't filter by row groups
                sortOrder                           // The sort order
            );
            while (c.moveToNext()) {
                a.add(c.getString(c.getColumnIndex(Link.COLUMN_NAME_HREF)));
            }
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
        public String href0(String wordStr)
        {
            boolean isURL = wordStr.startsWith("http://");
            String q;
            if (isURL)
                q = String.format("SELECT %s FROM %s WHERE %s = ?",
                    Web.COLUMN_NAME_HREF0, Web.TABLE_NAME, Web.COLUMN_NAME_HREF0);
            else
                q = String.format("SELECT %s FROM %s WHERE %s LIKE ? ORDER BY _id",
                    Vocabulary.COLUMN_NAME_HREF0, Vocabulary.TABLE_NAME, Vocabulary.COLUMN_NAME_WORD);
            String[] projection = {wordStr};
            String href0 = null;
            SQLiteDatabase db = getReadableDatabase();
            Cursor c = db.rawQuery(q, projection);
            while (c.moveToNext()) {
                if (isURL)
                    href0 = c.getString(c.getColumnIndex(Web.COLUMN_NAME_HREF0));
                else
                    href0 = c.getString(c.getColumnIndex(Vocabulary.COLUMN_NAME_HREF0));
                break;
            }
            return href0;
        }
        public String select0(String href0, boolean all)
        {
            List<String>    page_a = new ArrayList<String>();
            SQLiteDatabase db = getReadableDatabase();
            String  q_one = "SELECT " + Web.COLUMN_NAME_PAGE + ", " + Web.COLUMN_NAME_MORE + " FROM " + Web.TABLE_NAME + " WHERE " + Web.COLUMN_NAME_HREF0 + " = ?";
            String  q_all = "SELECT " + (Link.TABLE_NAME+"."+Link.COLUMN_NAME_HREF) + ", " + Web.COLUMN_NAME_PAGE + ", " + Web.COLUMN_NAME_MORE + " FROM " + Link.TABLE_NAME + " LEFT OUTER JOIN " + Web.TABLE_NAME + " ON " + (Link.TABLE_NAME+"."+Link.COLUMN_NAME_HREF) + " = " + (Web.TABLE_NAME+"."+Web.COLUMN_NAME_HREF0) + " WHERE " + (Link.TABLE_NAME+"."+Link.COLUMN_NAME_HREF0) + " = ? ORDER BY " + (Link.TABLE_NAME+"."+Link._ID);
            //Log.i("DB", q_one);
            //Log.i("DB", q_all);
            String[] projection = {href0};

            Cursor c = db.rawQuery(all ? q_all : q_one, projection);
            while (c.moveToNext()) {
                page_a.add(
                    String.format("{'href': '%s', 'content': %s, 'more': %s}",
                    all ? c.getString(c.getColumnIndex(Link.COLUMN_NAME_HREF)) : href0,
                    c.getString(c.getColumnIndex(Web.COLUMN_NAME_PAGE)),
                    c.getString(c.getColumnIndex(Web.COLUMN_NAME_MORE))));
            }
            return page_a.isEmpty() ? null : String.format("[%s]", Joint.join(page_a, ","));
        }
        public void insert(final Cambridge.Result result, final boolean force)
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
                    values.put(Web.COLUMN_NAME_HREF0, href0);
                    values.put(Web.COLUMN_NAME_PAGE, page.contentJSONStr());
                    values.put(Web.COLUMN_NAME_MORE, page.more.getJSONStr());
                    long href0_id = db.insertWithOnConflict(
                        Web.TABLE_NAME,
                        null,
                        values,
                        SQLiteDatabase.CONFLICT_IGNORE
                    );

                    for (Cambridge.Block block: page.content) {
                        values = new ContentValues();
                        values.put(Vocabulary.COLUMN_NAME_WORD, block.head.word);
                        values.put(Vocabulary.COLUMN_NAME_HREF0, href0);
                        values.put(Vocabulary.COLUMN_NAME_WORDSTR, 0);
                        db.insertWithOnConflict(
                            Vocabulary.TABLE_NAME,
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
                            values.put(Vocabulary.COLUMN_NAME_WORD, wordStr);   // as it is
                            values.put(Vocabulary.COLUMN_NAME_HREF0, href0);
                            values.put(Vocabulary.COLUMN_NAME_WORDSTR, 1);
                            db.insertWithOnConflict(
                                Vocabulary.TABLE_NAME,
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
                            values.put(Link.COLUMN_NAME_HREF0, href0);
                            values.put(Link.COLUMN_NAME_HREF, link);
                            db.insertWithOnConflict(
                                Link.TABLE_NAME,
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
/*
        public byte[] getOrDownloadIfNecessary(String audioUrl) {
Log.i("AUDIO", audioUrl);
            // search
            SQLiteDatabase db = getReadableDatabase();
            String[] projection = {Audio.COLUMN_NAME_AUDIO_BINARY};
            String selection = Audio.COLUMN_NAME_AUDIO_URL + " = ?";
            String[] selectionArgs = {audioUrl};
            Cursor c = db.query(
                Audio.TABLE_NAME,
                projection,
                selection,
                selectionArgs,
                null,
                null,
                null
            );
            byte[] blob = null;
            while (c.moveToNext()) {
                blob = c.getBlob(c.getColumnIndex(Audio.COLUMN_NAME_AUDIO_BINARY));
                break;
            }
            if (blob != null) {
Log.i("AUDIO", String.format("%d", blob.length));
                return blob;
            }

            // download
            URL audioURL = null;
            try {
                audioURL = new URL(audioUrl);
            } catch (java.net.MalformedURLException e) {
                e.printStackTrace();
                return null;
            }
            try {
                URLConnection ucon = audioURL.openConnection();
                InputStream is = ucon.getInputStream();
                BufferedInputStream bis = new BufferedInputStream(is);
                ByteArrayBuffer baf = new ByteArrayBuffer(500);
                int current = 0;
                while ((current = bis.read()) != -1) {
                    baf.append((byte) current);
                }
                blob = baf.toByteArray();
Log.i("AUDIO", String.format("%d", blob.length));
            } catch (java.io.IOException e) {
                e.printStackTrace();
                blob = null;
            }
            if (blob == null) {
Log.i("AUDIO", "no blob");
                return null;
            }

            // insert
            db = getWritableDatabase();
            ContentValues values;
            values = new ContentValues();
            values.put(Audio.COLUMN_NAME_AUDIO_URL, audioUrl);
            values.put(Audio.COLUMN_NAME_AUDIO_BINARY, blob);
            db.insertWithOnConflict(
                Audio.TABLE_NAME,
                null,
                values,
                SQLiteDatabase.CONFLICT_IGNORE
            );
            return blob;
        }
 */
    }
}
