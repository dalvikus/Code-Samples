package com.dn2soft.dicka;

import com.dn2soft.dicka.WordSmartContract.WordSmartJson.TheWords;
import android.app.DialogFragment;
//import android.app.FragmentTransaction;
/*
/disk/android/sdk/samples/android-16/ApiDemos/src/com/example/android/apis/app/AlertDialogSamples.java
/disk/android/sdk/samples/android-16/ApiDemos/src/com/example/android/apis/app/FragmentDialog.java
/disk/android/sdk/samples/android-16/ApiDemos/src/com/example/android/apis/view/ExpandableList3.java
/disk/android/sdk/samples/android-16/ApiDemos/res/layout/list_12.xml
/disk/android/sdk/samples/android-16/ApiDemos/res/layout/fragment_menu.xml
http://developer.android.com/guide/topics/ui/dialogs.html
http://developer.android.com/reference/android/app/DialogFragment.html
 */
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.PagerTitleStrip;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS
import android.util.Log;
import android.content.Context;

import java.util.List;
import java.util.ArrayList;

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;
import android.support.v4.app.NavUtils;
import android.support.v4.app.TaskStackBuilder;
import android.support.v4.view.ViewPager;
import android.view.View;

import android.view.MenuItem;

public class WordSmartCollectionActivity extends FragmentActivityEx {
    private static final String TAG = "WordSmartCollectionActivity";

    public static final String TABINDEX = "WordSmartCollectionActivity: Tab Index";


    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide fragments representing
     * each object in a collection. We use a {@link android.support.v4.app.FragmentStatePagerAdapter}
     * derivative, which will destroy and re-create fragments as needed, saving and restoring their
     * state in the process. This is important to conserve memory and is a best practice when
     * allowing navigation between objects in a potentially large collection.
     */
    DemoCollectionPagerAdapter mDemoCollectionPagerAdapter;

    /**
     * The {@link android.support.v4.view.ViewPager} that will display the object collection.
     */
    ViewPager mViewPager;

    // these five below are necessary to communicate "caller" activity (MainDisplayActivity) at android.R.id.home; actually only WordSmartJson.theWordsList is mandatory
    String mCambridgeResultBaseJsonStr;
    String mCambridgeResultJsonStr;
    String mWordSmartJsonStr;
    String mEnglishKoreanJsonStr;

    String mLabel = null;       // != null means it comes from LabelListActivity
    String mBookUnitStr = null; // != null means it comes from BookListActivity
    String mRootStr = null;     // != null means it comes from RootListActivity
                                // if all are null, it comes from MainDisplayActivity
    ArrayList<String> mTheWordsJsonStrArrayList;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
/*
        boolean b = requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        Log.i(TAG, "requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS): " + b);
        if (b)
            setProgressBarIndeterminateVisibility(false);
 */
        getActionBar().setSubtitle("long-press TAB to make a label");

        Intent intent = getIntent();
        int tabIndex = (int) intent.getLongExtra(TABINDEX, 0);
        // the following five is necessary to communicate with caller (MainDisplayActivity) at android.R.id.home
        mCambridgeResultBaseJsonStr = intent.getStringExtra(MainDisplayActivity.CAMBRIDGERESULTBASE);
        mCambridgeResultJsonStr = intent.getStringExtra(MainDisplayActivity.CAMBRIDGERESULT);
        mWordSmartJsonStr = intent.getStringExtra(MainDisplayActivity.WORDSMART);
        mEnglishKoreanJsonStr = intent.getStringExtra(MainDisplayActivity.ENGLISHKOREAN);
        // only WordSmartJson.theWordsList is mandatory
        mLabel = intent.getStringExtra(LabelListActivity.LABEL);
        mBookUnitStr = intent.getStringExtra(BookListActivity.BOOK_UNIT_STR);
        mRootStr = intent.getStringExtra(RootListActivity.ROOT_STR);
        mTheWordsJsonStrArrayList = intent.getStringArrayListExtra(LabelListActivity.THE_WORDS_LIST);
        List<TheWords> theWordsList = null;
        if (mTheWordsJsonStrArrayList != null) {
            theWordsList = new ArrayList<TheWords>();
            for (String theWordsJsonStr: mTheWordsJsonStrArrayList)
                theWordsList.add(WordSmartContract.setTheWords(theWordsJsonStr));
        }
        WordSmartContract.WordSmartJson wordSmartJson = null;
        if (mLabel == null && mBookUnitStr == null && mRootStr == null)
            wordSmartJson = WordSmartContract.setWordSmartJson(mWordSmartJsonStr);
        setContentView(R.layout.activity_collection_demo);
        
            //((TextView)view.findViewById(R.id.text)).setText(item);

        // Create an adapter that when requested, will return a fragment representing an object in
        // the collection.
        // 
        // ViewPager and its adapters use support library fragments, so we must use
        // getSupportFragmentManager.
        mDemoCollectionPagerAdapter = new DemoCollectionPagerAdapter(getSupportFragmentManager(), (mLabel == null && mBookUnitStr == null && mRootStr == null) ? wordSmartJson.theWordsList : theWordsList);

        // Set up action bar.
        final ActionBar actionBar = getActionBar();

        // Specify that the Home button should show an "Up" caret, indicating that touching the
        // button will take the user one step up in the application's hierarchy.
        actionBar.setDisplayHomeAsUpEnabled(true);

        // Set up the ViewPager, attaching the adapter.
        mViewPager = (ViewPager) findViewById(R.id.pager);
        mViewPager.setAdapter(mDemoCollectionPagerAdapter);
        mViewPager.setCurrentItem(tabIndex);

        PagerTitleStrip pagerTitleStrip = (PagerTitleStrip) mViewPager.findViewById(R.id.pager_title_strip);
        final ViewPager vp = mViewPager;
        pagerTitleStrip.setOnLongClickListener(
            new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View pView) {
                    // Do something when your hold starts here.
////                Log.i(TAG, "hello, world: |" + pView + "|");
                    PagerAdapter pageAdapter = vp.getAdapter();
////                Log.i(TAG, "" + pageAdapter);
                    int index = vp.getCurrentItem();
////                Log.i(TAG, "index = " + index);
                    String word = pageAdapter.getPageTitle(index).toString();
////                Log.i(TAG, word);
                    showDialog(word);
                    return true;
                }
            }
        );
    }

    private void simulateBack()
    {
        Intent upIntent = null;
        if (mLabel != null) {
            upIntent = new Intent(this, LabelListActivity.class);
            upIntent.putExtra(LabelListActivity.LABEL, mLabel);
            upIntent.putStringArrayListExtra(LabelListActivity.THE_WORDS_LIST, mUpdateDataList.size() > 0 ? null : mTheWordsJsonStrArrayList);
        } else if (mBookUnitStr != null) {
            upIntent = new Intent(this, BookListActivity.class);
            upIntent.putExtra(BookListActivity.BOOK_UNIT_STR, mBookUnitStr);
            upIntent.putStringArrayListExtra(BookListActivity.THE_WORDS_LIST, mTheWordsJsonStrArrayList);
        } else if (mRootStr != null) {
            upIntent = new Intent(this, RootListActivity.class);
            upIntent.putExtra(RootListActivity.ROOT_STR, mRootStr);
            upIntent.putStringArrayListExtra(RootListActivity.THE_WORDS_LIST, mTheWordsJsonStrArrayList);
        } else {
            upIntent = new Intent(this, MainDisplayActivity.class);
            upIntent.putExtra(MainDisplayActivity.TABINDEX, MainDisplayActivity.TAB_WORDSMART_THEWORDS);
            upIntent.putExtra(MainDisplayActivity.CAMBRIDGERESULTBASE, mCambridgeResultBaseJsonStr);
            upIntent.putExtra(MainDisplayActivity.CAMBRIDGERESULT, mCambridgeResultJsonStr);
            upIntent.putExtra(MainDisplayActivity.WORDSMART, mWordSmartJsonStr);
            upIntent.putExtra(MainDisplayActivity.ENGLISHKOREAN, mEnglishKoreanJsonStr);
        }
        if (NavUtils.shouldUpRecreateTask(this, upIntent)) {
            // This activity is not part of the application's task, so create a new task
            // with a synthesized back stack.
////                TaskStackBuilder.from(this)
            TaskStackBuilder.create(this)
                    // If there are ancestor activities, they should be added here.
                    .addNextIntent(upIntent)
                    .startActivities();
            finish();
        } else {
            // This activity is part of the application's task, so simply
            // navigate up to the hierarchical parent activity.
            NavUtils.navigateUpTo(this, upIntent);
        }
    }
    @Override
    public void onBackPressed()
    {
        simulateBack();
    }

    private ArrayList<String> mUpdateDataList = new ArrayList<String>();
    void showDialog(String word) {
/*
        // DialogFragment.show() will take care of adding the fragment
        // in a transaction.  We also want to remove any currently showing
        // dialog, so make our own transaction and take care of that here.
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        Fragment prev = getFragmentManager().findFragmentByTag("dialog");
        if (prev != null) {
            ft.remove(prev);
        }
        ft.addToBackStack(null);
 */
        // Create and show the dialog.
        mUpdateDataList.clear();
        DialogFragment newFragment = LabelDialogFragment.newInstance(word, mUpdateDataList);
        newFragment.show(getFragmentManager(), "label dialog");
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            simulateBack();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /**
     * A {@link android.support.v4.app.FragmentStatePagerAdapter} that returns a fragment
     * representing an object in the collection.
     */
    public static class DemoCollectionPagerAdapter extends FragmentStatePagerAdapter {

        final List<String> mJsonStrList;
        final List<WordSmartContract.WordSmartJson.TheWords> mTheWordsList;
        public DemoCollectionPagerAdapter(FragmentManager fm, List<WordSmartContract.WordSmartJson.TheWords> theWordsList) {
            super(fm);

            mTheWordsList = theWordsList;
            mJsonStrList = new ArrayList<String>();
            for (WordSmartContract.WordSmartJson.TheWords theWords: theWordsList) {
////            Log.i(TAG, "WSW: |" + theWords.word + "|: |" + theWords.sense + "|");
                String theWordsJsonStr = WordSmartContract.setTheWordsJsonStr(theWords);
                mJsonStrList.add(theWordsJsonStr);
            }
        }

        @Override
        public Fragment getItem(int i) {
            WordSmartContract.WordSmartJson.TheWords theWords = mTheWordsList.get(i);
            if (theWords.sense == null) {
                Fragment fragment = new CambridgeDisplayFragment();
                Bundle args = new Bundle();
                args.putString(CambridgeDisplayFragment.ARG_OBJECT_WORD_STR, theWords.word);
                //args.putString(CambridgeDisplayFragment.ARG_OBJECT_JSON_STR, mCambridgeResultJsonStr);
                fragment.setArguments(args);
                return fragment;
            }
            Fragment fragment = new WordSmartDisplayFragment();
            Bundle args = new Bundle();
////        args.putInt(DemoObjectFragment.ARG_OBJECT, i + 1); // Our object is just an integer :-P
            args.putString(WordSmartDisplayFragment.ARG_OBJECT, mJsonStrList.get(i));
            fragment.setArguments(args);
            return fragment;
        }

        @Override
        public int getCount() {
            return mTheWordsList.size();
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return mTheWordsList.get(position).word;
        }
    }
}
