package com.dn2soft.dicka;

public final class HistoryContract {
    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private HistoryContract() {}

/*
CREATE TABLE history_table (word_str TEXT, datetime_str TEXT);
 */
    public static abstract class TableHistory {
        private TableHistory() {}

        public static final String NOW = "DATETIME('NOW')";
        public static final int LIMIT = 10;

        public static final String TABLE_NAME = "history_table";
        public static final String COLUMN_NAME_WORD = "word_str";
        public static final String COLUMN_NAME_DATETIME = "datetime_str";
    }
/*
INSERT INTO history_table VALUES ('word', DATETIME('now'));

SELECT word_str, DATETIME(datetime_str) FROM history_table ORDER BY DATETIME(datetime_str) DESC LIMIT 5;
SELECT DISTINCT word_str FROM history_table ORDER BY DATETIME(datetime_str) DESC LIMIT 5;
 */
}
