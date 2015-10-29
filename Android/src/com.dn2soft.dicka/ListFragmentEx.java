package com.dn2soft.dicka;
import com.dn2soft.gas.app.GasPlanMenuDialogFragment;
import com.dn2soft.www.app.SearchVideoMenuDialogFragment;

// extends ListFragment implementing OnQueryTextListener, OnSuggestionListener
import android.app.ListFragment;
import android.widget.SearchView.OnQueryTextListener;
import android.widget.SearchView.OnSuggestionListener;

import android.view.MenuInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.SearchView;
import android.widget.CursorAdapter;

import android.os.AsyncTask;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.app.AlertDialog;
import java.util.ArrayList;

import android.content.Context;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Intent;

import android.util.Log;

import android.database.Cursor;
import android.app.SearchManager;


/**
 * Extend ListFragment(Fragment) implementing OnQueryTextListener, OnSuggestionListener
 * by overriding the following:
 *      Fragment.onCreateOptionsMenu
 *      Fragment.onOptionsItemSelected
 *      OnQueryTextListener.onQueryTextChange
 *      OnQueryTextListener.onQueryTextSubmit
 *      OnSuggestionListener.onSuggestionClick
 *      OnSuggestionListener.onSuggestionSelect
 */
public class ListFragmentEx extends ListFragment implements OnQueryTextListener, OnSuggestionListener {
    private static final String TAG = "ListFragmentEx";

    protected MenuItem mSearchItem;
    protected SearchView mSearchView;
    protected CursorAdapter mSuggestionAdapter;

    // Fragment.onCreateOptionsMenu
    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater)
    {
        inflater.inflate(R.menu.main_activity_actions, menu);

        this.mSearchItem = menu.findItem(R.id.action_search);
        this.mSearchView = (SearchView) this.mSearchItem.getActionView();
        this.mSearchView.setOnQueryTextListener((SearchView.OnQueryTextListener) this);
        this.mSearchView.setOnSuggestionListener((SearchView.OnSuggestionListener) this);
        Activity activity = getActivity();
        this.mSuggestionAdapter = this.mSearchView.getSuggestionsAdapter();
        this.mSearchView.setSearchableInfo(
            ((SearchManager) ((Context) activity).getSystemService(Context.SEARCH_SERVICE)).getSearchableInfo(activity.getComponentName())
        );
        this.mSuggestionAdapter = this.mSearchView.getSuggestionsAdapter();
////    Log.i(TAG, "getSuggestionsAdapter: " + mSuggestionAdapter);
        this.mSearchItem.setActionView(this.mSearchView);
    }

    // Fragment.onOptionsItemSelected
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        if (ListFragmentEx.onOptionsItemSelectedHelper(((Fragment) this).getActivity(), ((Fragment) this).getFragmentManager(), item))
            return true;
        return super.onOptionsItemSelected(item);
    }
    public static boolean onOptionsItemSelectedHelper(final Activity activity, final FragmentManager fragmentManager, MenuItem item)
    {
// Handle item selection
        switch (item.getItemId()) {
            case R.id.menu_history:
                (new AsyncTask<Void, Void, ArrayList<String>> () {
                    @Override
                    protected ArrayList<String> doInBackground(Void...none)
                    {
                        HistoryDatabase.HistoryOpenHelper helper = new HistoryDatabase.HistoryOpenHelper((Context) activity, LoadActivity.getDickRootPath());
                        ArrayList<String> wordStrListInHistory = null;
                        if (helper != null) {
                            wordStrListInHistory = helper.select();
                            helper.close();
                        }
                        return wordStrListInHistory;
                    }

                    @Override
                    protected void onPostExecute(ArrayList<String> wordStrListInHistory)
                    {
                        DialogFragment fragment = HistoryMenuDialogFragment.newInstance(wordStrListInHistory);
                        fragment.show(fragmentManager, "history_menu_dialog");
                    }
                }).execute();
                return true;
            case R.id.menu_labels:
                (new AsyncTask<Void, Void, String> () {
                    @Override
                    protected String doInBackground(Void...none)
                    {
                        LabelDatabase.LabelOpenHelper helper = new LabelDatabase.LabelOpenHelper((Context) activity, LoadActivity.getDickRootPath());
                        LabelContract.LabelForJson labelForJson = helper.select();
                        helper.close();
                        ArrayList<String[]> displayLabelArrayList = labelForJson.getAllDisplayLabelArray();
                        StringArrayArrayList saal = new StringArrayArrayList(displayLabelArrayList);
                        return StringArrayArrayList.getJsonStr(saal);
                    }

                    @Override
                    protected void onPostExecute(String saalJsonStr)
                    {
                        DialogFragment fragment = LabelMenuDialogFragment.newInstance(saalJsonStr);
                        fragment.show(fragmentManager, "label_menu_dialog");
                    }
                }).execute();
                return true;
            case R.id.menu_book:
                BookMenuDialogFragment.newInstance().show(fragmentManager, "book_menu_dialog");
                return true;
            case R.id.menu_root:
                (new AsyncTask<Void, Void, ArrayList<String>>() {
                    @Override
                    protected ArrayList<String> doInBackground(Void...none)
                    {
                        WordSmartDatabase.WordSmartOpenHelper wsHelper = new WordSmartDatabase.WordSmartOpenHelper((Context) activity, LoadActivity.getDickRootPath());
                        ArrayList<String> rootSenseArrayList = null;
                        if (wsHelper != null) {
                            rootSenseArrayList = wsHelper.getRootList();
                            wsHelper.close();
                        }
                        return rootSenseArrayList;
                    }

                    @Override
                    protected void onPostExecute(ArrayList<String> rootSenseArrayList)
                    {
                        DialogFragment fragment = RootMenuDialogFragment.newInstance(rootSenseArrayList);
                        fragment.show(fragmentManager, "root_menu_dialog");
                    }
                }).execute();
                return true;
            case R.id.menu_play_music:
                PlayMusicMenuDialogFragment.newInstance().show(fragmentManager, "play_music_menu_dialog");
                return true;
            case R.id.menu_gas_plan:
                GasPlanMenuDialogFragment.newInstance().show(fragmentManager, "gas_plan_menu_dialog");
                return true;
            case R.id.search_video:
                SearchVideoMenuDialogFragment.newInstance().show(fragmentManager, "search_video_menu_dialog");
                return true;
            case R.id.menu_settings:
/*
                new AlertDialog.Builder((Context) activity)
                    .setTitle("Settings...")
                    .setMessage("nothing yet")
                    .setNeutralButton(
                        "okay",
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which)
                            {
                                //Log.i(TAG, "okay: onClick");
                            }
                        }
                     )
                    .create()
                    .show();
 */
                    {
                        Intent intent = new Intent((Context) activity, SettingsActivity.class);
                        activity.startActivity(intent);
                    }
                    return true;
            default:
                return false;
        }
    }


    // OnQueryTextListener.onQueryTextChange
    @Override
    public boolean onQueryTextChange(String newText)
    {
        return ListFragmentEx.onQueryTextChangeHelper(((Fragment) this).getActivity(), this.mSearchView, this.mSearchItem, newText);
    }
    public static boolean onQueryTextChangeHelper(Activity activity, SearchView searchView, MenuItem searchItem, String newText)
    {
        Context context = (Context) activity;
        if (newText.endsWith(PlayMusicService.ACTION_PLAY_STR)) {
            searchItem.collapseActionView();
            Intent intent = new Intent(PlayMusicService.ACTION_PLAY);
            intent.setClass(context, PlayMusicService.class);
            context.startService(intent);
            return true;
        }
        if (newText.endsWith(PlayMusicService.ACTION_STOP_STR)) {
            searchItem.collapseActionView();
            Intent intent = new Intent(PlayMusicService.ACTION_STOP);
            intent.setClass(context, PlayMusicService.class);
            context.startService(intent);
            return true;
        }
        if (newText.endsWith(PlayMusicService.ACTION_TOGGLE_STR)) {
            searchItem.collapseActionView();
            Intent intent = new Intent(PlayMusicService.ACTION_TOGGLE);
            intent.setClass(context, PlayMusicService.class);
            context.startService(intent);
            return true;
        }
        if (newText.endsWith(PlayMusicService.ACTION_NEXT_STR)) {
            searchItem.collapseActionView();
            Intent intent = new Intent(PlayMusicService.ACTION_NEXT);
            intent.setClass(context, PlayMusicService.class);
            context.startService(intent);
            return true;
        }

        if (newText.endsWith(Search.SWITCH_VOCABULARY)) {
            boolean onlyInWordSmart = !newText.startsWith(DictionaryProvider.VOCABULARY_CHAR);
            String text = onlyInWordSmart ? DictionaryProvider.VOCABULARY_CHAR : "";
            text += newText.substring(onlyInWordSmart ? 0 : 1, newText.length() - Search.SWITCH_VOCABULARY.length());
            searchView.setQuery(text, false);
        }
        return true;    // handle it
    }

    // OnQueryTextListener.onQueryTextSubmit
    @Override
    public boolean onQueryTextSubmit(String query)
    {
        return ListFragmentEx.onQueryTextSubmitHelper(((Fragment) this).getActivity(), this.mSearchItem, query);
    }
    public static boolean onQueryTextSubmitHelper(Activity activity, MenuItem searchItem, String query)
    {
        searchItem.collapseActionView();
        if (query.startsWith("!"))
            new NationalCancerInstituteSearch(activity, query.substring(1)).execute();
        else
            new Search(activity, query).execute();
        return true;    // handle it
    }


    // OnSuggestionListener.onSuggestionClick
    @Override
    public boolean onSuggestionClick(int position)
    {
        return ListFragmentEx.onSuggestionClickHelper(((Fragment) this).getActivity(), this.mSuggestionAdapter, this.mSearchItem, position);
    }
    public static boolean onSuggestionClickHelper(Activity activity, CursorAdapter suggestionAdapter, MenuItem searchItem, int position) {
        Cursor cursor = null;
        if (suggestionAdapter == null)
            return true;
        if (
            suggestionAdapter == null ||
            (cursor = suggestionAdapter.getCursor()) == null ||
            !cursor.moveToPosition(position)
        ) {
            return true;
        }
        String word = cursor.getString(1);
////    Log.i(TAG, "word: |" + word + "|");
        searchItem.collapseActionView();
        new Search(activity, word).execute();
        return true;
    }

    // OnSuggestionListener.onSuggestionSelect
    @Override
    public boolean onSuggestionSelect(int position)
    {
        return ListFragmentEx.onSuggestionSelectHelper(((Fragment) this).getActivity(), position);
    }
    public static boolean onSuggestionSelectHelper(Activity activity, int position)
    {
        // nothing
        return true;    // handle it
    }
}
