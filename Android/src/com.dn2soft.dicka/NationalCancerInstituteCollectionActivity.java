package com.dn2soft.dicka;

import com.dn2soft.dicka.NationalCancerInstituteContract.NationalCancerInstituteJson;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS
import android.app.Activity;
import android.util.Log;

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

import android.view.MenuItem;

public class NationalCancerInstituteCollectionActivity extends FragmentActivityEx {
    private static final String TAG = "NationalCancerInstituteCollectionActivity";

    public static final String TABINDEX = "NationalCancerInstituteCollectionActivity: Tab Index";

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

    String mQuery;
    String mNationalCancerInstituteJsonStr;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
/*
        boolean b = requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        Log.i(TAG, "requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS): " + b);
        if (b)
            setProgressBarIndeterminateVisibility(false);
 */

        Intent intent = getIntent();
        int tabIndex = (int) intent.getLongExtra(TABINDEX, 0);
        mQuery = intent.getStringExtra(NationalCancerInstituteListActivity.QUERY);
        mNationalCancerInstituteJsonStr = intent.getStringExtra(NationalCancerInstituteListActivity.NATIONAL_CANCER_INSTITUTE);
        NationalCancerInstituteJson nationalCancerInstituteJson = NationalCancerInstituteContract.setNationalCancerInstituteJson(mNationalCancerInstituteJsonStr);
        List<String>    jsonStrList = new ArrayList<String>();
        if (nationalCancerInstituteJson != null) {
            for (NationalCancerInstituteJson.TermsDef termsDef: nationalCancerInstituteJson.termsDefList) {
                String termsDefJsonStr = NationalCancerInstituteContract.setTermsDefJsonStr(termsDef);
                jsonStrList.add(termsDefJsonStr);
            }
        }
        setContentView(R.layout.activity_collection_demo);

        // Create an adapter that when requested, will return a fragment representing an object in
        // the collection.
        // 
        // ViewPager and its adapters use support library fragments, so we must use
        // getSupportFragmentManager.
        mDemoCollectionPagerAdapter = new DemoCollectionPagerAdapter(getSupportFragmentManager(), jsonStrList, nationalCancerInstituteJson);

        // Set up action bar.
        final ActionBar actionBar = getActionBar();

        // Specify that the Home button should show an "Up" caret, indicating that touching the
        // button will take the user one step up in the application's hierarchy.
        actionBar.setDisplayHomeAsUpEnabled(true);

        // Set up the ViewPager, attaching the adapter.
        mViewPager = (ViewPager) findViewById(R.id.pager);
        mViewPager.setAdapter(mDemoCollectionPagerAdapter);
        mViewPager.setCurrentItem(tabIndex);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
                // This is called when the Home (Up) button is pressed in the action bar.
                // Create a simple intent that starts the hierarchical parent activity and
                // use NavUtils in the Support Package to ensure proper handling of Up.
                Intent upIntent = new Intent(this, NationalCancerInstituteListActivity.class);
                upIntent.putExtra(NationalCancerInstituteListActivity.QUERY, mQuery);
                upIntent.putExtra(NationalCancerInstituteListActivity.NATIONAL_CANCER_INSTITUTE, mNationalCancerInstituteJsonStr);
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
        final NationalCancerInstituteJson mNationalCancerInstituteJson;
        public DemoCollectionPagerAdapter(FragmentManager fm, List<String> jsonStrList, NationalCancerInstituteJson nationalCancerInstituteJson) {
            super(fm);

            mJsonStrList = jsonStrList;
            mNationalCancerInstituteJson = nationalCancerInstituteJson;
        }

        @Override
        public Fragment getItem(int i) {
            Fragment fragment = new NationalCancerInstituteDisplayFragment();
            Bundle args = new Bundle();
////        args.putInt(DemoObjectFragment.ARG_OBJECT, i + 1); // Our object is just an integer :-P
            args.putString(NationalCancerInstituteDisplayFragment.ARG_OBJECT, mJsonStrList.get(i));
            fragment.setArguments(args);
            return fragment;
        }

        @Override
        public int getCount() {
            return mNationalCancerInstituteJson.termsDefList.size();
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return mNationalCancerInstituteJson.termsDefList.get(position).terms;
        }
    }
}
