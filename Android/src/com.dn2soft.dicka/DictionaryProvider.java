/**
 * SearchableDictionary
 *  DictionaryProvider.java
 */
package com.dn2soft.dicka;

import java.io.File;

import android.app.SearchManager;
import android.content.ContentProvider;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.net.Uri;

import android.util.Log;

/**
 * Provides access to the dictionary database.
 */
public class DictionaryProvider extends ContentProvider {
    private static final String TAG = "DictionaryProvider";

    public static final String VOCABULARY_CHAR = ",";

    public static String AUTHORITY = "com.dn2soft.dicka.DictionaryProvider";
    public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/dictionary");

    // MIME types used for searching words or looking up a single definition
    public static final String WORDS_MIME_TYPE = ContentResolver.CURSOR_DIR_BASE_TYPE +
                                                  "/vnd.dn2soft.dicka";
    public static final String DEFINITION_MIME_TYPE = ContentResolver.CURSOR_ITEM_BASE_TYPE +
                                                       "/vnd.dn2soft.dicka";

    private VocabularyDatabase.VocabularyOpenHelper mVocabularyOpenHelper;
    private WordSmartDatabase.WordSmartOpenHelper mWordSmartOpenHelper;

    // UriMatcher stuff
    private static final int SEARCH_WORDS = 0;
    private static final int GET_WORD = 1;
    private static final int SEARCH_SUGGEST = 2;
    private static final int REFRESH_SHORTCUT = 3;
    private static final UriMatcher sURIMatcher = buildUriMatcher();

    /**
     * Builds up a UriMatcher for search suggestion and shortcut refresh queries.
     */
    private static UriMatcher buildUriMatcher() {
        UriMatcher matcher =  new UriMatcher(UriMatcher.NO_MATCH);
        // to get definitions...
        matcher.addURI(AUTHORITY, "dictionary", SEARCH_WORDS);
        matcher.addURI(AUTHORITY, "dictionary/#", GET_WORD);
        // to get suggestions...
        matcher.addURI(AUTHORITY, SearchManager.SUGGEST_URI_PATH_QUERY, SEARCH_SUGGEST);
        matcher.addURI(AUTHORITY, SearchManager.SUGGEST_URI_PATH_QUERY + "/*", SEARCH_SUGGEST);

        /* The following are unused in this implementation, but if we include
         * {@link SearchManager#SUGGEST_COLUMN_SHORTCUT_ID} as a column in our suggestions table, we
         * could expect to receive refresh queries when a shortcutted suggestion is displayed in
         * Quick Search Box, in which case, the following Uris would be provided and we
         * would return a cursor with a single item representing the refreshed suggestion data.
         */
        matcher.addURI(AUTHORITY, SearchManager.SUGGEST_URI_PATH_SHORTCUT, REFRESH_SHORTCUT);
        matcher.addURI(AUTHORITY, SearchManager.SUGGEST_URI_PATH_SHORTCUT + "/*", REFRESH_SHORTCUT);
        return matcher;
    }

    @Override
    public boolean onCreate() {
        File dickaRootPath = LoadActivity.getDickRootPath(getContext());
        mVocabularyOpenHelper = new VocabularyDatabase.VocabularyOpenHelper(getContext(), dickaRootPath);
        mWordSmartOpenHelper = new WordSmartDatabase.WordSmartOpenHelper(getContext(), dickaRootPath);
        return true;
    }

    /**
     * Handles all the dictionary searches and suggestion queries from the Search Manager.
     * When requesting a specific word, the uri alone is required.
     * When searching all of the dictionary for matches, the selectionArgs argument must carry
     * the search query as the first element.
     * All other arguments are ignored.
     */
    @Override
    public Cursor query(
        Uri uri,
        String[] projection0,
        String selection0,
        String[] selectionArgs0,
        String sortOrder
    )
    {
        if (sURIMatcher.match(uri) != SEARCH_SUGGEST)
            return null;
        ////Log.i(TAG, "query[" + selectionArgs0.length + "]: |" + selectionArgs0[0] + "|");
        String query = selectionArgs0[0];
        if (query.isEmpty())
            return null;
        if (query.startsWith("#") || query.startsWith("@") || query.startsWith("!"))
            return null;
        //Log.i(TAG, "query: |" + query + "|");
        query = query.toLowerCase();
        boolean onlyInWordSmart = !query.startsWith(VOCABULARY_CHAR);
        if (!onlyInWordSmart) {
            query = query.substring(1);
        }
        query = query.trim();
        if (query.isEmpty())
                return null;
        //Log.i(TAG, "query: |" + query + "|");
        int index = query.lastIndexOf(Search.COMMAND_STR);
        query = query.substring(0, index == -1 ? query.length() : index);
        //Log.i(TAG, "query: |" + query + "|");
        query = query.replaceAll(Search.SEARCH_ALL_CHAR_PATTERN, "%") + "%";
        //Log.i(TAG, "query: |" + query + "|");

        String selection = String.format(
            "%s LIKE ?"
            ,        
            onlyInWordSmart
                ? WordSmartContract.TheWords.COLUMN_NAME_WORD
                : VocabularyContract.Vocabulary.COLUMN_NAME_WORD
        );
        String[] projection = new String[] {query};
        if (onlyInWordSmart)
            return mWordSmartOpenHelper.getSuggestions(selection, projection);
        return mVocabularyOpenHelper.getSuggestions(selection, projection);
    }

    /**
     * This method is required in order to query the supported types.
     * It's also useful in our own query() method to determine the type of Uri received.
     */
    @Override
    public String getType(Uri uri) {
        switch (sURIMatcher.match(uri)) {
            case SEARCH_WORDS:
                return WORDS_MIME_TYPE;
            case GET_WORD:
                return DEFINITION_MIME_TYPE;
            case SEARCH_SUGGEST:
                return SearchManager.SUGGEST_MIME_TYPE;
            case REFRESH_SHORTCUT:
                return SearchManager.SHORTCUT_MIME_TYPE;
            default:
                throw new IllegalArgumentException("Unknown URL " + uri);
        }
    }

    // Other required implementations...

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        throw new UnsupportedOperationException();
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        throw new UnsupportedOperationException();
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        throw new UnsupportedOperationException();
    }

}
