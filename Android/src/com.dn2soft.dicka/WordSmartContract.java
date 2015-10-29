package com.dn2soft.dicka;

import com.dn2soft.util.Jsonable;
import com.dn2soft.util.Json;
import com.dn2soft.util.JsonWalk;
import android.util.Log;

import android.content.Context;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.Cursor;
import java.util.List;
import java.util.ArrayList;

import java.io.File;

public final class WordSmartContract {
    private static final String TAG = "WordSmartContract";
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private WordSmartContract() {}

    public static abstract class TheWords {
        private TheWords() {}

        public static final String TABLE_NAME = "TheWords";
        public static final String COLUMN_NAME_BOOK_NO = "book_no";
        public static final String COLUMN_NAME_UNIT = "unit";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_POS = "pos";
        public static final String COLUMN_NAME_PRON = "pron";
        public static final String COLUMN_NAME_SENSE = "sense";
        public static final String COLUMN_NAME_EXAMPLES = "examples";

        public static final int LIMIT = 25; // >= maximum number of words in a unit

/*
CREATE
VIRTUAL                             -- FTS
TABLE TheWords
USING fts4                          -- FTS
(
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

----PRIMARY KEY (book_no, word)     -- non-FTS
    tokenize = porter               -- FTS
----------------------------------------
-- Some words in Word Smart do not be found in Cambridge:
--     not found: use altenatives
--     NOT FOUND: no substitutes
--
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
    }
    public static abstract class TheHitParade {
        private TheHitParade() {}

        public static final String TABLE_NAME = "TheHitParade";
        public static final String COLUMN_NAME_TYPE = "type";
        public static final String COLUMN_NAME_BOOK = "book";
        public static final String COLUMN_NAME_PAGE = "page";
        public static final String COLUMN_NAME_LINE = "line";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_SENSE = "sense";

/*
CREATE
VIRTUAL                 -- FTS
TABLE TheHitParade
USING fts4              -- FTS
(
    type    INTEGER,    -- 1: SAT, 2: GRE
    book    INTEGER,    -- 1: Word Smart I, 2: Word Smart II
    -- in a book, words are not sorted
    -- (page, line) shows the words at they are
    page    INTEGER,
    line    INTEGER,
    word    TEXT,
    sense   TEXT,

----PRIMARY KEY (type, book, word)  -- non-FTS
    tokenize = porter               -- FTS
);
 */
    }
    public static abstract class Root {
        private Root() {}

        public static final String TABLE_NAME = "Root";
        public static final String COLUMN_NAME_ROOT = "root0";
        public static final String COLUMN_NAME_SENSE = "sense";
        public static final String COLUMN_NAME_WORD = "word";

/*
CREATE
VIRTUAL                         -- FTS
TABLE Root
USING fts4                      -- FTS
(
----root    TEXT,       -- column name conflict with table name in fts
    root0   TEXT,       -- fts
    sense   TEXT,
    word    TEXT,

----PRIMARY KEY (root, word)    -- non-FTS
    tokenize = porter           -- FTS
);
 */
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
            //public String[] example;
            public String examples;

            public String snippet;
            public String offsets;
        }
        public List<TheWords>  theWordsList = new ArrayList<TheWords>();
        public static class TheHitParade implements Jsonable
        {
            public String type;
            public String book;
            public String word;
            public String sense;
        }
        public List<TheHitParade> theHitParadeList = new ArrayList<TheHitParade>();
        public static class Root implements Jsonable
        {
            public String root;
            public String sense;
            public List<String> wordList;
        }
        public List<Root>  rootList;
    }
    public static class RootListJson implements Jsonable
    {
        public List<WordSmartJson.Root>  rootList;
    }

    public static WordSmartJson setWordSmartJson(String wordSmartJsonStr)
    {
        if (wordSmartJsonStr == null)
            return null;
        WordSmartJson   wordSmartJson = null;
        try {
            wordSmartJson = (WordSmartJson) JsonWalk.setWalkClassJson(
                WordSmartJson.class,
                Json.setJson(wordSmartJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return wordSmartJson;
    }
    public static WordSmartJson.TheWords setTheWords(String theWordsJsonStr)
    {
        if (theWordsJsonStr == null)
            return null;
        WordSmartJson.TheWords   theWordsJson = null;
        try {
            theWordsJson = (WordSmartJson.TheWords) JsonWalk.setWalkClassJson(
                WordSmartJson.TheWords.class,
                Json.setJson(theWordsJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return theWordsJson;
    }
    public static String setTheWordsJsonStr(WordSmartJson.TheWords theWords)
    {
        if (theWords == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(WordSmartJson.TheWords.class, theWords);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
    public static String setJsonStr(WordSmartJson wordSmartJson)
    {
        if (wordSmartJson == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(WordSmartJson.class, wordSmartJson);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
    public static String setRootListJsonStr(RootListJson rootListJson)
    {
        if (rootListJson == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(RootListJson.class, rootListJson);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
    public static RootListJson setRootListJson(String rootListJsonStr)
    {
        if (rootListJsonStr == null)
            return null;
        RootListJson rootListJson = null;
        try {
            rootListJson = (RootListJson) JsonWalk.setWalkClassJson(
                RootListJson.class,
                Json.setJson(rootListJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return rootListJson;
    }
}
