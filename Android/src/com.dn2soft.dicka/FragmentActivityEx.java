package com.dn2soft.dicka;

import android.app.SearchManager;
import android.widget.SearchView.OnSuggestionListener;
import android.widget.CursorAdapter;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS
import android.app.Activity;
import android.util.Log;
import android.content.Context;

import android.support.v4.app.FragmentActivity;

import android.view.Menu;
import android.view.MenuItem;
import android.widget.SearchView;
import android.widget.SearchView.OnQueryTextListener;

public class FragmentActivityEx extends FragmentActivity implements OnQueryTextListener, OnSuggestionListener {
    private static final String TAG = "FragmentActivityEx";

    protected MenuItem mSearchItem;
    protected SearchView mSearchView;
    protected CursorAdapter mSuggestionAdapter;

    // Activity.onCreateOptionsMenu
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        ((Activity) this).getMenuInflater().inflate(R.menu.main_activity_actions, menu);

        this.mSearchItem = menu.findItem(R.id.action_search);
        this.mSearchView = (SearchView) this.mSearchItem.getActionView();
        this.mSearchView.setOnQueryTextListener((SearchView.OnQueryTextListener) this);
        this.mSearchView.setOnSuggestionListener((SearchView.OnSuggestionListener) this);
        this.mSuggestionAdapter = this.mSearchView.getSuggestionsAdapter();
        this.mSearchView.setSearchableInfo(
            ((SearchManager) ((Context) this).getSystemService(Context.SEARCH_SERVICE)).getSearchableInfo(((Activity) this).getComponentName())
        );
        this.mSuggestionAdapter = this.mSearchView.getSuggestionsAdapter();
////    Log.i(TAG, "getSuggestionsAdapter: " + mSuggestionAdapter);
        this.mSearchItem.setActionView(this.mSearchView);

        return true;
    }

    // Activity.onOptionsItemSelected
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return ListFragmentEx.onOptionsItemSelectedHelper((Activity) this, ((Activity) this).getFragmentManager(), item);
    }

    // OnQueryTextListener.onQueryTextChange
    @Override
    public boolean onQueryTextChange(String newText) {
        return ListFragmentEx.onQueryTextChangeHelper((Activity) this, this.mSearchView, this.mSearchItem, newText);
    }

    // OnQueryTextListener.onQueryTextSubmit
    @Override
    public boolean onQueryTextSubmit(String query) {
        return ListFragmentEx.onQueryTextSubmitHelper((Activity) this, this.mSearchItem, query);
    }


    // OnSuggestionListener.onSuggestionClick
    @Override
    public boolean onSuggestionClick(int position) {
        return ListFragmentEx.onSuggestionClickHelper((Activity) this, this.mSuggestionAdapter, this.mSearchItem, position);
    }

    // OnSuggestionListener.onSuggestionSelect
    @Override
    public boolean onSuggestionSelect(int position) {
        return ListFragmentEx.onSuggestionSelectHelper((Activity) this, position);
    }
}
