package com.dn2soft.dicka;

import com.dn2soft.util.Jsonable;
import com.dn2soft.util.Json;
import com.dn2soft.util.JsonWalk;

import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.ArrayList;
import android.util.SparseBooleanArray;
import java.util.Map;
import java.util.HashMap;
import java.util.Comparator;
import java.util.Collections;

import android.util.Log;

public final class LabelContract {
    private static final String TAG = "LabelContract";

    // To prevent someone from accidentally instantiating the contract class,
    // give it an empty constructor.
    private LabelContract() {}

/*
CREATE TABLE label_table (
--  rowid: label_id
    label_name TEXT,
    parent_id INTEGER default -1    -- -1: root, foreign key
);
 */
    public static abstract class TableLabel {
        private TableLabel() {}

        public static final String TABLE_NAME = "label_table";
        public static final String COLUMN_NAME_LABEL = "label_name";    // as it is
        public static final String COLUMN_NAME_PARENT_ID = "parent_id";
    }

/*
CREATE TABLE word_table (
--  rowid: word_id
    WORD_STR TEXT,      -- uppercase
    label_id INTEGER    -- foreign key
);
 */
    public static abstract class TableWord {
        private TableWord() {}

        public static final String TABLE_NAME = "word_table";
        public static final String COLUMN_NAME_WORD = "WORD_STR";   // uppercase
        public static final String COLUMN_NAME_LABEL_ID = "label_id";
    }
/*
test it with these:

INSERT INTO label_table VALUES ('tendency', -1);    -- rowid(labelId) = 1
INSERT INTO label_table VALUES ('rebuke', -1);      -- rowid(labelId) = 2
INSERT INTO word_table VALUES ('predilection', 1);  -- rowid(wordId) = 1
INSERT INTO word_table VALUES ('penchant', 1);      -- rowid(wordId) = 2
INSERT INTO word_table VALUES ('censure', 2);       -- rowid(wordId) = 3
 */


    // for each label_id
    public static class LabelData
    {
        public final LabelForJson.Label label;
        public final Set<Integer> childrenLabelIdSet;
        public final Set<String> wordStrSet;
        public LabelData(LabelForJson.Label lbl) {
            label = lbl;
            childrenLabelIdSet = new HashSet<Integer>();
            wordStrSet = new HashSet<String>();
        }
    }
    public static Map<Integer, LabelData> buildMap(LabelForJson labelForJson)
    {
        Map<Integer, LabelData> m = new HashMap<Integer, LabelData>();
        for (LabelForJson.Label label: labelForJson.labelList)
            m.put(new Integer(label.id), new LabelData(label));
        for (LabelForJson.Label label: labelForJson.labelList) {
            if (label.parentId != -1)
                m.get(new Integer(label.parentId)).childrenLabelIdSet.add(new Integer(label.id));
        }
        for (LabelForJson.Word word: labelForJson.wordList)
            m.get(new Integer(word.labelId)).wordStrSet.add(word.name.toUpperCase());  // convert to uppercase
        return m;
    }

    public static class LabelForJson implements Jsonable {
        public LabelForJson() {
            labelList = new ArrayList<Label>();
            wordList = new ArrayList<Word>();
        }

        public static class Label implements Jsonable {
            public int id;  // rowid
            public String name;
            public int parentId;    // -1: root
        }
        public List<Label> labelList;

        public static class Word implements Jsonable
        {
            public int id;  // rowid
            public String name;
            public int labelId;
        }
        public List<Word>  wordList;

        public void print()
        {
            Log.i(TAG, "labelList...");
            for (Label label: labelList) {
                Log.i(TAG, "    id: " + label.id);
                Log.i(TAG, "    name: " + label.name);
                Log.i(TAG, "    parentId: " + label.parentId);
            }
            Log.i(TAG, "wordList...");
            for (Word word: wordList) {
                Log.i(TAG, "    id: " + word.id);
                Log.i(TAG, "    name: " + word.name);
                Log.i(TAG, "    labelId: " + word.labelId);
            }
        }

        public static LabelForJson set(String jsonStr)
        {
            if (jsonStr == null)
                return null;
            LabelForJson labelForJson = null;
            try {
                labelForJson = (LabelForJson) JsonWalk.setWalkClassJson(
                    LabelForJson.class,
                    Json.setJson(jsonStr)
                );
            }
            catch (ClassNotFoundException e) {e.printStackTrace();}
            catch (InstantiationException e) {e.printStackTrace();}
            catch (IllegalAccessException e) {e.printStackTrace();}
            return labelForJson;
        }

        public static String getJsonStr(LabelForJson labelForJson)
        {
            if (labelForJson == null)
                return null;
            String jsonStr = null;
            try {
                jsonStr = JsonWalk.walkClass(LabelForJson.class, labelForJson);
            }
            catch (ClassNotFoundException e) {e.printStackTrace();}
            catch (IllegalAccessException e) {e.printStackTrace();}
            return jsonStr;
        }

        public static String getLabelOnly(String label)
        {
            return label == null ? null : label.substring(1 + label.lastIndexOf("/"));
        }


        public static final int LABEL_ID_INDEX = 0;     // start with 0
        public static final int DISPLAY_LABEL_INDEX = 1;
        public static final int PARENT_ID_INDEX = 2;
        public static final int WORD_ID_OFFSET = 3;     // number of XXX_INDEX above
        /**
         * get sorted list of display label; sorted by display label
         *
         *  each element consists of three parts
         *      [LABEL_ID_INDEX]: labelId
         *      [DISPLAY_LABEL_INDEX]: display label
         *      [PARENT_ID_INDEX]: parentId
         *      [WORD_ID_OFFSET]: wordId
         */
        public ArrayList<String[]> getDisplayLabelArray(String word)
        {
            if (labelList == null)
                return null;

            // list of "Word" (actually, pair of (wordId, labelId))
            // for given word
            class Blob {
                final List<Word> myWordList = new ArrayList<Word>();
                String wordIdStr(int labelId)
                {
                    for (Word w: myWordList) {
                        if (w.labelId == labelId)
                            return Integer.toString(w.id);
                    }
                    return null;
                }
            }
            HashMap<Integer, Label> m = new HashMap<Integer, Label>();
            for (Label label: labelList) {
                m.put(new Integer(label.id), label);
            }
            Blob blob = new Blob();
            for (Word w: wordList) {
                if (w.name.toUpperCase().equals(word.toUpperCase()))
                    blob.myWordList.add(w);
            }

            ArrayList<String[]> al = new ArrayList<String[]>(labelList.size());
            for (Label label: labelList) {
                StringBuilder sb = new StringBuilder();
                Label lb = label;
                while (true) {
                    sb.insert(0, "/" + lb.name);
                    if (lb.parentId == -1)
                        break;
                    lb = m.get(lb.parentId);
                }
                al.add(
                    new String[] {
                        String.valueOf(label.id),       // LABEL_ID_INDEX
                        sb.toString().substring(1),     // DISPLAY_LABEL_INDEX  // remove a leading '/'
                        String.valueOf(label.parentId), // PARENT_ID_INDEX
                        blob.wordIdStr(label.id)        // WORD_ID_OFFSET
                    }
                );
            }
////        for (String[] a: al)
////            Log.i(TAG, "{" + a[LABEL_ID_INDEX] + ", |" + a[DISPLAY_LABEL_INDEX] + "|, " + a[PARENT_ID_INDEX] + "}");
            Collections.sort(
                al,
                new Comparator<String[]>() {
                    @Override
                    public int compare(String[] a, String[] b) {
                        return a[DISPLAY_LABEL_INDEX].compareTo(b[DISPLAY_LABEL_INDEX]);
                    }
                }
            );
////        for (String[] a: al)
////            Log.i(TAG, "{" + a[LABEL_ID_INDEX] + ", |" + a[DISPLAY_LABEL_INDEX] + "|, " + a[PARENT_ID_INDEX] + "}");

            return al;
        }

        public ArrayList<String[]> getAllDisplayLabelArray()
        {
            if (labelList == null)
                return null;

            Map<Integer, LabelData> m = LabelContract.buildMap(this);
            ArrayList<String[]> al = new ArrayList<String[]>(labelList.size());
            for (Label label: labelList) {
                StringBuilder sb = new StringBuilder();
                Label lb = label;
                while (true) {
                    sb.insert(0, "/" + lb.name);
                    if (lb.parentId == -1)
                        break;
                    lb = m.get(lb.parentId).label;
                }
                String[] a = new String[WORD_ID_OFFSET + m.get(label.id).wordStrSet.size()];
                a[LABEL_ID_INDEX] = String.valueOf(label.id);
                a[DISPLAY_LABEL_INDEX] = sb.toString().substring(1); // remove a leading '/'
                a[PARENT_ID_INDEX] = String.valueOf(label.parentId);
                int i = WORD_ID_OFFSET;
                for (String UWORD: m.get(label.id).wordStrSet)
                    a[i++] = UWORD;
                al.add(a);
            }
////        for (String[] a: al)
////            Log.i(TAG, "{|" + TextUtils.join("|, |", a) + "|}");
            Collections.sort(
                al,
                new Comparator<String[]>() {
                    @Override
                    public int compare(String[] a, String[] b) {
                        return a[DISPLAY_LABEL_INDEX].compareTo(b[DISPLAY_LABEL_INDEX]);
                    }
                }
            );
////        for (String[] a: al)
////            Log.i(TAG, "{|" + TextUtils.join("|, |", a) + "|}");

            return al;
        }

        public UpdateData setUpdateData(
            final ArrayList<String[]> displayLabelArrayList,
            final String word,
            final boolean createLabel,
            final String newLabel,
            final boolean underLabel,
            final SparseBooleanArray labels
        )
        {
            if (word == null)
                throw new IllegalArgumentException("word is null");
            UpdateData data = new UpdateData(word);
            if (createLabel) {
                if (newLabel == null)
                    throw new IllegalArgumentException("createLabel is set but newLabel = null");
                if (newLabel.isEmpty())
                    throw new IllegalArgumentException("createLabel is set but newLabel: |" + newLabel + "|: is empty");
                if (newLabel.contains("/"))
                    throw new IllegalArgumentException("createLabel is set but newLabel: |" + newLabel + "|: contains invalid character('/')");
                data.createLabel = true;
                data.newLabel = newLabel;   // as it is
                if (underLabel) {
                    int iSelected = -1;
                    for (int i = 0; i < displayLabelArrayList.size(); ++i) {
                        if (labels.get(i)) {
                            iSelected = i;
                            break;
                        }
                    }
                    if (iSelected == -1)
                        throw new IllegalArgumentException("underLabel is set but no label selected");
                    data.underLabel = true;
                    data.underLabelId = (new Integer(displayLabelArrayList.get(iSelected)[LABEL_ID_INDEX])).intValue();
                } else {
                    data.underLabel = false;
                }
            } else {
                data.createLabel = false;
                // determine addLabelIdList, delWordIdList, purgeLabelIdList
                // by comparing displayLabelArrayList, labels
                Map<Integer, LabelData> m = LabelContract.buildMap(this);
/*
                for (Map.Entry<Integer, LabelData> entry: m.entrySet()) {
                    Log.i(TAG, "lableId: |" + entry.getKey() + "|");
                    LabelData labelData = entry.getValue();
                    Log.i(TAG, "    parentId: " + labelData.label.parentId);
                    for (String w: labelData.wordStrSet)
                        Log.i(TAG, "    word: |" + w + "|");
                    for (Integer labelId: labelData.childrenLabelIdSet)
                        Log.i(TAG, "    child: |" + labelId + "|");
                }
 */
                List<Integer> wouldBeDelLabelIdList = new ArrayList<Integer>();
                for (int i = 0; i < displayLabelArrayList.size(); ++i) {
                    String[] a = displayLabelArrayList.get(i);
                    Integer labelId = new Integer(a[LABEL_ID_INDEX]);
                    boolean bIn = a[WORD_ID_OFFSET] != null;
                    boolean bOut = labels.get(i);
////                Log.i(TAG, "|" + a[DISPLAY_LABEL_INDEX] + "|: in(" + bIn + "), out(" + bOut + ")");
                    if (bIn && !bOut) {
                        wouldBeDelLabelIdList.add(labelId);
                        data.delWordIdList.add(new Integer(a[WORD_ID_OFFSET]));
                        if (!m.containsKey(labelId.intValue()))
                            throw new IllegalArgumentException("FATAL: no key: |" + labelId + "|: report it!");
                        if (!m.get(labelId.intValue()).wordStrSet.remove(word.toUpperCase()))
                            throw new IllegalArgumentException("FATAL: no val: |" + word + "| in key: |" + labelId + "|: report it!");
                    } else if (!bIn && bOut)
                        data.addLabelIdList.add(labelId);
                }
////            Log.i(TAG, "" + wouldBeDelLabelIdList);
/*
                for (Map.Entry<Integer, LabelData> entry: m.entrySet()) {
                    Log.i(TAG, "lableId: |" + entry.getKey() + "|");
                    LabelData labelData = entry.getValue();
                    Log.i(TAG, "    parentId: " + labelData.label.parentId);
                    for (String w: labelData.wordStrSet)
                        Log.i(TAG, "    word: |" + w + "|");
                    for (Integer labelId: labelData.childrenLabelIdSet)
                        Log.i(TAG, "    child: |" + labelId + "|");
                }
 */
                // purgeLabelIdList
                for (Integer labelId: wouldBeDelLabelIdList) {
                    Integer lblId = labelId;
                    LabelData labelData = m.get(lblId);
                    while (labelData.wordStrSet.isEmpty() && labelData.childrenLabelIdSet.isEmpty()) {
                        data.purgeLabelIdList.add(lblId);
                        int parentId = labelData.label.parentId;
                        if (parentId == -1)
                            break;
                        labelData = m.get(new Integer(parentId));
                        labelData.childrenLabelIdSet.remove(lblId);
                        lblId = parentId;
                    }
                }
            }
            return data;
        }
    }

/*
 */
    public static class UpdateData {
        final String UWORD;    // != null
        UpdateData(String w) {
            UWORD = w.toUpperCase();
            addLabelIdList = new ArrayList<Integer>();
            delWordIdList = new ArrayList<Integer>();
            purgeLabelIdList = new ArrayList<Integer>();
        }

        boolean createLabel;    // if True, newLabel != null
            String newLabel;        // if non-null, not containing '/'
            boolean underLabel;     // if True, underLabel != null
            int underLabelId;
        final ArrayList<Integer> addLabelIdList;
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
        // for each labelId in addLabelIdList;
        // INSERT INTO word_table VALUES (word, labelId)
    }
 */

        final ArrayList<Integer> delWordIdList;     // for each wordId in array
        //  DELETE FROM word_table WHERE rowid = wordId
        final ArrayList<Integer> purgeLabelIdList;  // for each labelId in array
        //  DELETE FROM label_table WHERE rowid = labelId

        Boolean shouldUpdate()
        {
            if (createLabel) {
                if (newLabel == null)
                    throw new IllegalArgumentException("createLabel is set but newLabel = null");
                if (newLabel.isEmpty())
                    throw new IllegalArgumentException("createLabel is set but newLabel: |" + newLabel + "|: is empty");
                if (newLabel.contains("/"))
                    throw new IllegalArgumentException("createLabel is set but newLabel: |" + newLabel + "|: contains invalid character('/')");
                return Boolean.TRUE;
            } else {
                if (addLabelIdList.size() + delWordIdList.size() + purgeLabelIdList.size() > 0)
                    return Boolean.TRUE;
                else
                    return Boolean.FALSE;
            }
        }

        public void print()
        {
            Log.i(TAG, "UWORD: |" + UWORD + "|");
            Log.i(TAG, "createLabel: " + createLabel);
            Log.i(TAG, "newLabel: |" + newLabel + "|");
            Log.i(TAG, "underLabel: " + underLabel);
            Log.i(TAG, "underLabelId: " + underLabelId);
            Log.i(TAG, "addLabelIdList:");
            for (Integer labelId: addLabelIdList)
                Log.i(TAG, "    " + labelId);
            Log.i(TAG, "delWordIdList:");
            for (Integer wordId: delWordIdList)
                Log.i(TAG, "    " + wordId);
            Log.i(TAG, "purgeLabelIdList:");
            for (Integer labelId: purgeLabelIdList)
                Log.i(TAG, "    " + labelId);
        }
    }
}
