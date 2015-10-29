package com.dn2soft.dicka;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileFilter;
import com.dn2soft.dicka.ZipFileExIteration.FileElement;

import java.util.Collections;
import java.util.Comparator;
import android.text.TextUtils;
import java.util.Arrays;
import java.util.Random;
import java.io.FileInputStream;
import android.os.Environment;
import java.io.FilenameFilter;
import java.util.List;
import java.util.ArrayList;
import android.os.Binder;

import android.content.Intent;
import android.os.IBinder;
import android.app.Service;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.util.Log;

// io
import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;

import android.content.Context;
import android.content.res.Resources;
import android.preference.PreferenceManager;
import android.content.SharedPreferences;

import android.os.Looper;
import android.content.res.AssetFileDescriptor;

public final class PlayMusicService extends Service implements
////AudioManager.OnAudioFocusChangeListener,
    MediaPlayer.OnErrorListener,
    MediaPlayer.OnPreparedListener,
    MediaPlayer.OnCompletionListener,
    MediaPlayer.OnSeekCompleteListener
{
    private static final String TAG = "PlayMusicService";
    public static final String ACTION_PLAY_STR = "..mp";
    public static final String ACTION_STOP_STR = "..ms";
    public static final String ACTION_TOGGLE_STR = "..mm";
    public static final String ACTION_NEXT_STR = "..mn";
    public static final String ACTION_NONE = "com.dn2soft.dicka.action.NONE";
    public static final String ACTION_PLAY = "com.dn2soft.dicka.action.PLAY";
    public static final String ACTION_TOGGLE = "com.dn2soft.dicka.action.TOGGLE";
    public static final String ACTION_NEXT = "com.dn2soft.dicka.action.NEXT";
    public static final String ACTION_PREV = "com.dn2soft.dicka.action.PREV";
    public static final String ACTION_STOP = "com.dn2soft.dicka.action.STOP";

    public static final int PLAY_MODE_REPLAY        = 0;    // play it repeatedly
    public static final int PLAY_MODE_CONTINUOUS    = 1;    // play next continuously
    public static final int PLAY_MODE_RANDOM        = 2;    // play next randomly
    private int mPlayMode = PLAY_MODE_CONTINUOUS;
    public int getPlayMode() {return mPlayMode;}
    public void setPlayMode(int playMode) {
        mPlayMode = playMode;
        DataSourceSetter setter = mSetter == null ? DEFAULT_DATA_SOURCE_SETTER : mSetter;
        setter.setPlayMode(playMode);
    }
    public static final int SORT_BY_ASC = +1;
    public static final int SORT_BY_DESC = -1;
    public void sort(int by)
    {
        DataSourceSetter setter = mSetter == null ? DEFAULT_DATA_SOURCE_SETTER : mSetter;
        setter.sort(by);
    }
    public List<String> getPlayList()
    {
        DataSourceSetter setter = mSetter == null ? DEFAULT_DATA_SOURCE_SETTER : mSetter;
        return setter.getPlayList();
    }
    public int getPosition()
    {
        DataSourceSetter setter = mSetter == null ? DEFAULT_DATA_SOURCE_SETTER : mSetter;
        return setter.getPosition();
    }
    public boolean setPosition(int position)
    {
        DataSourceSetter setter = mSetter == null ? DEFAULT_DATA_SOURCE_SETTER : mSetter;
        return setter.setPosition(position);
    }

    public static final int STATE_NONE      = 0;    // 1. nothing done yet, 2. an error occurred in initMediaPlayer
    public static final int STATE_READY     = 1;    // transient state to STATE_PLAYING, which is set in onPrepared
    public static final int STATE_PLAYING   = 2;
    public static final int STATE_STOPPED   = 3;
    public static final int STATE_PAUSED    = 4;
    public static final String[] STATE_STR = new String[] {
        "none",
        "ready",
        "playing",
        "stopped",
        "paused"
    };
    private int mState = STATE_NONE;
    public int getState() {return mState;}
    public interface OnStateChangeListener {
        public void onStateChange(int newState, int oldState, boolean refreshPlayList);
    }
    List<OnStateChangeListener> mOnStateChangeListenerList = new ArrayList<OnStateChangeListener>();
    public Boolean registerOnStateChangeListener(OnStateChangeListener listener)
    {
        if (mOnStateChangeListenerList.contains(listener)) {
////        Log.i(TAG, "registerOnStateChangeListener: listener = " + listener + ": already registered");
            return null;
        } else {
            return mOnStateChangeListenerList.add(listener);
        }
    }
    public Boolean unregisterOnStateChangeListener(OnStateChangeListener listener) {
        if (mOnStateChangeListenerList.contains(listener)) {
            return mOnStateChangeListenerList.remove(listener);
        } else {
////        Log.i(TAG, "unregisterOnStateChangeListener: listener = " + listener + ": not registered yet");
            return null;
        }
    }

    private final IBinder mBinder = new PlayMusicBinder();
    /**
     * Class used for the client Binder.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     */
    public class PlayMusicBinder extends Binder {
        PlayMusicService getService() {
            // Return this instance of LocalService so clients can call public methods
            return PlayMusicService.this;
        }
    }
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    public void set(int count)
    {
        int oldState = mState;
        int newState = mState;
        for (OnStateChangeListener listener: mOnStateChangeListenerList) {
            listener.onStateChange(newState, oldState, false);
        }
    }
    public void set(String cmd)
    {
//Log.i(TAG, "set: mMediaPlayer = " + mMediaPlayer);
        int oldState = mState;
        if (cmd.equals(ACTION_PLAY)) {
            boolean isSet = initMediaPlayer(BY_PLAY);
            mState = isSet ? STATE_READY : STATE_NONE;
        } else if (cmd.equals(ACTION_TOGGLE)) {
            if (mMediaPlayer != null) {
                if (mMediaPlayer.isPlaying()) {
                    mMediaPlayer.pause();
                    mState = STATE_PAUSED;
                } else {
                    mMediaPlayer.start();
                    mState = STATE_PLAYING;
                }
            }
        } else if (cmd.equals(ACTION_STOP)) {
            if (mMediaPlayer != null) {
                mMediaPlayer.release();
                mMediaPlayer = null;
                mState = STATE_STOPPED;
            }
        } else if (cmd.equals(ACTION_PREV)) {
            if (mMediaPlayer != null) {
                mMediaPlayer.reset();
            }
            boolean isSet = initMediaPlayer(BY_PREV);
            mState = isSet ? STATE_READY : STATE_NONE;
        } else if (cmd.equals(ACTION_NEXT)) {
            if (mMediaPlayer != null) {
                mMediaPlayer.reset();
            }
            boolean isSet = initMediaPlayer(BY_NEXT);
            mState = isSet ? STATE_READY : STATE_NONE;
        } else {
            Log.e(TAG, "set: not implemented yet");
            mState = STATE_NONE;
        }
        for (OnStateChangeListener listener: mOnStateChangeListenerList) {
            listener.onStateChange(mState, oldState, false);
        }
    }

    MediaPlayer mMediaPlayer;
    MediaPlayer getMediaPlayer() {return mMediaPlayer;}

    private List<List<FileElement>> mAudioListCollection;
    public void addAudioFiles(List<FileElement> audioFiles)
    {
        if (audioFiles != null) {
            mAudioListCollection.add(audioFiles);
            set(ACTION_STOP);
            mSetter = new ZipDataSourceSetter((Context) this, audioFiles, mPlayMode);
            for (OnStateChangeListener listener: mOnStateChangeListenerList) {
                listener.onStateChange(STATE_NONE, STATE_NONE, true);
            }
        }
    }

    Boolean mLoopDefaultValue;

    private static int BY_PLAY = 0;
    private static int BY_NEXT = +1;
    private static int BY_PREV = -1;
    interface DataSourceSetter {
        boolean setDataSource(MediaPlayer mp, int by);
        void sort(int by);
        void setPlayMode(int playMode);
        List<String> getPlayList();
        int getPosition();
        boolean setPosition(int position);
    }
    private DataSourceSetter mSetter;

    private static class IndexString
    {
        int index;
        String str;
        IndexString(int i, String s) {
            index = i;
            str = s;
        }
    }
    public static int[] sortString(List<IndexString> isa, final int by)
    {
        Collections.sort(
            isa,
            new Comparator<IndexString>() {
                @Override
                public int compare(IndexString a, IndexString b)
                {
                    if (by == SORT_BY_DESC)
                        return a.str.compareToIgnoreCase(b.str);
                    return b.str.compareToIgnoreCase(a.str);
                }
            }
        );
        int[] r = new int[isa.size()];
        for (int i = 0; i < isa.size(); ++i)
            r[i] = isa.get(i).index;
        return r;
    }
    public static int[] shuffleIndices(int[] a)
    {
        if (a == null)
            return null;
        int len = a.length;
        if (len <= 1)
            return a;
        int[] b = Arrays.copyOf(a, len);
        Random random = new Random();
        int n = len;
        while (n > 1) {
            int i = random.nextInt(n);
            if (i != n - 1) {   // switch values in indices i, n - 1
                int k = b[n - 1];
                b[n - 1] = b[i];
                b[i] = k;
            }
            --n;
        }
        return b;
    }
    void setOnDataSourceSetter(DataSourceSetter setter) {mSetter = setter;}
    static class RawDataSourceSetter implements DataSourceSetter {
        final Context mContext;
        final String[][] mAudioResIds;
        final int NUMBER_OF_FILES;
        int mPlayMode;
        int[] mPlayIndex;   // indices to mAudioResIds
        int mPosition;      // index to mPlayIndex
        List<IndexString> mISA0;
        List<IndexString> mISASortByAsc;
        int[] mPlayIndexSortedByAsc;
        List<IndexString> mISASortByDesc;
        int[] mPlayIndexSortedByDesc;
        RawDataSourceSetter(Context context, String[][] audioResIds, int playMode) {
            mContext = context;
            mAudioResIds = audioResIds;
            NUMBER_OF_FILES = audioResIds.length;
            mPlayMode = playMode;
            mPosition = NUMBER_OF_FILES > 0 ? 0 : -1;
            mPlayIndex = new int[NUMBER_OF_FILES];
            for (int i = 0; i < NUMBER_OF_FILES; ++i)
                mPlayIndex[i] = i;
            mISA0 = new ArrayList<IndexString>(NUMBER_OF_FILES);
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                mISA0.add(new IndexString(i, audioResIds[i][1]));
            }
            mISASortByAsc = new ArrayList<IndexString>(mISA0);
            mPlayIndexSortedByAsc = PlayMusicService.sortString(mISASortByAsc, SORT_BY_ASC);
            mISASortByDesc = new ArrayList<IndexString>(mISA0);
            mPlayIndexSortedByDesc = PlayMusicService.sortString(mISASortByDesc, SORT_BY_DESC);
        }

        @Override
        public int getPosition() {return mPosition;}

        @Override
        public boolean setPosition(int position)
        {
            if (position < 0 || position >= NUMBER_OF_FILES || mPosition == position)
                return false;
            mPosition = position;
            return true;
        }

        @Override
        public List<String> getPlayList()
        {
            List<String> playlist = new ArrayList<String>();
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                IndexString is = mISA0.get(mPlayIndex[i]);
                playlist.add(is.str);
            }
            return playlist;
        }

        @Override
        public void sort(int by)
        {
            if (mPosition < 0 || mPosition >= mPlayIndex.length)
                return;
            int k = mPlayIndex[mPosition];
            mPlayIndex = by == SORT_BY_ASC ? mPlayIndexSortedByAsc : mPlayIndexSortedByDesc;
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                if (mPlayIndex[i] == k) {
                    mPosition = i;
                    break;
                }
            }
        }

        @Override
        public void setPlayMode(int playMode) {mPlayMode = playMode;}
/*
        @Override
        public void shuffle()
        {
            if (NUMBER_OF_FILES == 0)
                return;
            int k = mPlayIndex[mPosition];
            mPlayIndex = PlayMusicService.shuffleIndices(mPlayIndex);
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                if (mPlayIndex[i] == k) {
                    mPosition = i;
                    break;
                }
            }
        }
 */

        @Override
        public boolean setDataSource(MediaPlayer mp, int by)
        {
            if (mp == null)
                return false;
            if (NUMBER_OF_FILES == 0)
                return false;

            if (mPlayMode != PLAY_MODE_REPLAY && by != 0 && NUMBER_OF_FILES > 1) {
                if (mPlayMode == PLAY_MODE_RANDOM) {
                    Random random = new Random();
                    by = 1 + random.nextInt(NUMBER_OF_FILES - 1);   // 1: at least, another one
                }
                int mod = (mPosition + by) % NUMBER_OF_FILES;
                mPosition = mod < 0 ? (mod + NUMBER_OF_FILES) : mod;
            }

            boolean rval = false;
            try {
                AssetFileDescriptor afd = null;
                try {
                    afd = mContext.getResources().openRawResourceFd(Integer.parseInt(mAudioResIds[mPlayIndex[mPosition]][0]));
                } catch (Resources.NotFoundException e) {e.printStackTrace();}
                if (afd == null) {
                    Log.e(TAG, "initMediaPlayer: from resource: afd is null");
                    return false;
                }
                mp.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
                rval = true;
            }
            catch (IllegalStateException e) {e.printStackTrace();}
            catch (IOException e) {e.printStackTrace();}
            catch (IllegalArgumentException e) {e.printStackTrace();}
            return rval;
        }
    }
    static class ZipDataSourceSetter implements DataSourceSetter {
        final Context mContext;
        final List<FileElement> mAudioFiles;
        final int NUMBER_OF_FILES;
        int mPlayMode;
        int[] mPlayIndex;
        int mPosition;
        List<IndexString> mISA0;
        List<IndexString> mISASortByAsc;
        int[] mPlayIndexSortedByAsc;
        List<IndexString> mISASortByDesc;
        int[] mPlayIndexSortedByDesc;
        ZipDataSourceSetter(Context context, List<FileElement> audioFiles, int playMode) {
            mContext = context;
            mAudioFiles = audioFiles;
            NUMBER_OF_FILES = audioFiles.size();
            mPlayMode = playMode;
            mPosition = NUMBER_OF_FILES > 0 ? 0 : -1;
            mPlayIndex = new int[NUMBER_OF_FILES];
            for (int i = 0; i < NUMBER_OF_FILES; ++i)
                mPlayIndex[i] = i;
            mISA0 = new ArrayList<IndexString>(NUMBER_OF_FILES);
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                String pathname = audioFiles.get(i).pathname;
                String filename = pathname.substring(1 + pathname.lastIndexOf("/"));
                mISA0.add(new IndexString(i, filename.substring(0, filename.lastIndexOf("."))));
            }
            mISASortByAsc = new ArrayList<IndexString>(mISA0);
            mPlayIndexSortedByAsc = PlayMusicService.sortString(mISASortByAsc, SORT_BY_ASC);
            mISASortByDesc = new ArrayList<IndexString>(mISA0);
            mPlayIndexSortedByDesc = PlayMusicService.sortString(mISASortByDesc, SORT_BY_DESC);
        }

        @Override
        public int getPosition() {return mPosition;}

        @Override
        public boolean setPosition(int position)
        {
            if (position < 0 || position >= NUMBER_OF_FILES || mPosition == position)
                return false;
            mPosition = position;
            return true;
        }

        @Override
        public List<String> getPlayList()
        {
            List<String> playlist = new ArrayList<String>();
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                IndexString is = mISA0.get(mPlayIndex[i]);
                playlist.add(is.str);
            }
            return playlist;
        }

        @Override
        public void sort(int by)
        {
            int k = mPlayIndex[mPosition];
            mPlayIndex = Arrays.copyOf(by == SORT_BY_ASC ? mPlayIndexSortedByAsc : mPlayIndexSortedByDesc, NUMBER_OF_FILES);
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                if (mPlayIndex[i] == k) {
                    mPosition = i;
                    break;
                }
            }
        }

        @Override
        public void setPlayMode(int playMode) {mPlayMode = playMode;}
/*
        @Override
        public void shuffle()
        {
            if (NUMBER_OF_FILES == 0)
                return;
            int k = mPlayIndex[mPosition];
            mPlayIndex = PlayMusicService.shuffleIndices(mPlayIndex);
            for (int i = 0; i < NUMBER_OF_FILES; ++i) {
                if (mPlayIndex[i] == k) {
                    mPosition = i;
                    break;
                }
            }
        }
 */

        @Override
        public boolean setDataSource(MediaPlayer mp, int by)
        {
            if (mp == null)
                return false;
            if (NUMBER_OF_FILES == 0)
                return false;

            if (mPlayMode != PLAY_MODE_REPLAY && by != 0 && NUMBER_OF_FILES > 1) {
                if (mPlayMode == PLAY_MODE_RANDOM) {
                    Random random = new Random();
                    by = 1 + random.nextInt(NUMBER_OF_FILES - 1);   // 1: at least, another one
                }
                int mod = (mPosition + by) % NUMBER_OF_FILES;
                mPosition = mod < 0 ? (mod + NUMBER_OF_FILES) : mod;
            }

            boolean rval = false;
            try {
                FileElement fe = mAudioFiles.get(mPlayIndex[mPosition]);
                if (fe.zipFiles == null) {
                    String pathname = fe.pathname;
                    if (pathname.charAt(0) == '/') {    // from file system
                        FileInputStream fis = new FileInputStream(pathname);
                        mp.setDataSource(fis.getFD());
                        fis.close();
                    } else {    // from assets
                        AssetFileDescriptor afd = null;
                        try {
                            afd = mContext.getAssets().openFd(pathname);
                        } catch (IOException e) {e.printStackTrace();}
                        if (afd == null) {
                            Log.e(TAG, "initMediaPlayer: from resource: afd is null");
                            return false;
                        }
                        mp.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
                    }
                } else {
                    if (fe.tmpPathname == null) {
                        String copiedStr = null;
                        ZipFileExFileFilter fileFilter = fe.zipFiles[0].charAt(0) == '/' ? new FileListLoad.FileSystemAudioFilter() : new FileListLoad.AssetAudioFilter(mContext.getAssets());
                        try {
                            copiedStr = LoadActivity.CONTAINER.copy(fileFilter.getInputStream(fe.zipFiles[0]), fe.zipFiles, fe.pathname, null);
                        } catch (IOException e) {   // including ZipException, FileNotFoundException
                            e.printStackTrace();
                        }
                        fe.tmpPathname = copiedStr;
                    }
                    FileInputStream fis = new FileInputStream(fe.tmpPathname);
                    mp.setDataSource(fis.getFD());
                    fis.close();
                }
                rval = true;
            }
            catch (IllegalStateException e) {e.printStackTrace();}
            catch (IOException e) {e.printStackTrace();}
            catch (IllegalArgumentException e) {e.printStackTrace();}
            return rval;
        }
    }
    private DataSourceSetter DEFAULT_DATA_SOURCE_SETTER;
    private boolean initMediaPlayer(int by) // by = 0: replay, >0: next, <0: prev
    {
        if (mMediaPlayer == null)   // if set, reuse it
            mMediaPlayer = new MediaPlayer();
        DataSourceSetter setter = mSetter;
        if (setter == null)
            setter = DEFAULT_DATA_SOURCE_SETTER;
        if (!setter.setDataSource(mMediaPlayer, by)) {
            Log.e(TAG, "" + setter + ": .setDataSource(): failed");
            return false;
        }
        mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
        mMediaPlayer.prepareAsync(); // prepare async to not block main thread
        mMediaPlayer.setOnErrorListener(this);
        mMediaPlayer.setOnPreparedListener(this);
        mMediaPlayer.setOnCompletionListener(this);
        mMediaPlayer.setOnSeekCompleteListener(this);

        return true;
    }

    @Override
    public void onCreate() {
        mSetter = null;

        // resource IDs
        DEFAULT_DATA_SOURCE_SETTER = new RawDataSourceSetter(
            (Context) this,
            new String[][] {
/*
                {String.valueOf(R.raw.my___sgwannabe), "My Love - SG Wanna Be"},
                {String.valueOf(R.raw.cocktail___marronnier), "Cocktail Love - Marronnier"},
                {String.valueOf(R.raw.you_and_i___iu), "You & I - IU"}
 */
            },
            mPlayMode
        );
/*
        AudioManager audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        int result = audioManager.requestAudioFocus(this, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);
Log.i(TAG, "AudioManager.requestAudioFocus() returns " + result);
        if (result != AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
Log.i(TAG, "AudioManager.requestAudioFocus(): " + result + ": could not get audio focus");
        }
 */
        mAudioListCollection = new ArrayList<List<FileElement>>();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
//Log.i(TAG, "onStartCommand: mMediaPlayer: " + mMediaPlayer);
        int oldState = mState;
        if (intent.getAction().equals(ACTION_PLAY)) {
//Log.i(TAG, "onStartCommand: ACTION_PLAY");
//Log.i(TAG, "onStartCommand: mMediaPlayer: " + mMediaPlayer);
            boolean isSet = initMediaPlayer(BY_PLAY);
            mState = isSet ? STATE_READY : STATE_NONE;
        } else if (intent.getAction().equals(ACTION_TOGGLE)) {
//Log.i(TAG, "onStartCommand: ACTION_TOGGLE");
//Log.i(TAG, "onStartCommand: mMediaPlayer: " + mMediaPlayer);
//Log.i(TAG, "onStartCommand: indexPlaying = " + indexPlaying);
            if (mMediaPlayer != null) {
                if (mMediaPlayer.isPlaying()) {
                    mMediaPlayer.pause();
                    mState = STATE_PAUSED;
                } else {
                    mMediaPlayer.start();
                    mState = STATE_PLAYING;
                }
            }
        } else if (intent.getAction().equals(ACTION_STOP)) {
//Log.i(TAG, "onStartCommand: ACTION_STOP");
//Log.i(TAG, "onStartCommand: mMediaPlayer: " + mMediaPlayer);
            if (mMediaPlayer != null) {
                mMediaPlayer.release();
                mMediaPlayer = null;
                mState = STATE_STOPPED;
            }
        } else if (intent.getAction().equals(ACTION_PREV)) {
//Log.i(TAG, "onStartCommand: ACTION_NEXT");
//Log.i(TAG, "onStartCommand: mMediaPlayer: " + mMediaPlayer);
            if (mMediaPlayer != null) {
                mMediaPlayer.reset();
            }
            boolean isSet = initMediaPlayer(BY_PREV);
            mState = isSet ? STATE_READY : STATE_NONE;
        } else if (intent.getAction().equals(ACTION_NEXT)) {
//Log.i(TAG, "onStartCommand: ACTION_NEXT");
//Log.i(TAG, "onStartCommand: mMediaPlayer: " + mMediaPlayer);
            if (mMediaPlayer != null) {
                mMediaPlayer.reset();
            }
            boolean isSet = initMediaPlayer(BY_NEXT);
            mState = isSet ? STATE_READY : STATE_NONE;
        } else {
            mState = STATE_NONE;
            Log.e(TAG, "onStartCommand: not implemented yet: |" + intent.getAction() + "|");
        }
        for (OnStateChangeListener listener: mOnStateChangeListenerList) {
            listener.onStateChange(mState, oldState, false);
        }

        return START_NOT_STICKY;
    }
    @Override
    public void onDestroy() {
//Log.i(TAG, "onDestroy: MediaPlayer: " + mMediaPlayer);
        if (mMediaPlayer != null) {
            mMediaPlayer.release();
            mMediaPlayer = null;
            mState = STATE_NONE;
        }
        if (mAudioListCollection != null) {
            for (List<FileElement> audioList: mAudioListCollection) {
                if (audioList != null) {
                    audioList.clear();
                }
            }
        }
    }
/*
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
 */

/*
If any of these methods is called right after a MediaPlayer object is constructed, the user supplied callback method OnErrorListener.onError() won't be called by the internal player engine and the object state remains unchanged; but if these methods are called right after reset(), the user supplied callback method OnErrorListener.onError() will be invoked by the internal player engine and the object will be transfered to the Error state.
 */
/*
True if the method handled the error, false if it didn't. Returning false, or not having an OnErrorListener at all, will cause the OnCompletionListener to be called.
 */
    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
//Log.i(TAG, "onError: MediaPlayer: " + mMediaPlayer);
//Log.i(TAG, "onError: mp: " + mp);
//Log.i(TAG, "onError: what: " + what);
//Log.i(TAG, "onError: what: |" + String.valueOf(what) + "|");
//Log.i(TAG, "onError: extra: " + extra);
        return true;    // no more call to OnCompletionListener
    }

/*
release():
    Releases resources associated with this MediaPlayer object. It is considered good practice to call this method when you're done using the MediaPlayer. In particular, whenever an Activity of an application is paused (its onPause() method is called), or stopped (its onStop() method is called), this method should be invoked to release the MediaPlayer object, unless the application has a special need to keep the object around. In addition to unnecessary resources (such as memory and instances of codecs) being held, failure to call this method immediately if a MediaPlayer object is no longer needed may also lead to continuous battery consumption for mobile devices, and playback failure for other applications if no multiple instances of the same codec are supported on a device. Even if multiple instances of the same codec are supported, some performance degradation may be expected when unnecessary multiple instances are used at the same time.
 */
/*
The invoke of the callback signals that the object is now in the PlaybackCompleted state.
 */
    @Override
    public void onCompletion(MediaPlayer mp)
    {
//Log.i(TAG, "onCompletion: MediaPlayer: " + mMediaPlayer);
//Log.i(TAG, "onCompletion: mp: " + mp);
        if (mp == null)
            return;
        int oldState = mState;
        mState = STATE_STOPPED;
        for (OnStateChangeListener listener: mOnStateChangeListenerList) {
            listener.onStateChange(mState, oldState, false);
        }

/*
        Context context = (Context) this;
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        if (mLoopDefaultValue == null) {    // not set yet
            Resources res = context.getResources();
            mLoopDefaultValue = new Boolean(res.getBoolean(R.bool.pref_defaultValue_loop));
        }
        boolean loop = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_LOOP, mLoopDefaultValue.booleanValue());
 */
//Log.i(TAG, "mPlayMode = " + mPlayMode);
        boolean loop = mPlayMode == PlayMusicService.PLAY_MODE_REPLAY;
        if (mp != null) {
            mp.reset();
        }
        oldState = mState;
        boolean isSet = initMediaPlayer(loop ? BY_PLAY : BY_NEXT);
        mState = isSet ? STATE_READY : STATE_NONE;
        for (OnStateChangeListener listener: mOnStateChangeListenerList) {
            listener.onStateChange(mState, oldState, false);
        }
    }

    @Override
    public void onPrepared(MediaPlayer mp)
    {
//Log.i(TAG, "onPrepared: MediaPlayer: " + mMediaPlayer);
//Log.i(TAG, "onPrepared: mp: " + mp);
//Log.i(TAG, "onPrepared: indexPlaying = " + indexPlaying);
        int oldState = mState;
        if (mp != null) { // mp = null possible?
            mp.start();
            mState = STATE_PLAYING;
            for (OnStateChangeListener listener: mOnStateChangeListenerList) {
                listener.onStateChange(mState, oldState, false);
            }
        }
    }

    @Override
    public void onSeekComplete(MediaPlayer mp)
    {
//      Log.i(TAG, "onSeekComplete: mMediaPlayer = " + mMediaPlayer);
//      Log.i(TAG, "onSeekComplete: mp = " + mp);
    }


/**
 *
    @Override
    public void onAudioFocusChange(int focusChange) {
//Log.i(TAG, "onAudioFocusChange");
        switch (focusChange) {
            case AudioManager.AUDIOFOCUS_GAIN:
//Log.i(TAG, "AudioManager.AUDIOFOCUS_GAIN");
//Log.i(TAG, "onAudioFocusChange: MediaPlayer: " + mMediaPlayer);
                // resume playback
                if (mMediaPlayer == null)
                    initMediaPlayer(BY_PLAY);
                else if (!mMediaPlayer.isPlaying())
                    mMediaPlayer.start();
                mMediaPlayer.setVolume(1.0f, 1.0f);
                break;

            case AudioManager.AUDIOFOCUS_LOSS:
//Log.i(TAG, "AudioManager.AUDIOFOCUS_LOSS");
//Log.i(TAG, "onAudioFocusChange: MediaPlayer: " + mMediaPlayer);
                // Lost focus for an unbounded amount of time: stop playback and release media player
                if (mMediaPlayer != null) {
                    if (mMediaPlayer.isPlaying())
                        mMediaPlayer.stop();
                    mMediaPlayer.release();
                    mMediaPlayer = null;
                }
                break;

            case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
//Log.i(TAG, "AudioManager.AUDIOFOCUS_LOSS_TRANSIENT");
//Log.i(TAG, "onAudioFocusChange: MediaPlayer: " + mMediaPlayer);
                // Lost focus for a short time, but we have to stop
                // playback. We don't release the media player because playback
                // is likely to resume
                if (mMediaPlayer != null) {
                    if (mMediaPlayer.isPlaying())
                        mMediaPlayer.pause();
                }
                break;

            case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
//Log.i(TAG, "AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK");
//Log.i(TAG, "onAudioFocusChange: MediaPlayer: " + mMediaPlayer);
                // Lost focus for a short time, but it's ok to keep playing
                // at an attenuated level
                if (mMediaPlayer != null) {
                    if (mMediaPlayer.isPlaying())
                        mMediaPlayer.setVolume(0.1f, 0.1f);
                }
                break;
        }
    }
 */

    public static boolean isSupportedMediaFileBasedOnExtension(String lowerExtStr)
    {
        return lowerExtStr.equals("mp3") || lowerExtStr.equals("mp4") || lowerExtStr.equals("ogg");
    }
}
