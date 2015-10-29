package com.dn2soft.dicka;

import com.dn2soft.dicka.WordSmartContract.WordSmartJson.TheWords;
import com.dn2soft.dicka.LoadActivity.Entry;

import java.io.File;
import android.content.Intent;

import android.util.Log;
import android.content.Context;


import android.app.Activity;    // requestWindowFeature, setProgressBarIndeterminateVisibility
import android.os.AsyncTask;
import java.util.List;
import java.util.ArrayList;

public final class LabelLoad extends AsyncTask<Void, Void, List<TheWords>>
{
    private static final String TAG = "LabelLoad";

    final Activity          mActivity;
    final String            mLabel;
    final ArrayAdapterEx    mAdapter;
    final ArrayList<String> mTheWordsJsonStrArrayList;

    public LabelLoad(
        Activity        activity,
        String          label,
        ArrayAdapterEx  adapter,
        ArrayList<String> theWordsJsonStrArrayList
    ) {
        mActivity = activity;
        mLabel = label;
        mAdapter = adapter;
        mTheWordsJsonStrArrayList = theWordsJsonStrArrayList;
    }

    @Override
    protected void onPreExecute()
    {
        mActivity.getActionBar().setSubtitle("loading... |" + mLabel + "|");
        mActivity.setProgressBarIndeterminateVisibility(true);
    }

    @Override
    protected List<TheWords> doInBackground(Void... none)
    {
        List<TheWords> r = null;
        WordSmartDatabase.WordSmartOpenHelper wordSmartOpenHelper = null;
        LabelDatabase.LabelOpenHelper labelOpenHelper = null;
        try {
            File dickaRootPath = LoadActivity.getDickRootPath();
            wordSmartOpenHelper = new WordSmartDatabase.WordSmartOpenHelper((Context) mActivity, dickaRootPath);
            labelOpenHelper = new LabelDatabase.LabelOpenHelper((Context) mActivity, dickaRootPath);
            if (wordSmartOpenHelper != null && labelOpenHelper != null) {
                r = wordSmartOpenHelper.select(labelOpenHelper.getWords(mLabel), true);
            }
        } finally {
            if (wordSmartOpenHelper != null)
                wordSmartOpenHelper.close();
            if (labelOpenHelper != null)
                labelOpenHelper.close();
        }
        return r;
    }

    @Override
    protected void onPostExecute(List<TheWords> theWordsList)
    {
        mActivity.setProgressBarIndeterminateVisibility(false);
        ArrayList<String> jsonStrArrayList = null;
        if (mAdapter != null) {
            mActivity.getActionBar().setSubtitle("long-press any items to manage labels");
            jsonStrArrayList = mTheWordsJsonStrArrayList;
        } else {
            mActivity.getActionBar().setSubtitle(null);
            jsonStrArrayList = new ArrayList<String>();
        }
        if (theWordsList != null && theWordsList.size() > 0) {
            for (TheWords theWords: theWordsList)
                jsonStrArrayList.add(WordSmartContract.setTheWordsJsonStr(theWords));
        }

        if (mAdapter != null) {
            List<Entry> entries = new ArrayList<Entry>();
            if (theWordsList != null) {
                for (TheWords theWords: theWordsList) {
                    entries.add(new Entry(theWords.word, "        " + theWords.sense));
                }
            }
            mAdapter.setData(entries);
        } else {
            Intent intent = new Intent((Context) mActivity, LabelListActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            intent.putExtra(LabelListActivity.LABEL, mLabel);
            intent.putStringArrayListExtra(LabelListActivity.THE_WORDS_LIST, jsonStrArrayList);
            mActivity.startActivity(intent);
        }
    }
}
