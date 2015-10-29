package com.dn2soft.dicka;

import android.preference.PreferenceManager;
import android.content.SharedPreferences;

import com.dn2soft.dicka.LoadActivity.Data;

import java.io.File;
import android.content.Intent;
import com.dn2soft.dickc.dictionary.Cambridge;

import android.util.Log;
import android.content.Context;


import android.app.Activity;    // requestWindowFeature, setProgressBarIndeterminateVisibility
import android.os.AsyncTask;
import java.util.ArrayList;
import com.dn2soft.dicka.LoadActivity.Entry;

public final class Search extends AsyncTask<Void, Void, Data>
{
    private static final String TAG = "Search";
    private static final String SEARCH_ALL_CHAR = ".";
    public static final String SEARCH_ALL_CHAR_PATTERN = "\\.";
    public static final String COMMAND_STR = "..";
    public static final String SWITCH_VOCABULARY = COMMAND_STR + ".";

    Activity        mActivity;          // setProgressBarIndeterminateVisibility
    String          mQuery;
    String          mHref;

    int mTabIndex = MainDisplayActivity.TAB_NONE;

    private void init(
        Activity        activity,
        String          query,
        String          href
    )
    {
        mActivity = activity;
        mQuery = query.trim().replaceAll("\\s+", " ");
        if (mQuery.startsWith(DictionaryProvider.VOCABULARY_CHAR))
            mQuery = mQuery.substring(DictionaryProvider.VOCABULARY_CHAR.length());
        mHref = href;
////    Log.i(TAG, "query: |" + query + "|");
    }
    public Search(
        Activity        activity,
        String          query,
        String          href
    ) {
        init(activity, query, href);
    }
    public Search(Activity activity, String query) {
        init(activity, query, null);
    }

    @Override
    protected void onPreExecute()
    {
        mActivity.getActionBar().setSubtitle("searching... |" + mQuery + "|");
        mActivity.setProgressBarIndeterminateVisibility(true);
////    Log.i(TAG, "" + mActivity);
    }

    @Override
    protected Data doInBackground(Void... none)
    {
        CambridgeDatabase.CambridgeOpenHelper           cambridgeHelper = null;
        WordSmartDatabase.WordSmartOpenHelper           wsHelper = null;
        EnglishKoreanDatabase.EnglishKoreanOpenHelper   ekHelper = null;
        try {
            File dickaRootPath = LoadActivity.getDickRootPath();
    ////    Log.i(TAG, "dickaRootPath: " + dickaRootPath.getAbsolutePath());
            cambridgeHelper = new CambridgeDatabase.CambridgeOpenHelper((Context) mActivity, dickaRootPath);
            wsHelper        = new WordSmartDatabase.WordSmartOpenHelper((Context) mActivity, dickaRootPath);
            ekHelper        = new EnglishKoreanDatabase.EnglishKoreanOpenHelper((Context) mActivity, dickaRootPath);

            Data data = new Data();
            data.entries = new ArrayList<Entry>();
            data.englishKoreanJson = ekHelper.select(mQuery);
            data.wordSmartJson = wsHelper.select(mQuery);
            String savedHref0 = cambridgeHelper.getSavedHref0(mHref == null ? mQuery : mHref);
            boolean isWiFiAvailable = LoadActivity.isWiFiAvailable(mActivity);

            SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences((Context) mActivity);
            boolean wifiOnly = sharedPref.getBoolean(SettingsActivity.PREF_CONNECTION_WIFI_ONLY, true);
Log.i(TAG, "onlyWiFi: " + wifiOnly);
    ////    Log.i(TAG, "savedHref0: " + (savedHref0 == null ? "null" : savedHref0));
    ////    Log.i(TAG, "Wi-Fi: " + isWiFiAvailable);
    ////    Log.i(TAG, "ENFORCE_WORD_SMART_ONLY: " + LoadActivity.ENFORCE_WORD_SMART_ONLY);
    /**
     * Search Policy
     *
     *  saveHref0   Wi-Fi   ENFORCE_WORD_SMART_ONLY
     *  non-null                                local Cambridge only
     *  (null)      True                        local Cambridge first and then online Cambridge
     *  (null)      False   True                local Cambridge only if any, otherwise warning
     *  (null)      False   False               local Cambridge fist and then, if any,
     *                                              online Cambridge at the expense of "data plan"
     */
            boolean wordSmartOnly =
                savedHref0 == null                  // if savedHref0 != null, wordSmartOnly = False
                    &&
                !isWiFiAvailable                    // if Wi-Fi = True, wordSmartOnly = False
                    &&
                LoadActivity.ENFORCE_WORD_SMART_ONLY// if neither savedHref0 nor Wi-Fi,
                                                    // ENFORCE_WORD_SMART_ONLY determines whether to use "data plan" or not
            ;
            data.result = cambridgeHelper.lookup(mHref == null ? mQuery : mHref, savedHref0);
            return data;
        } finally {
            if (cambridgeHelper != null)
                cambridgeHelper.close();
            if (wsHelper != null)
                wsHelper.close();
            if (ekHelper != null)
                ekHelper.close();
        }
    }

    @Override
    protected void onPostExecute(Data data)
    {
        String query = mQuery;
        mActivity.setProgressBarIndeterminateVisibility(false);
////    Log.i(TAG, "" + mActivity);
        mActivity.getActionBar().setSubtitle(null);


        boolean isEnglishKorean = false;
        EnglishKoreanContract.EnglishKoreanJson englishKoreanJson = data.englishKoreanJson;
        if (englishKoreanJson != null) {
            for (EnglishKoreanContract.EnglishKoreanJson.EnglishKorean englishKorean: englishKoreanJson.englishKoreanList) {
                isEnglishKorean = true;
////            Log.i(TAG, "EK: |" + englishKorean.word + "|: |" + englishKorean.sense + "|");
////            data.entries.add(new Entry(englishKorean.word, englishKorean.sense));
            }
        }

        boolean isWordSmart = false;
        WordSmartContract.WordSmartJson wordSmartJson = data.wordSmartJson;
        if (wordSmartJson != null) {
            for (WordSmartContract.WordSmartJson.TheWords theWords: wordSmartJson.theWordsList) {
                isWordSmart = true;
////            Log.i(TAG, "WSW: |" + theWords.word + "|: |" + theWords.sense + "|");
////            data.entries.add(new Entry(theWords.word, theWords.snippet));
            }
            for (WordSmartContract.WordSmartJson.TheHitParade theHitParade: wordSmartJson.theHitParadeList) {
                isWordSmart = true;
////            Log.i(TAG, "WSH: |" + theHitParade.word + "|: |" + theHitParade.sense + "|");
////            data.entries.add(new Entry(theHitParade.word, theHitParade.sense));
            }
            for (WordSmartContract.WordSmartJson.Root root: wordSmartJson.rootList) {
                isWordSmart = true;
////            Log.i(TAG, "WSR: |" + root.root + "|: |" + root.sense + "|");
////            data.entries.add(new Entry(root.root, root.sense));
            }
        }

        boolean isCambridge = false;
        while (data.result != null) {   // always true; [] in case of no data
            if (data.result.ErrorMessageList != null) {
/*
                for (String err: data.result.ErrorMessageList) {
                    Log.e(TAG, "|" + err + "|");
////                data.entries.add(new Entry("ERR", err));
                }
 */
                break;
            }
            if (data.result.WarnMessage != null) {
                Log.w(TAG, "|" + data.result.wordStr + "|: not found");
////            data.entries.add(new Entry("WARN", "|" + query + "|: not found"));
                break;
            }
            if (data.result.tryWordList != null) {
                data.entries.add(new Entry("try these words:", "    ......"));
                for (String w: data.result.tryWordList) {
////                Log.i(TAG, "|" + w + "|");
////                data.entries.add(new Entry("    " + w, ""));
                }
                break;
            }

            // Cambridge: True
            mTabIndex = MainDisplayActivity.TAB_CAMBRIDGE;
            if (data.result.doc.size() == 0)
                break;
            isCambridge = true;
////        Log.i(TAG, "|" + query + "|: got it");
////        data.entries.add(new Entry("|" + query + "|", "... got it!"));
////        data.entries.add(new Entry("JSON", data.result.getJSONStr()));

            break;
        }

/*
I/DeployDatabase( 6799): wordSmartJsonStr: |{'rootList':[],'theHitParadeList':[],'theWordsList':[]}|
I/DeployDatabase( 6799): englishKoreanJsonStr: |{'englishKoreanList':[]}|
I/DeployDatabase( 6799): Result.wordStr: |konica|
I/DeployDatabase( 6799): Result.ErrorMessageList: null
I/DeployDatabase( 6799): Result.WarnMessage: |null|
I/DeployDatabase( 6799): Result.tryWordList: [conical, tonics, tunica, ironical, minicab, phonics, minicam, erotica, formica, Formica]
I/DeployDatabase( 6799): Result.doc: []
 */
/*
        Log.i(TAG, "wordSmartJson: |" + data.wordSmartJson + "|");
        Log.i(TAG, "englishKoreanJson: |" + data.englishKoreanJson + "|");
        Log.i(TAG, "Result.wordStr: |" + data.result.wordStr + "|");
        Log.i(TAG, "Result.ErrorMessageList: " + data.result.ErrorMessageList);
        Log.i(TAG, "Result.WarnMessage: |" + data.result.WarnMessage + "|");
        Log.i(TAG, "Result.tryWordList: " + data.result.tryWordList);
        Log.i(TAG, "Result.doc: " + data.result.doc);
 */

/*
        EnglishKorean   WordSmart   Cambridge   tryWordList
        True            True                                Multi
        True            False                               Multi
        False           True                                Multi
        False           False       True                    Multi
        False           False       False       > 0         Single
                                                = 0         Popup
 */
        String cambridgeJsonStr = data.result.getJSONStr();
////    Log.i(TAG, "cambridgeJsonStr: |" + cambridgeJsonStr + "|");
        String wordSmartJsonStr = WordSmartContract.setJsonStr(wordSmartJson);
////    Log.i(TAG, "wordSmartJsonStr: |" + wordSmartJsonStr + "|");
        String englishKoreanJsonStr = EnglishKoreanContract.setJsonStr(englishKoreanJson);
////    Log.i(TAG, "englishKoreanJsonStr: |" + englishKoreanJsonStr + "|");

        String cambridgeResultBaseJsonStr = CambridgeContract.setResultBaseJsonStr((Cambridge.ResultBase) data.result);
        String cambridgeResultJsonStr = CambridgeContract.setResultJsonStr(data.result);
        if (isCambridge) {
            File dickaRootPath = LoadActivity.getDickRootPath();
            HistoryDatabase.HistoryOpenHelper helper = new HistoryDatabase.HistoryOpenHelper(mActivity, dickaRootPath);
            if (helper != null) {
                helper.insert(data.result.wordStr);
                helper.close();
            }
        }
        if (mTabIndex == MainDisplayActivity.TAB_NONE) {
            mTabIndex = isEnglishKorean ? MainDisplayActivity.TAB_ENGLISHKOREAN : MainDisplayActivity.TAB_CAMBRIDGE;
        }
////    Log.i(TAG, "mTabIndex = " + mTabIndex);
        if (isEnglishKorean || isWordSmart || isCambridge) {
////        Log.i(TAG, "Multipage");
        } else {
            if (data.result.tryWordList != null) {
////            Log.i(TAG, "Single page");
            } else {
////            Log.i(TAG, "Popup");
                if (data.result.ErrorMessageList != null) {
                } else {    // data.result.WarnMessage != null
                }
            }
        }
        Intent intent = new Intent((Context) mActivity, MainDisplayActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.putExtra(MainDisplayActivity.TABINDEX, mTabIndex);
        intent.putExtra(MainDisplayActivity.CAMBRIDGERESULTBASE, cambridgeResultBaseJsonStr);
        intent.putExtra(MainDisplayActivity.CAMBRIDGERESULT, cambridgeResultJsonStr);
        intent.putExtra(MainDisplayActivity.WORDSMART, wordSmartJsonStr);
        intent.putExtra(MainDisplayActivity.ENGLISHKOREAN, englishKoreanJsonStr);
        mActivity.startActivity(intent);
/*
        Json.BaseType  json = Json.setJson(cambridgeJsonStr);
        Cambridge.Result    result = new Cambridge.Result();
        result.setDoc(json, false);
        Log.i(TAG, "doc: " + result.doc);
        Log.i(TAG, "query: |" + result.wordStr + "|");
 */
    }
}
