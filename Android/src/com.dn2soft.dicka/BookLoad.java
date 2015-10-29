package com.dn2soft.dicka;

import com.dn2soft.dicka.WordSmartContract.WordSmartJson.TheWords;

import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.io.File;
import android.content.Intent;

import android.util.Log;
import android.content.Context;


import android.app.Activity;    // requestWindowFeature, setProgressBarIndeterminateVisibility
import android.os.AsyncTask;
import java.util.List;
import java.util.ArrayList;

public final class BookLoad extends AsyncTask<Void, Void, List<TheWords>>
{
    private static final String TAG = "BookLoad";

    private static final String UNIT_MATCH_STR = "\\s*#([123])\\.(\\d+)";
    private static final Pattern mPattern = Pattern.compile(UNIT_MATCH_STR);

    private final Activity mActivity;
    private final String mBookUnitStr;
    private String mErrorMessage;
    public BookLoad(Activity activity, String bookUnitStr) {
        mActivity = activity;
        mBookUnitStr = bookUnitStr;
    }

    @Override
    protected void onPreExecute()
    {
        mActivity.getActionBar().setSubtitle("loading... |" + mBookUnitStr + "|");
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
            Matcher m = mPattern.matcher(mBookUnitStr);
            if (m.matches()) {
                theWordsList = wsHelper.select(m.group(1), m.group(2));
            } else {
                mErrorMessage = "|" + mBookUnitStr + "|: No/invalid match with |" + UNIT_MATCH_STR + "|";
                Log.e(TAG, mErrorMessage);
            }
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
            if (mErrorMessage != null) {
            }
            return;
        }

        ArrayList<String> jsonStrArrayList = new ArrayList<String>();
        if (theWordsList != null && theWordsList.size() > 0) {
            for (TheWords theWords: theWordsList)
                jsonStrArrayList.add(WordSmartContract.setTheWordsJsonStr(theWords));
        }

        Intent intent = new Intent((Context) mActivity, BookListActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.putExtra(BookListActivity.BOOK_UNIT_STR, mBookUnitStr);
        intent.putStringArrayListExtra(BookListActivity.THE_WORDS_LIST, jsonStrArrayList);
        mActivity.startActivity(intent);
    }
}
