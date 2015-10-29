package com.dn2soft.dicka;

import android.provider.BaseColumns;

import com.dn2soft.dickc.dictionary.Cambridge;
import com.dn2soft.util.Json;
import com.dn2soft.util.JsonWalk;

/*
 * see com.dn2soft.java.utility.Derby.java
 */
public final class CambridgeContract {
    private final static String TAG = "CambridgeContract";
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private CambridgeContract() {}

/*
CREATE TABLE Web (
    href0   VARCHAR(128)    NOT NULL,
    page    VARCHAR(8192)   NOT NULL,   -- Page::getJSONStr()
    more    VARCHAR(4096),              -- More::getJSONStr()
                                        --      NULL:   not retrieved yet
                                        --      "":     none

    UNIQUE(href0)
);
CREATE TABLE Vocabulary (
    _id     INTEGER PRIMARY KEY,                -- android specific: BaseColumns._ID
    word    VARCHAR(64)     NOT NULL,
    href0   VARCHAR(128)    NOT NULL,
    wordStr SMALLINT        NOT NULL DEFAULT 0, -- is wordStr a variance of word
                                                --  0: given wordStr is one of words
                                                --  1: otherwise

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
CREATE INDEX IDX_Vocabulary_word ON Vocabulary(word);   -- sqlite specific
CREATE TABLE Link (
    _id     INTEGER PRIMARY KEY,                        -- android specific
    href0   VARCHAR(128)    NOT NULL,
    href    VARCHAR(128)    NOT NULL,

    UNIQUE (href0, href)--,
--  FOREIGN KEY (href) REFERENCES Web (href0),
--  FOREIGN KEY (href0) REFERENCES Web (href0)
);
CREATE INDEX IDX_Link_href0 ON Link(href0);
CREATE TABLE NotFound(
    word    VARCHAR(64)     NOT NULL,
    refer   VARCHAR(64),    -- NULL: refer not found

    UNIQUE(word)
);
CREATE INDEX IDX_NotFound_refer ON NotFound(refer);
 */


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
    public static abstract class Web {
        private Web() {}

        public static final String TABLE_NAME = "Web";
        public static final String COLUMN_NAME_HREF0 = "href0";
        public static final String COLUMN_NAME_PAGE = "page";
        public static final String COLUMN_NAME_MORE = "more";
    }
/*
CREATE TABLE Vocabulary (
    _id     INTEGER PRIMARY KEY,                -- android specific: BaseColumns._ID
    word    VARCHAR(64)     NOT NULL,
    href0   VARCHAR(128)    NOT NULL,
    wordStr SMALLINT        NOT NULL DEFAULT 0, -- is wordStr a variance of word
                                                --  0: given wordStr is one of words
                                                --  1: otherwise

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
CREATE INDEX IDX_Vocabulary_word ON Vocabulary(word);   -- sqlite specific
 */
    public static abstract class Vocabulary implements BaseColumns {
        private Vocabulary() {}

        public static final String TABLE_NAME = "Vocabulary";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_HREF0 = "href0";
        public static final String COLUMN_NAME_WORDSTR = "wordStr";
    }

/*
CREATE TABLE Link (
    _id     INTEGER PRIMARY KEY,                        -- android specific
    href0   VARCHAR(128)    NOT NULL,
    href    VARCHAR(128)    NOT NULL,

    UNIQUE (href0, href)--,
--  FOREIGN KEY (href) REFERENCES Web (href0),
--  FOREIGN KEY (href0) REFERENCES Web (href0)
);
CREATE INDEX IDX_Link_href0 ON Link(href0);
 */
    public static abstract class Link implements BaseColumns {
        private Link() {}

        public static final String TABLE_NAME = "Link";
        public static final String COLUMN_NAME_HREF0 = "href0";
        public static final String COLUMN_NAME_HREF = "href";
    }

/*
CREATE TABLE NotFound(
    word    VARCHAR(64)     NOT NULL,
    refer   VARCHAR(64),    -- NULL: refer not found

    UNIQUE(word)
);
CREATE INDEX IDX_NotFound_refer ON NotFound(refer);
 */
    public static abstract class NotFound {
        private NotFound() {}

        public static final String TABLE_NAME = "NotFound";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_REFER = "refer";
    }

    public static Cambridge.ResultBase setCambridgeResultBase(String cambridgeResultBaseJsonStr)
    {
        if (cambridgeResultBaseJsonStr == null)
            return null;
        Cambridge.ResultBase resultBase = null;
        try {
            resultBase = (Cambridge.ResultBase) JsonWalk.setWalkClassJson(
                Cambridge.ResultBase.class,
                Json.setJson(cambridgeResultBaseJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return resultBase;
    }
    public static Cambridge.Result setCambridgeResult(String cambridgeResultJsonStr)
    {
        if (cambridgeResultJsonStr == null)
            return null;
        Cambridge.Result result = null;
        try {
            result = (Cambridge.Result) JsonWalk.setWalkClassJson(
                Cambridge.Result.class,
                Json.setJson(cambridgeResultJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return result;
    }
    public static String setResultBaseJsonStr(Cambridge.ResultBase resultBase)
    {
        if (resultBase == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(Cambridge.ResultBase.class, resultBase);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
    public static String setResultJsonStr(Cambridge.Result result)
    {
        if (result == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(Cambridge.Result.class, result);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
}
