package com.dn2soft.dicka;

import android.util.Log;

import com.dn2soft.util.Jsonable;
import com.dn2soft.util.Json;
import com.dn2soft.util.JsonWalk;
import java.util.List;

/*
 * see com.dn2soft.java.utility.Derby.java
 */
public final class NationalCancerInstituteContract {
    private static final String TAG = "NationalCancerInstituteContract";
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private NationalCancerInstituteContract() {}

    // Inner class that defines the table contents
    public static abstract class TermsDef {
        private TermsDef() {}

        public static final String TABLE_NAME = "TermsDef";
        public static final String COLUMN_NAME_SRC = "src";
        public static final String COLUMN_NAME_TERMS = "terms";
        public static final String COLUMN_NAME_MP3 = "mp3";
        public static final String COLUMN_NAME_MTD = "mtd";
        public static final String COLUMN_NAME_DEF = "def";

        public static final int LIMIT = 20;
/*
CREATE VIRTUAL TABLE TermsDef
USING fts4                      -- FTS
--CREATE TABLE TermsDef
(
    src     SMALLINT        NOT NULL,   -- 1: genetics, 2: cancer
    terms   VARCHAR(128)    NOT NULL,   -- can be duplicated
    mp3     VARCHAR(16)     NOT NULL,   -- "": None
    mtd     VARCHAR(128)    NOT NULL,   -- "": None
    def     VARCHAR(4096)   NOT NULL,

--  UNIQUE(src, terms, def)
    tokenize = porter           -- FTS
);
--CREATE INDEX IDX_TermsDef_terms ON TermsDef(terms);     -- sqlite specific
 */
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

            public String snippet;
        }

        public List<TermsDef> termsDefList;
    }


    public static String setJsonStr(NationalCancerInstituteJson nationalCancerInstituteJson)
    {
        if (nationalCancerInstituteJson == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(NationalCancerInstituteJson.class, nationalCancerInstituteJson);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }

    public static NationalCancerInstituteJson setNationalCancerInstituteJson(String nationalCancerInstituteJsonStr)
    {
        if (nationalCancerInstituteJsonStr == null)
            return null;
        NationalCancerInstituteJson   nationalCancerInstituteJson = null;
        try {
            nationalCancerInstituteJson = (NationalCancerInstituteJson) JsonWalk.setWalkClassJson(
                NationalCancerInstituteJson.class,
                Json.setJson(nationalCancerInstituteJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return nationalCancerInstituteJson;
    }

    public static NationalCancerInstituteJson.TermsDef setTermsDef(String termsDefJsonStr)
    {
        if (termsDefJsonStr == null)
            return null;
        NationalCancerInstituteJson.TermsDef   termsDef = null;
        try {
            termsDef = (NationalCancerInstituteJson.TermsDef) JsonWalk.setWalkClassJson(
                NationalCancerInstituteJson.TermsDef.class,
                Json.setJson(termsDefJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return termsDef;
    }
    public static String setTermsDefJsonStr(NationalCancerInstituteJson.TermsDef termsDef)
    {
        if (termsDef == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(NationalCancerInstituteJson.TermsDef.class, termsDef);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
}
