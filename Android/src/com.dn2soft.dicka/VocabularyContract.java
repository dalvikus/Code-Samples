package com.dn2soft.dicka;

public final class VocabularyContract {
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private VocabularyContract() {}

/*
CREATE TABLE vocab (word UNIQUE);
 */
    public static abstract class Vocabulary {
        private Vocabulary() {}

        public static final String TABLE_NAME = "vocab";
        public static final String COLUMN_NAME_WORD = "word";
    }
}
