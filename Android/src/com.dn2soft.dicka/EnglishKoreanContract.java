package com.dn2soft.dicka;

import com.dn2soft.util.Json;
import com.dn2soft.util.Jsonable;
import com.dn2soft.util.JsonWalk;
import java.util.List;

public final class EnglishKoreanContract {
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private EnglishKoreanContract() {}

/*
CREATE VIRTUAL TABLE English_English0
USING fts4                      -- FTS
(
    word    TEXT,
    tokenize = porter           -- FTS
);
 */
    public static abstract class English_English0 {
        private English_English0() {}

        public static final String TABLE_NAME = "English_English0";
        public static final String COLUMN_NAME_WORD = "word";
    }

/*
CREATE TABLE English_Korean (word TEXT UNIQUE, sense TEXT);
 */
    public static abstract class English_Korean {
        private English_Korean() {}

        public static final String TABLE_NAME = "English_Korean";
        public static final String COLUMN_NAME_WORD = "word";
        public static final String COLUMN_NAME_SENSE = "sense";
    }


    /**
     * Wrapper class of English Korean database
     *
     * It is used to get json string(JsonWalk.walkClass) and to set class(JsonWalk.setWalkClassJson) by JsonWalk class.
     * All member should be public for JsonWalk.setWalkClassJson to access.
     */
    public static class EnglishKoreanJson implements Jsonable
    {
        public static class EnglishKorean implements Jsonable
        {
            public String word;
            public String sense;
        }

        public List<EnglishKorean> englishKoreanList;
    }

    public static EnglishKoreanJson setEnglishKoreanJson(String englishKoreanJsonStr)
    {
        if (englishKoreanJsonStr == null)
            return null;
        EnglishKoreanJson englishKoreanJson = null;
        try {
            englishKoreanJson = (EnglishKoreanJson) JsonWalk.setWalkClassJson(
                    EnglishKoreanJson.class,
                    Json.setJson(englishKoreanJsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return englishKoreanJson;
    }
    public static String setJsonStr(EnglishKoreanJson englishKoreanJson)
    {
        if (englishKoreanJson == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(EnglishKoreanJson.class, englishKoreanJson);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
}
