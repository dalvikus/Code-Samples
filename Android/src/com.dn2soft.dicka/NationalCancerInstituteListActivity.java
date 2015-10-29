package com.dn2soft.dicka;

import android.content.Intent;
import com.dn2soft.dicka.LoadActivity.Entry;
import com.dn2soft.dicka.NationalCancerInstituteContract.NationalCancerInstituteJson;

// for Data
import java.util.List;
import java.util.ArrayList;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS

// used for getDickRootPath
import java.io.File;

//// activity (Context, Activity, Bundle) but through (FragmentManager) fragment (ListFragment)
import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.app.FragmentManager;

// logging...
import android.util.Log;

// in ArrayAdapter class, to override getView
import android.widget.ListView;

// "common" views...
import android.view.View;


/**
 * main activity
 */
public class NationalCancerInstituteListActivity extends Activity {
    private static final String TAG = "NationalCancerInstituteListActivity";

    public static final String QUERY = "National Cancer Institute: Query";
    public static final String NATIONAL_CANCER_INSTITUTE = "National Cancer Institute";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
/*
        boolean b = requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        Log.i(TAG, "requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS): " + b);
        if (b)
            setProgressBarIndeterminateVisibility(false);
 */
        Intent intent = getIntent();
        String query = intent.getStringExtra(QUERY);
        String nationalCancerInstituteJsonStr = intent.getStringExtra(NATIONAL_CANCER_INSTITUTE);
////    Log.i(TAG, "nationalCancerInstituteJsonStr: |" + nationalCancerInstituteJsonStr + "|");

        FragmentManager fm = getFragmentManager();

        // Create the list fragment and add it as our sole content.
        if (fm.findFragmentById(android.R.id.content) == null) {
            //AppListFragment list = new AppListFragment(nationalCancerInstituteJsonStr);
            AppListFragment list = AppListFragment.newInstance(
                query,
                nationalCancerInstituteJsonStr
            );
            fm.beginTransaction().add(android.R.id.content, list).commit();
        }
    }



    public static class AppListFragment extends ListFragmentEx {

        // This is the Adapter being used to display the list's data.
        private ArrayAdapterEx mAdapter;

        String mQuery;
        String mNationalCancerInstituteJsonStr;
        NationalCancerInstituteJson mNationalCancerInstituteJson;
        public AppListFragment() {}     // should be empty

        static AppListFragment newInstance(
            String query,
            String nationalCancerInstituteJsonStr
        ) {
            AppListFragment f = new AppListFragment();

            Bundle args = new Bundle();
            args.putString(QUERY, query);
            args.putString(NATIONAL_CANCER_INSTITUTE, nationalCancerInstituteJsonStr);

            f.setArguments(args);
            return f;
        }
        @Override public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            Bundle args = getArguments();
            mQuery = args.getString(QUERY);
            mNationalCancerInstituteJsonStr = args.getString(NATIONAL_CANCER_INSTITUTE);
            mNationalCancerInstituteJson = NationalCancerInstituteContract.setNationalCancerInstituteJson(mNationalCancerInstituteJsonStr);
////        Log.i(TAG, "mNationalCancerInstituteJsonStr: |" + mNationalCancerInstituteJsonStr + "|");
////        Log.i(TAG, "mNationalCancerInstituteJson: " + mNationalCancerInstituteJson);
        }
        // If non-null, this is the current filter the user has provided.

        @Override public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            // Give some text to display if there is no data.  In a real
            // application this would come from a resource.
            boolean termsOnly = mQuery.startsWith("!");
            String query = termsOnly ? mQuery.substring(1).trim() : mQuery;
            String msg = String.format(
                "Not Found: |%s| in %s"
                ,
                query,
                termsOnly ? "Terms only" : "all context"
            );
            setEmptyText(msg);

            // We have a menu item to show in action bar.
            setHasOptionsMenu(true);

            // Create an empty adapter we will use to display the loaded data.
            mAdapter = new ArrayAdapterEx((Context) getActivity(), ArrayAdapterEx.TYPE_TWO_ITEMS);
            setListAdapter(mAdapter);

            List<Entry> entries = new ArrayList<Entry>();
            if (mNationalCancerInstituteJson != null) {
                for (NationalCancerInstituteJson.TermsDef termsDef: mNationalCancerInstituteJson.termsDefList) {
                    entries.add(new Entry(termsDef.terms, "        " + termsDef.snippet));
                }
            }
            mAdapter.setData(entries);
        }

        @Override
        public void onListItemClick(ListView l, View v, int position, long id)
        {
            // Insert desired behavior here.
////        Log.i(TAG, "Item clicked: " + id);
            Intent intent = new Intent((Context) getActivity(), NationalCancerInstituteCollectionActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            intent.putExtra(NationalCancerInstituteCollectionActivity.TABINDEX, id);
            // called back in "case android.R.id.home:"
            intent.putExtra(QUERY, mQuery);
            intent.putExtra(NATIONAL_CANCER_INSTITUTE, mNationalCancerInstituteJsonStr);
            getActivity().startActivity(intent);
        }
    }
}
