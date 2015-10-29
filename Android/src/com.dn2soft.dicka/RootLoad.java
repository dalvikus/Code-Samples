package com.dn2soft.dicka;

import com.dn2soft.dicka.WordSmartContract.WordSmartJson.TheWords;

import java.io.File;
import android.content.Intent;

import android.util.Log;
import android.content.Context;


import android.app.Activity;    // requestWindowFeature, setProgressBarIndeterminateVisibility
import android.os.AsyncTask;
import java.util.List;
import java.util.ArrayList;

public final class RootLoad extends AsyncTask<Void, Void, List<TheWords>>
{
    private static final String TAG = "RootLoad";

    private final Activity mActivity;
    private final String mRootStr;
    public RootLoad(Activity activity, String rootStr) {
        mActivity = activity;
        mRootStr = rootStr;
    }

    @Override
    protected void onPreExecute()
    {
        mActivity.getActionBar().setSubtitle("loading... |" + mRootStr + "|");
        mActivity.setProgressBarIndeterminateVisibility(true);
    }

    @Override
    protected List<TheWords> doInBackground(Void... none)
    {
        List<TheWords> theWordsList = null;
        WordSmartDatabase.WordSmartOpenHelper wsHelper = null;
        try {
            File dickaRootPath = LoadActivity.getDickRootPath();
            wsHelper = new WordSmartDatabase.WordSmartOpenHelper((Context) mActivity, dickaRootPath);
            theWordsList = wsHelper.selectByRootStr(mRootStr);
        } finally {
            if (wsHelper != null)
                wsHelper.close();
        }
        return theWordsList;
    }

    @Override
    protected void onPostExecute(List<TheWords> theWordsList)
    {
        mActivity.setProgressBarIndeterminateVisibility(false);
        mActivity.getActionBar().setSubtitle(null);

        if (theWordsList == null) {
            return;
        }

        ArrayList<String> jsonStrArrayList = new ArrayList<String>();
        if (theWordsList != null && theWordsList.size() > 0) {
            for (TheWords theWords: theWordsList)
                jsonStrArrayList.add(WordSmartContract.setTheWordsJsonStr(theWords));
        }

        Intent intent = new Intent((Context) mActivity, RootListActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.putExtra(RootListActivity.ROOT_STR, mRootStr);
        intent.putStringArrayListExtra(RootListActivity.THE_WORDS_LIST, jsonStrArrayList);
        mActivity.startActivity(intent);
    }
}
