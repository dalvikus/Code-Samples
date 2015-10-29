package com.dn2soft.dicka;

import com.dn2soft.dicka.WordSmartContract.WordSmartJson.TheWords;
import android.content.Intent;
import com.dn2soft.dicka.LoadActivity.Entry;

import java.util.List;
import java.util.ArrayList;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS

//// activity (Context, Activity, Bundle) but through (FragmentManager) fragment (ListFragment)
import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.app.FragmentManager;

// logging...
import android.util.Log;

import android.widget.ListView;

// "common" views...
import android.view.View;


/**
 * main activity
 */
public class LabelListActivity extends Activity {
    private static final String TAG = "LabelListActivity";

    public static final String LABEL = "Label";
    public static final String THE_WORDS_LIST = "List<WordSmartContract.WordSmartJson.TheWords>";

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
        getActionBar().setSubtitle("long-press any items to manage labels");

        Intent intent = getIntent();
        String label = intent.getStringExtra(LABEL);
        ArrayList<String> theWordsJsonStrArrayList = intent.getStringArrayListExtra(THE_WORDS_LIST);
////    Log.i(TAG, "nationalCancerInstituteJsonStr: |" + nationalCancerInstituteJsonStr + "|");

        FragmentManager fm = getFragmentManager();

        // Create the list fragment and add it as our sole content.
        if (fm.findFragmentById(android.R.id.content) == null) {
            //AppListFragment list = new AppListFragment(nationalCancerInstituteJsonStr);
            AppListFragment list = AppListFragment.newInstance(
                label,
                theWordsJsonStrArrayList
            );
            fm.beginTransaction().add(android.R.id.content, list).commit();
        }
    }



    public static class AppListFragment extends ListFragmentEx {

        // This is the Adapter being used to display the list's data.
        private ArrayAdapterEx mAdapter;

        String mLabel;
        ArrayList<String> mTheWordsJsonStrArrayList;

        public AppListFragment() {}     // should be empty

        static AppListFragment newInstance(
            String label,
            ArrayList<String> theWordsJsonStrArrayList
        ) {
            AppListFragment f = new AppListFragment();

            Bundle args = new Bundle();
            args.putString(LABEL, label);
            args.putStringArrayList(THE_WORDS_LIST, theWordsJsonStrArrayList);

            f.setArguments(args);
            return f;
        }

        @Override public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            Bundle args = getArguments();
            mLabel = args.getString(LABEL);
            mTheWordsJsonStrArrayList = args.getStringArrayList(THE_WORDS_LIST);
////        Log.i(TAG, "mNationalCancerInstituteJsonStr: |" + mNationalCancerInstituteJsonStr + "|");
////        Log.i(TAG, "mNationalCancerInstituteJson: " + mNationalCancerInstituteJson);
        }
        // If non-null, this is the current filter the user has provided.

        @Override public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            setEmptyText("No words in label " + mLabel);

            // We have a menu item to show in action bar.
            setHasOptionsMenu(true);

            // Create an empty adapter we will use to display the loaded data.
            mAdapter = new ArrayAdapterEx((Context) getActivity(), ArrayAdapterEx.TYPE_TWO_ITEMS);
            setListAdapter(mAdapter);

            if (mTheWordsJsonStrArrayList != null) {
                //Log.i(TAG, "reuse");
                List<Entry> entries = new ArrayList<Entry>();
                for (String theWordsJsonStr: mTheWordsJsonStrArrayList) {
                    TheWords theWords = WordSmartContract.setTheWords(theWordsJsonStr);
                    String sense = theWords.sense;
                    if (sense == null)
                        sense = "not found in Word Smart";
                    entries.add(new Entry(theWords.word, sense));
                }
                mAdapter.setData(entries);
            } else {
                //Log.i(TAG, "reconstruct");
                mTheWordsJsonStrArrayList = new ArrayList<String>();
                new LabelLoad(getActivity(), mLabel, mAdapter, mTheWordsJsonStrArrayList).execute();
            }
        }

        @Override
        public void onListItemClick(ListView l, View v, int position, long id)
        {
            // Insert desired behavior here.
////        Log.i(TAG, "Item clicked: " + id);
            Intent intent = new Intent((Context) getActivity(), WordSmartCollectionActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            intent.putExtra(WordSmartCollectionActivity.TABINDEX, id);
            // called back in "case android.R.id.home:"
            intent.putExtra(LABEL, mLabel);
            intent.putStringArrayListExtra(THE_WORDS_LIST, mTheWordsJsonStrArrayList);
            getActivity().startActivity(intent);
        }
    }
}
