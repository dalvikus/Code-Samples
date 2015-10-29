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
public class RootListActivity extends Activity {
    private static final String TAG = "RootListActivity";

    public static final String ROOT_STR = "Root.rootStr(root0)";
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
        Intent intent = getIntent();
        String rootStr = intent.getStringExtra(RootListActivity.ROOT_STR);
        ArrayList<String> theWordsJsonStrArrayList = intent.getStringArrayListExtra(RootListActivity.THE_WORDS_LIST);
////    Log.i(TAG, "nationalCancerInstituteJsonStr: |" + nationalCancerInstituteJsonStr + "|");

        ((Activity) this).getActionBar().setSubtitle("<<" + rootStr + ">>");

        FragmentManager fm = getFragmentManager();

        // Create the list fragment and add it as our sole content.
        if (fm.findFragmentById(android.R.id.content) == null) {
            RootListFragment list = RootListFragment.newInstance(
                rootStr,
                theWordsJsonStrArrayList
            );
            fm.beginTransaction().add(android.R.id.content, list).commit();
        }
    }


    public static class RootListFragment extends ListFragmentEx {

        // This is the Adapter being used to display the list's data.
        private ArrayAdapterEx mAdapter;

        String mRootStr;
        ArrayList<String> mTheWordsJsonStrArrayList;

        public RootListFragment() {}     // should be empty

        static RootListFragment newInstance(
            String rootStr,
            ArrayList<String> theWordsJsonStrArrayList
        ) {
            RootListFragment f = new RootListFragment();

            Bundle args = new Bundle();
            args.putString(RootListActivity.ROOT_STR, rootStr);
            args.putStringArrayList(RootListActivity.THE_WORDS_LIST, theWordsJsonStrArrayList);

            f.setArguments(args);
            return f;
        }

        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            Bundle args = getArguments();
            mRootStr = args.getString(RootListActivity.ROOT_STR);
            mTheWordsJsonStrArrayList = args.getStringArrayList(RootListActivity.THE_WORDS_LIST);
        }
        // If non-null, this is the current filter the user has provided.

        @Override
        public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            setEmptyText("No words in root: |" + mRootStr + "|");

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
            }
        }

        @Override
        public void onListItemClick(ListView l, View v, int position, long id)
        {
            Intent intent = new Intent((Context) getActivity(), WordSmartCollectionActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            intent.putExtra(WordSmartCollectionActivity.TABINDEX, id);
            intent.putExtra(RootListActivity.ROOT_STR, mRootStr);
            intent.putStringArrayListExtra(RootListActivity.THE_WORDS_LIST, mTheWordsJsonStrArrayList);
            getActivity().startActivity(intent);
        }
    }
}
