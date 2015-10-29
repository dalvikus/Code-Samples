package com.dn2soft.dicka;

import java.util.List;
import java.util.ArrayList;
import com.dn2soft.dicka.LabelContract.LabelForJson;
import android.content.ContentValues;

import android.content.Context;
import java.io.File;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase;
import android.database.Cursor;

import android.util.Log;

public final class LabelDatabase {
    private static final String TAG = "LabelDatabase";

    public static final String DATABASE_NAME = "label.db";

    public static class LabelOpenHelper extends SQLiteOpenHelper {
        // If you change the database schema, you must increment the database version.
        public static final int DATABASE_VERSION = 1;

        public LabelOpenHelper(Context context, File rootPath) {
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

        public List<String> getWords(String label)
        {
            SQLiteDatabase db = getReadableDatabase();
            String  q = String.format(
//              "SELECT WORD_STR FROM word_table WHERE label_id IN (SELECT rowid FROM label_table WHERE label_name LIKE 'tendency') ORDER BY WORD_STR"
                "SELECT %s FROM %s WHERE %s IN (SELECT rowid FROM %s WHERE %s LIKE ?) ORDER BY %s"
                ,
                LabelContract.TableWord.COLUMN_NAME_WORD,
                LabelContract.TableWord.TABLE_NAME,
                LabelContract.TableWord.COLUMN_NAME_LABEL_ID,
                LabelContract.TableLabel.TABLE_NAME,
                LabelContract.TableLabel.COLUMN_NAME_LABEL,
                LabelContract.TableWord.COLUMN_NAME_WORD
            );
            //Log.i(TAG, "query: |" + q + "| with |" + label + "|");

            List<String> a = new ArrayList<String>();
            Cursor c = db.rawQuery(q, new String[] {label});
            if (c == null) {    // an error occurred
                Log.e(TAG, "query: |" + q + "|");
                Log.e(TAG, "An error occurred");
                return null;
            }
            while (c.moveToNext()) {
                a.add(c.getString(0));
            }
            c.close();
            if (a.size() == 0)
                a = null;
            return a;
        }

        public LabelForJson select()
        {
            SQLiteDatabase db = getReadableDatabase();
            LabelForJson labelForJson = new LabelForJson();
    
            String  q = String.format(
                "SELECT rowid,%s,%s FROM %s"
                ,
                LabelContract.TableLabel.COLUMN_NAME_LABEL,
                LabelContract.TableLabel.COLUMN_NAME_PARENT_ID,
                LabelContract.TableLabel.TABLE_NAME
            );
////        Log.i(TAG, "query: |" + q + "| with |" + query + "|");

            Cursor c = db.rawQuery(q, null);
            if (c == null) {    // an error occurred
                Log.e(TAG, "query: |" + q + "|");
                Log.e(TAG, "An error occurred");
                return null;
            }
            while (c.moveToNext()) {
                LabelForJson.Label label = new LabelForJson.Label();
                labelForJson.labelList.add(label);
                label.id = (new Integer(c.getString(0))).intValue();
                label.name = c.getString(1);
                label.parentId = (new Integer(c.getString(2))).intValue();
            }
            c.close();

            q = String.format(
                "SELECT rowid,%s,%s FROM %s"
                ,
                LabelContract.TableWord.COLUMN_NAME_WORD,
                LabelContract.TableWord.COLUMN_NAME_LABEL_ID,
                LabelContract.TableWord.TABLE_NAME
            );
////        Log.i(TAG, "query: |" + q + "| with |" + query + "|");

            c = db.rawQuery(q, null);
            if (c == null) {    // an error occurred
                Log.e(TAG, "query: |" + q + "|");
                Log.e(TAG, "An error occurred");
                return null;
            }
            while (c.moveToNext()) {
                LabelForJson.Word word = new LabelForJson.Word();
                labelForJson.wordList.add(word);
                word.id = (new Integer(c.getString(0))).intValue();
                word.name = c.getString(1);
                word.labelId = (new Integer(c.getString(2))).intValue();
            }
            c.close();
            return labelForJson;
        }

        public boolean update(LabelContract.UpdateData data)
        {
            SQLiteDatabase db = getWritableDatabase();

            boolean ret = false;
            db.beginTransaction();
            try {
/*
    if (createLabel) {
        if (underLabel) {
            // INSERT INTO label_table VALUES(newLabel, underLabelId);
        } else {
            // INSERT INTO label_table VALUES(newLabel, -1);
        }
        // with returned rowid
        // INSERT INTO word_table VALUES (word, rowid)
    } else {
        // for each id in addLabelIdList;
        // INSERT INTO word_table VALUES (word, id)
    }
 */
                if (data.UWORD != null) {
                    ContentValues values;
                    if (data.createLabel) {
                        if (data.newLabel == null) {
                            Log.e(TAG, "ERR: newLabel = null");
                            return false;
                        }
                        values = new ContentValues();
                        values.put(LabelContract.TableLabel.COLUMN_NAME_LABEL, data.newLabel);
                        values.put(LabelContract.TableLabel.COLUMN_NAME_PARENT_ID, data.underLabel ? data.underLabelId : (int) -1);
                        long labelId = db.insertWithOnConflict(
                            LabelContract.TableLabel.TABLE_NAME,
                            null,
                            values,
                            SQLiteDatabase.CONFLICT_IGNORE
                        );
///                     Log.i(TAG, "rowid(labelId) = " + labelId);
                        if (labelId == -1) {
                            Log.e(TAG, "rowid (labelId)= -1");
                            return false;
                        }
                        values = new ContentValues();
                        values.put(LabelContract.TableWord.COLUMN_NAME_WORD, data.UWORD);
                        values.put(LabelContract.TableWord.COLUMN_NAME_LABEL_ID, labelId);
                        long wordId = db.insertWithOnConflict(
                            LabelContract.TableWord.TABLE_NAME,
                            null,
                            values,
                            SQLiteDatabase.CONFLICT_IGNORE
                        );
                    } else {
                        if (data.addLabelIdList != null) {
                            for (Integer labelId: data.addLabelIdList) {
                                values = new ContentValues();
                                values.put(LabelContract.TableWord.COLUMN_NAME_WORD, data.UWORD);
                                values.put(LabelContract.TableWord.COLUMN_NAME_LABEL_ID, labelId);
                                long wordId = db.insertWithOnConflict(
                                    LabelContract.TableWord.TABLE_NAME,
                                    null,
                                    values,
                                    SQLiteDatabase.CONFLICT_IGNORE
                                );
                            }
                        }
                    }
                }
/*
        ArrayList<String> delWordIdList;  // for each id in array
        //  DELETE FROM label_table WHERE rowid = id
        ArrayList<String> purgeLabelIdList;   // for each id in array
        //  DELETE FROM word_table WHERE rowid = id
 */
                if (data.delWordIdList != null) {
                    for (Integer wordId: data.delWordIdList) {
                        db.delete(
                            LabelContract.TableWord.TABLE_NAME,
                            "rowid = CAST(? AS INTEGER)",
                            new String[] {wordId.toString()}
                        );
                    }
                }
                if (data.purgeLabelIdList != null) {
                    for (Integer labelId: data.purgeLabelIdList) {
                        db.delete(
                            LabelContract.TableLabel.TABLE_NAME,
                            "rowid = CAST(? AS INTEGER)",
                            new String[] {labelId.toString()}
                        );
                    }
                }

                db.setTransactionSuccessful();
                ret = true;
            } finally {
                db.endTransaction();
            }
            return ret;
        }
        public int updateLabel(String labelIdStr, String labelName, String parentIdStr)
        {
            SQLiteDatabase db = getWritableDatabase();
            if (labelName == null && parentIdStr == null) {
                Log.e(TAG, "both labelName and parentIdStr are null: invalid");
                return -1;
            } else if (labelName != null && parentIdStr != null) {
                Log.e(TAG, "Neither labelName nor parentIdStr is null: invalid");
                return -1;
            }
            ContentValues values = new ContentValues();
            if (labelName == null) {
                Integer parentId = null;
                try {
                    parentId = new Integer(parentIdStr);
                } catch (NumberFormatException e) {e.printStackTrace();}
                if (parentId == null) {
                    Log.e(TAG, "malformed parentIdStr: |" + parentIdStr + "|");
                    return -1;
                }
                values.put(LabelContract.TableLabel.COLUMN_NAME_PARENT_ID, parentId.intValue());
            } else
                values.put(LabelContract.TableLabel.COLUMN_NAME_LABEL, labelName);
            return db.update(
                LabelContract.TableLabel.TABLE_NAME,
                values,
                "rowid = CAST(? AS INTEGER)",
                new String[] {labelIdStr}
            );
        }
    }
}
