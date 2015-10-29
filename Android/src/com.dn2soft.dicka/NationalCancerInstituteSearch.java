package com.dn2soft.dicka;

import android.content.Intent;

import android.util.Log;
import android.content.Context;

import android.app.Activity;    // requestWindowFeature, setProgressBarIndeterminateVisibility
import android.os.AsyncTask;

public final class NationalCancerInstituteSearch extends AsyncTask<Void, Void, NationalCancerInstituteContract.NationalCancerInstituteJson>
{
    private static final String TAG = "NationalCancerInstituteSearch";

    final Activity mActivity;
    final String mQuery;

    public NationalCancerInstituteSearch(
        Activity        activity,
        String          query
    ) {
        mActivity = activity;
        mQuery = query.trim().replaceAll("\\s+", " ").replaceAll(Search.SEARCH_ALL_CHAR_PATTERN, "%");
////    Log.i(TAG, "query: |" + query + "|");
    }

    @Override
    protected void onPreExecute()
    {
        mActivity.getActionBar().setSubtitle("searching... |" + mQuery + "|");
        mActivity.setProgressBarIndeterminateVisibility(true);
    }

    @Override
    protected NationalCancerInstituteContract.NationalCancerInstituteJson doInBackground(Void... none)
    {
        NationalCancerInstituteContract.NationalCancerInstituteJson r = null;
        NationalCancerInstituteDatabase.NationalCancerInstituteOpenHelper helper = null;
        try {
            helper = new NationalCancerInstituteDatabase.NationalCancerInstituteOpenHelper((Context) mActivity, LoadActivity.getDickRootPath());
            if (helper != null) {
                r = helper.select(mQuery);
            }
        } finally {
            if (helper != null)
                helper.close();
        }
        return r;
    }

    @Override
    protected void onPostExecute(NationalCancerInstituteContract.NationalCancerInstituteJson nationalCancerInstituteJson)
    {
        mActivity.setProgressBarIndeterminateVisibility(false);
        mActivity.getActionBar().setSubtitle(null);

////    Log.i(TAG, "NationalCancerInstituteJson: " + nationalCancerInstituteJson);
        String jsonStr = NationalCancerInstituteContract.setJsonStr(nationalCancerInstituteJson);
////    Log.i(TAG, "jsonStr: |" + jsonStr + "|");

        Intent intent = new Intent((Context) mActivity, NationalCancerInstituteListActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.putExtra(NationalCancerInstituteListActivity.QUERY, mQuery);
        intent.putExtra(NationalCancerInstituteListActivity.NATIONAL_CANCER_INSTITUTE, jsonStr);
        mActivity.startActivity(intent);
    }
}
