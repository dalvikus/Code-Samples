package com.dn2soft.dicka;

import com.dn2soft.dicka.WordSmartContract.WordSmartJson.TheWords;
import android.content.Intent;
import com.dn2soft.dicka.LoadActivity.Entry;

// for Data
import java.util.List;
import java.util.ArrayList;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS

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
public class BookListActivity extends Activity {
    private static final String TAG = "BookListActivity";

    public static final String BOOK_UNIT_STR = "Book.Unit";
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
        String bookUnitStr = intent.getStringExtra(BookListActivity.BOOK_UNIT_STR);
        ArrayList<String> theWordsJsonStrArrayList = intent.getStringArrayListExtra(BookListActivity.THE_WORDS_LIST);
////    Log.i(TAG, "nationalCancerInstituteJsonStr: |" + nationalCancerInstituteJsonStr + "|");

        ((Activity) this).getActionBar().setSubtitle("<" + bookUnitStr + ">");

        FragmentManager fm = getFragmentManager();

        // Create the list fragment and add it as our sole content.
        if (fm.findFragmentById(android.R.id.content) == null) {
            BookListFragment list = BookListFragment.newInstance(
                bookUnitStr,
                theWordsJsonStrArrayList
            );
            fm.beginTransaction().add(android.R.id.content, list).commit();
        }
    }


    public static class BookListFragment extends ListFragmentEx {

        // This is the Adapter being used to display the list's data.
        private ArrayAdapterEx mAdapter;

        String mBookUnitStr;
        ArrayList<String> mTheWordsJsonStrArrayList;

        public BookListFragment() {}     // should be empty

        static BookListFragment newInstance(
            String bookUnitStr,
            ArrayList<String> theWordsJsonStrArrayList
        ) {
            BookListFragment f = new BookListFragment();

            Bundle args = new Bundle();
            args.putString(BookListActivity.BOOK_UNIT_STR, bookUnitStr);
            args.putStringArrayList(BookListActivity.THE_WORDS_LIST, theWordsJsonStrArrayList);

            f.setArguments(args);
            return f;
        }

        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            Bundle args = getArguments();
            mBookUnitStr = args.getString(BookListActivity.BOOK_UNIT_STR);
            mTheWordsJsonStrArrayList = args.getStringArrayList(BookListActivity.THE_WORDS_LIST);
        }
        // If non-null, this is the current filter the user has provided.

        @Override
        public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            setEmptyText("No words in <#" + mBookUnitStr + ">");

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
                    entries.add(new Entry(theWords.word, "        " + theWords.sense));
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
            intent.putExtra(BookListActivity.BOOK_UNIT_STR, mBookUnitStr);
            intent.putStringArrayListExtra(BookListActivity.THE_WORDS_LIST, mTheWordsJsonStrArrayList);
            getActivity().startActivity(intent);
        }
    }
}
