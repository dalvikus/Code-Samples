package com.dn2soft.dicka;

import android.hardware.SensorManager;
import android.hardware.Sensor;

import android.view.MotionEvent;
import android.support.v4.view.GestureDetectorCompat;

import com.dn2soft.dickc.dictionary.Cambridge;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS
import android.app.Activity;
import com.dn2soft.dicka.LoadActivity.Entry;

import android.widget.ListView;
import java.util.ArrayList;
import java.util.List;
import android.support.v4.app.ListFragment;
import android.content.Context;
import android.util.Log;

import android.app.ActionBar;
import android.app.FragmentTransaction;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.View;

import android.content.res.Resources;
import android.preference.PreferenceManager;
import android.content.SharedPreferences;

public class MainDisplayActivity extends FragmentActivityEx implements ActionBar.TabListener {
    private static final String TAG = "MainDisplayActivity";

    public static final String TABINDEX             = "MainDisplayActivity: Tab Index";
    public static final String CAMBRIDGERESULTBASE  = "Cambridge.ResultBase";
    public static final String CAMBRIDGERESULT      = "Cambridge.Result";
    public static final String WORDSMART            = "Word Smart";
    public static final String ENGLISHKOREAN        = "English Korean";

    public static final int     TAB_NONE                    = -1;
    public static final int     TAB_CAMBRIDGE               = 0;
    public static final int     TAB_WORDSMART_THEWORDS      = 1;
    public static final int     TAB_ENGLISHKOREAN           = 2;
    public static final int     TAB_WORDSMART_ROOT          = 3;
    public static final int     TAB_WORDSMART_THEHITPARAGE  = 4;
    private static final int    TAB_LENGTH      = 5;    // number of TAB_XXX
    private static final String[] mTabTitles = new String[] {
        "Cambridge",
        "Word Smart: The Words",
        "English Korean",
        "Word Smart: Root",
        "Word Smart: The Hit Parage"
    };

    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide fragments for each of the
     * three primary sections of the app. We use a {@link android.support.v4.app.FragmentPagerAdapter}
     * derivative, which will keep every loaded fragment in memory. If this becomes too memory
     * intensive, it may be best to switch to a {@link android.support.v4.app.FragmentStatePagerAdapter}.
     */
    AppSectionsPagerAdapter mAppSectionsPagerAdapter;

    /**
     * The {@link ViewPager} that will display the three primary sections of the app, one at a
     * time.
     */
    ViewPager mViewPager;

    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private Sensor mGravity;
    private ShakeDetector mShakeDetector;
    private static boolean mByShakeDefaultValue;
    @Override
    public void onResume()
    {
        super.onResume();

        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences((Context) this);
        boolean byShake = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
        if (byShake) {
            mSensorManager.registerListener(mShakeDetector, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);//SENSOR_DELAY_UI);
            mSensorManager.registerListener(mShakeDetector, mGravity, SensorManager.SENSOR_DELAY_UI);
        }
    }
    @Override
    public void onPause() {
        // Add the following line to unregister the Sensor Manager onPause
        super.onPause();

        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences((Context) this);
        boolean byShake = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
        if (byShake) {
            mSensorManager.unregisterListener(mShakeDetector);
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final Context context = (Context) this;
/*
        boolean b = requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        Log.i(TAG, "" + this);
        if (b)
            setProgressBarIndeterminateVisibility(false);
 */
        setContentView(R.layout.activity_main);

        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        Resources res = context.getResources();
        mByShakeDefaultValue = res.getBoolean(R.bool.pref_defaultValue_byShake);

        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mGravity = mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);
        mShakeDetector = new ShakeDetector(context, mGravity != null);
        mShakeDetector.setOnShakeListener(new ShakeDetector.OnShakeListener() {
            @Override
            public void onShake(int count) {
                Log.i(TAG, "# of shakes = " + count);
                SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
                boolean byShake = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
            }
        });

        Intent intent = getIntent();
        int tabIndex = intent.getIntExtra(TABINDEX, 0);
////    Log.i(TAG, "tabIndex = " + tabIndex);
        String cambridgeResultBaseJsonStr   = intent.getStringExtra(CAMBRIDGERESULTBASE);
////    Log.i(TAG, "cambridgeResultBaseJsonStr: |" + cambridgeResultBaseJsonStr + "|");
        String cambridgeResultJsonStr       = intent.getStringExtra(CAMBRIDGERESULT);
////    Log.i(TAG, "cambridgeResultJsonStr: |" + cambridgeResultJsonStr + "|");
        String wordSmartJsonStr             = intent.getStringExtra(WORDSMART);
////    Log.i(TAG, "wordSmartJsonStr: |" + wordSmartJsonStr + "|");
        String englishKoreanJsonStr         = intent.getStringExtra(ENGLISHKOREAN);
////    Log.i(TAG, "englishKoreanJsonStr: |" + englishKoreanJsonStr + "|");

        // Create the adapter that will return a fragment for each of the three primary sections
        // of the app.
        mAppSectionsPagerAdapter = new AppSectionsPagerAdapter(
            getSupportFragmentManager()
            ,
            cambridgeResultBaseJsonStr,
            cambridgeResultJsonStr,
            wordSmartJsonStr,
            englishKoreanJsonStr
        );

        // Set up the action bar.
        final ActionBar actionBar = getActionBar();

        // Specify that the Home/Up button should not be enabled, since there is no hierarchical
        // parent.
        actionBar.setHomeButtonEnabled(false);

        // Specify that we will be displaying tabs in the action bar.
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        // Set up the ViewPager, attaching the adapter and setting up a listener for when the
        // user swipes between sections.
        mViewPager = (ViewPager) findViewById(R.id.pager);
/*
        final GestureDetectorCompat detector = new GestureDetectorCompat((Context) this, new LoadActivity.MyGestureListener());

        mViewPager.setOnTouchListener(
            new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    return detector.onTouchEvent(event);
                }
            }
        );
 */
        mViewPager.setAdapter(mAppSectionsPagerAdapter);
        mViewPager.setOnPageChangeListener(
            new ViewPager.SimpleOnPageChangeListener() {
                @Override
                public void onPageSelected(int position) {
                    // When swiping between different app sections, select the corresponding tab.
                    // We can also use ActionBar.Tab#select() to do this if we have a reference to the
                    // Tab.
                    //Log.i(TAG, "onPageSelected[" + position + "]");
                    mIndexPagePrevSelected = mIndexPageSelected;
                    mIndexPageSelected = position;
                    actionBar.setSelectedNavigationItem(position);
                }
            }
        );

        // For each of the sections in the app, add a tab to the action bar.
        for (int i = 0; i < mAppSectionsPagerAdapter.getCount(); i++) {
            // Create a tab with text corresponding to the page title defined by the adapter.
            // Also specify this Activity object, which implements the TabListener interface, as the
            // listener for when this tab is selected.
            actionBar.addTab(
                actionBar.newTab()
                         .setText(mAppSectionsPagerAdapter.getPageTitle(i))
                         .setTabListener(this)
            );
        }
        mViewPager.setCurrentItem(tabIndex);
    }

    private int mIndexPageSelected = -1;
    private int mIndexPagePrevSelected = -1;
    @Override
    public void onTabUnselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
        //Log.i(TAG, "onTabUnselected[" + tab.getPosition() + "], mIndexPageSelected = " + mIndexPageSelected + ", mIndexPagePrevSelected = " + mIndexPagePrevSelected);
    }

    @Override
    public void onTabSelected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
        // When the given tab is selected, switch to the corresponding page in the ViewPager.
        //Log.i(TAG, "onTabSelected[" + tab.getPosition() + "], mIndexPageSelected = " + mIndexPageSelected + ", mIndexPagePrevSelected = " + mIndexPagePrevSelected);
        mViewPager.setCurrentItem(tab.getPosition());
        mIndexPagePrevSelected = mIndexPageSelected;
    }

    @Override
    public void onTabReselected(ActionBar.Tab tab, FragmentTransaction fragmentTransaction) {
    }

    /**
     * A {@link FragmentPagerAdapter} that returns a fragment corresponding to one of the primary
     * sections of the app.
     */
    public static class AppSectionsPagerAdapter extends FragmentPagerAdapter {

        final String mCambridgeResultBaseJsonStr;
        final String mCambridgeResultJsonStr;
        final String mWordSmartJsonStr;
        final String mEnglishKoreanJsonStr;
/*
        final Cambridge.ResultBase mResultBase;
        final Cambridge.Result mResult;
        final WordSmartContract.WordSmartJson mWordSmartJson;
        final EnglishKoreanContract.EnglishKoreanJson mEnglishKoreanJson;
 */
        public AppSectionsPagerAdapter(
            FragmentManager fm
            ,
            String cambridgeResultBaseJsonStr,
            String cambridgeResultJsonStr,
            String wordSmartJsonStr,
            String englishKoreanJsonStr
        ) {
            super(fm);

            mCambridgeResultBaseJsonStr = cambridgeResultBaseJsonStr;
            mCambridgeResultJsonStr = cambridgeResultJsonStr;
            mWordSmartJsonStr = wordSmartJsonStr;
            mEnglishKoreanJsonStr = englishKoreanJsonStr;
/*
            mResultBase = CambridgeContract.setCambridgeResultBase(cambridgeResultBaseJsonStr);
            Log.i(TAG, "ResultBase.wordStr: |" + mResultBase.wordStr + "|");
            Log.i(TAG, "ResultBase.WarnMessage: " + mResultBase.WarnMessage);
            Log.i(TAG, "ResultBase.ErrorMessageList: " + mResultBase.ErrorMessageList);
            Log.i(TAG, "ResultBase.tryWordList: " + mResultBase.tryWordList);
            mResult = CambridgeContract.setCambridgeResult(cambridgeResultJsonStr);
            Log.i(TAG, "Result.doc: " + mResult.doc);
            mWordSmartJson = WordSmartContract.setWordSmartJson(wordSmartJsonStr);
            Log.i(TAG, "WordSmartJson: " + mWordSmartJson);
            mEnglishKoreanJson = EnglishKoreanContract.setEnglishKoreanJson(englishKoreanJsonStr);
            Log.i(TAG, "EnglishKoreanJson: " + mEnglishKoreanJson);
 */
        }

        @Override
        public Fragment getItem(int i) {
            Cambridge.ResultBase resultBase = CambridgeContract.setCambridgeResultBase(mCambridgeResultBaseJsonStr);
            String wordStr = resultBase.wordStr;
            if (i == TAB_CAMBRIDGE && resultBase.tryWordList == null) {
                Cambridge.Result result = CambridgeContract.setCambridgeResult(mCambridgeResultJsonStr);
                if (result.doc.size() > 0) {
                    Fragment fragment = new CambridgeDisplayFragment();
                    Bundle args = new Bundle();
                    args.putString(CambridgeDisplayFragment.ARG_OBJECT_WORD_STR, resultBase.wordStr);
                    args.putString(CambridgeDisplayFragment.ARG_OBJECT_JSON_STR, mCambridgeResultJsonStr);
                    fragment.setArguments(args);
                    return fragment;
                } else {
                    String message = null;
                    if (resultBase.WarnMessage != null) {
                        message = resultBase.WarnMessage;
                    } else if (resultBase.ErrorMessageList != null) {
                        StringBuilder sb = new StringBuilder();
                        for (String err: resultBase.ErrorMessageList) {
                            sb.append(err + "\n");
                        }
                        message = sb.toString();
                    }
                    if (message == null) {
                        message = "Oops: |" + resultBase.wordStr + "|: check if Wi-Fi is set";
                    }
                    Fragment fragment = new BlueScreenFragment();
                    Bundle args = new Bundle();
                    args.putString(BlueScreenFragment.ARG_OBJECT_FATAL, message);
                    fragment.setArguments(args);
                    return fragment;
                }
            }
            Fragment f = AppListFragment.newInstance(
                i
                ,
                mCambridgeResultBaseJsonStr,
                mCambridgeResultJsonStr,
                mWordSmartJsonStr,
                mEnglishKoreanJsonStr
            );
            return f;
        }

        @Override
        public int getCount() {
            return TAB_LENGTH;
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return mTabTitles[position];
        }
    }


    public static class AppListFragment extends ListFragment
    {
        // This is the Adapter being used to display the list's data.
        ArrayAdapterEx mAdapter;
        // all below is set not in a constructor but in newInstance
        int mTabIndex;
        String mCambridgeResultBaseJsonStr;
        String mCambridgeResultJsonStr;
        String mWordSmartJsonStr;
        String mEnglishKoreanJsonStr;
        Cambridge.ResultBase mResultBase;
        Cambridge.Result mResult;
        WordSmartContract.WordSmartJson mWordSmartJson;
        EnglishKoreanContract.EnglishKoreanJson mEnglishKoreanJson;

        public AppListFragment() {}     // should be empty

        static AppListFragment newInstance(
            int index
            ,
            String cambridgeResultBaseJsonStr,
            String cambridgeResultJsonStr,
            String wordSmartJsonStr,
            String englishKoreanJsonStr
        ) {
            AppListFragment f = new AppListFragment();

            Bundle args = new Bundle();
            args.putInt("index", index);
            args.putString(CAMBRIDGERESULTBASE, cambridgeResultBaseJsonStr);
            args.putString(CAMBRIDGERESULT, cambridgeResultJsonStr);
            args.putString(WORDSMART, wordSmartJsonStr);
            args.putString(ENGLISHKOREAN, englishKoreanJsonStr);

            f.setArguments(args);
            return f;
        }
        @Override public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            Bundle args = getArguments();
            mTabIndex = args.getInt("index");
            mCambridgeResultBaseJsonStr = args.getString(CAMBRIDGERESULTBASE);
            mCambridgeResultJsonStr = args.getString(CAMBRIDGERESULT);
            mWordSmartJsonStr = args.getString(WORDSMART);
            mEnglishKoreanJsonStr = args.getString(ENGLISHKOREAN);
            mResultBase = CambridgeContract.setCambridgeResultBase(mCambridgeResultBaseJsonStr);
////        Log.i(TAG, "ResultBase.tryWordList: " + mResultBase.tryWordList);
            mResult = CambridgeContract.setCambridgeResult(mCambridgeResultJsonStr);
////        Log.i(TAG, "Result.doc: " + mResult.doc);
            mWordSmartJson = WordSmartContract.setWordSmartJson(mWordSmartJsonStr);
////        Log.i(TAG, "WordSmartJson: " + mWordSmartJson);
            mEnglishKoreanJson = EnglishKoreanContract.setEnglishKoreanJson(mEnglishKoreanJsonStr);
////        Log.i(TAG, "EnglishKoreanJson: " + mEnglishKoreanJson);
        }

        @Override public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            // Give some text to display if there is no data.  In a real
            // application this would come from a resource.
            setEmptyText("Not Found");

            // Create an empty adapter we will use to display the loaded data.
            int iType = mTabIndex == TAB_CAMBRIDGE ? ArrayAdapterEx.TYPE_ONE_ITEM : ArrayAdapterEx.TYPE_TWO_ITEMS;
            mAdapter = new ArrayAdapterEx((Context) getActivity(), iType);
            setListAdapter(mAdapter);

            List<Entry> entries = new ArrayList<Entry>();
            switch (mTabIndex) {
            case TAB_CAMBRIDGE:
                {
                    if (mResultBase.tryWordList != null) {
                        for (String word: mResultBase.tryWordList) {
                            entries.add(new Entry(word, null));
                        }
                    }
                }
                break;
            case TAB_WORDSMART_THEWORDS:
                {
                    if (mWordSmartJson != null) {
                        for (WordSmartContract.WordSmartJson.TheWords theWords: mWordSmartJson.theWordsList) {
////                        Log.i(TAG, "WSW: |" + theWords.word + "|: |" + theWords.sense + "|");
                            entries.add(new Entry(theWords.word, "        " + theWords.snippet));
                        }
                    }
                }
                break;
            case TAB_WORDSMART_THEHITPARAGE:
                {
                    if (mWordSmartJson != null) {
                        for (WordSmartContract.WordSmartJson.TheHitParade theHitParade: mWordSmartJson.theHitParadeList) {
////                        Log.i(TAG, "WSH: |" + theHitParade.word + "|: |" + theHitParade.sense + "|");
                            entries.add(new Entry(theHitParade.word, "        " + theHitParade.sense));
                        }
                    }
                }
                break;
            case TAB_WORDSMART_ROOT:
                {
                    if (mWordSmartJson != null) {
                        for (WordSmartContract.WordSmartJson.Root root: mWordSmartJson.rootList) {
////                        Log.i(TAG, "WSR: |" + root.root + "|: |" + root.sense + "|");
                            entries.add(new Entry(root.root, "        " + root.sense));
                            for (String w: root.wordList) {
                                entries.add(new Entry("    " + w, ""));
                            }
                        }
                    }
                }
                break;
            case TAB_ENGLISHKOREAN:
                {
                    if (mEnglishKoreanJson != null) {
                        for (EnglishKoreanContract.EnglishKoreanJson.EnglishKorean englishKorean: mEnglishKoreanJson.englishKoreanList) {
////                        Log.i(TAG, "EK: |" + englishKorean.word + "|: |" + englishKorean.sense + "|");
                            entries.add(new Entry(englishKorean.word, "        " + englishKorean.sense));
                        }
                    }
                }
                break;
            default:
                break;
            }
            mAdapter.setData(entries);
        }

        @Override public void onListItemClick(ListView l, View v, int position, long id) {
            // Insert desired behavior here.
            Activity activity = getActivity();
            if (mTabIndex == TAB_WORDSMART_THEWORDS && mWordSmartJson.theWordsList.size() > 0) {
                Intent intent = new Intent((Context) activity, WordSmartCollectionActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                intent.putExtra(WordSmartCollectionActivity.TABINDEX, id);
                // called back in "case android.R.id.home:"
                intent.putExtra(CAMBRIDGERESULTBASE, mCambridgeResultBaseJsonStr);
                intent.putExtra(CAMBRIDGERESULT, mCambridgeResultJsonStr);
                intent.putExtra(WORDSMART, mWordSmartJsonStr);
                intent.putExtra(ENGLISHKOREAN, mEnglishKoreanJsonStr);
                activity.startActivity(intent);
            } else if (mTabIndex == TAB_CAMBRIDGE) {
                Entry entry = (Entry) getListView().getItemAtPosition(position);
////            Log.i(TAG, "TAB_CAMBRIDGE: |" + entry.getWord() + "|: |" + entry.getSense() + "|");
                if (entry.getSense() == null) {
                    new Search(activity, entry.getWord()).execute();
                }
            } else if (mTabIndex == TAB_WORDSMART_THEHITPARAGE) {
                Entry entry = (Entry) getListView().getItemAtPosition(position);
////            Log.i(TAG, "TAB_WORDSMART_THEHITPARAGE: |" + entry.getWord() + "|: |" + entry.getSense() + "|");
                new Search(activity, entry.getWord()).execute();
            } else if (mTabIndex == TAB_WORDSMART_ROOT) {
                Entry entry = (Entry) getListView().getItemAtPosition(position);
////            Log.i(TAG, "TAB_WORDSMART_ROOT: |" + entry.getWord() + "|: |" + entry.getSense() + "|");
                if (entry.getSense().isEmpty()) {
                    new Search(activity, entry.getWord()).execute();
                }
            } else if (mTabIndex == TAB_ENGLISHKOREAN) {
                Entry entry = (Entry) getListView().getItemAtPosition(position);
////            Log.i(TAG, "TAB_WORDSMART_ROOT: |" + entry.getWord() + "|: |" + entry.getSense() + "|");
                new Search(activity, entry.getWord()).execute();
            }
        }
    }
}
