package com.dn2soft.dicka;
import com.dn2soft.dicka.ZipFileExIteration.FileElement;

import android.widget.TextView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.AdapterView;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.os.Looper;
////////////////////////////////////////
// bind to PlayMusicService
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;
import android.widget.Toast;
import android.content.Context;
import android.content.Intent;
import android.widget.SeekBar;
import android.media.MediaPlayer;
////////////////////////////////////////

import android.util.Log;

import android.app.Activity;
import android.app.DialogFragment;
import android.app.Dialog;
import android.app.AlertDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageButton;

import java.util.List;
import java.util.ArrayList;
import android.content.DialogInterface;


public class PlayMusicMenuDialogFragment extends DialogFragment implements PlayMusicService.OnStateChangeListener, SeekBar.OnSeekBarChangeListener {
    private static final String TAG = "PlayMusicMenuDialogFragment";
    private static final int GET_CODE = 0;

    public static PlayMusicMenuDialogFragment newInstance()
    {
        PlayMusicMenuDialogFragment fragment = new PlayMusicMenuDialogFragment();
        return fragment;
    }
    public static AlertDialog buildAlertDialog(Context context, String message)
    {
        return (new AlertDialog.Builder(context))
              .setTitle("Alert")
              .setMessage(message)
              .setCancelable(false)
              .setPositiveButton(
                "okay",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id)
                    {
                        dialog.dismiss();
                    }
                }
              )
              .create();
    }


    SeekBar mSeekBar;
    ImageButton mPrevImgBtn;
    ImageButton mPlayImgBtn;
    ImageButton mStopImgBtn;
    ImageButton mPauseImgBtn;
    ImageButton mNextImgBtn;
    ImageButton mReplayImgBtn;
    ImageButton mContinuousImgBtn;
    ImageButton mRandomImgBtn;
    ImageButton mSortAscImgBtn;
    ImageButton mSortDescImgBtn;
    ImageButton mUpImgBtn;
    ImageButton mDownImgBtn;
    ListView mListView;
    Button mCancelBtn;
    Button mBrowseBtn;
    Button mAssetsBtn;
    ArrayAdapterPlay mAdapter;
/*
////////////////////////////////////////////////////////////////////////////////
// bind to PlayMusicService
    public static PlayMusicService mPlayMusicService;
    boolean mBound = false;
    // Defines callbacks for service binding, passed to bindService()
    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            PlayMusicService.PlayMusicBinder binder = (PlayMusicService.PlayMusicBinder) service;
            mPlayMusicService = binder.getService();
            mPlayMusicService.registerOnStateChangeListener(PlayMusicMenuDialogFragment.this);
            mBound = true;

            init(mAdapter);
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            mPlayMusicService.unregisterOnStateChangeListener(PlayMusicMenuDialogFragment.this);
            mPlayMusicService = null;
            mBound = false;
        }
    };
    @Override
    public void onStart()
    {
        super.onStart();
        // Bind to LocalService
        Intent intent = new Intent((Context) getActivity(), PlayMusicService.class);
        ((Context) getActivity()).bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }
    @Override
    public void onStop()
    {
        super.onStop();
        // Unbind from the service
        if (mBound) {
            ((Context) getActivity()).unbindService(mConnection);
            mBound = false;
        }
    }
// bind to PlayMusicService
////////////////////////////////////////////////////////////////////////////////
 */

////////////////////////////////////////////////////////////////////////////////
    Thread mSeekThread;
    private void visualizeViaSeekBar()
    {
//      Log.i(TAG, "" + mPlayMusicService);
        MediaPlayer mp = mPlayMusicService.getMediaPlayer();
//      Log.i(TAG, "" + mp);
        final int ms = mp.getDuration();
//      Log.i(TAG, "MediaPlayer.getDuration(): " + ms + " ms");
        mSeekBar.setMax(ms);
//      Log.i(TAG, "mSeekThread: " + mSeekThread);
        if (mSeekThread != null) {
            mSeekThread.interrupt();
            mSeekThread = null;
        }
        mSeekThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
//Log.i(TAG, "initMediaPlayer: UI thread: " + Looper.getMainLooper().equals(Looper.myLooper()));
                    if (Thread.interrupted()) {
                        Log.i(TAG, "Thread interrupted");
                        break;
                    }
                    MediaPlayer mp = mPlayMusicService.getMediaPlayer();
                    if (mp == null) {
                        Log.i(TAG, "MediaPlayer is null");
                        break;
                    }
                    int curPos = -1;
                    try {
                        curPos = mp.getCurrentPosition();
                    } catch (IllegalStateException e) {
                        e.printStackTrace();
                    }
                    if (curPos == -1) {
                        Log.i(TAG, "MediaPlayer.getCurrentPosition() raised IllegalStateException");
                        break;
                    }
                    if (curPos == ms) {
                        Log.i(TAG, "MediaPlayer.getCurrentPosition() = MediaPlayer.getDuration()");
                        break;
                    }
                    mSeekBar.setProgress(curPos);
                    try {
                        Thread.sleep(ms / 100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        break;
                    }
                }
//Log.i(TAG, "end of thread");
                mSeekBar.setProgress(0);
            }
        });
//Log.i(TAG, "mSeekThread: " + mSeekThread);
        mSeekThread.start();
    }
    private void setPlayModeButtons(int playMode)
    {
        switch (playMode) {
        case PlayMusicService.PLAY_MODE_REPLAY:
            mPrevImgBtn.setVisibility(View.GONE);
            mPrevImgBtn.setEnabled(false);
            mNextImgBtn.setVisibility(View.GONE);
            mNextImgBtn.setEnabled(false);
            mReplayImgBtn.setVisibility(View.VISIBLE);
            mReplayImgBtn.setEnabled(true);
            mContinuousImgBtn.setVisibility(View.GONE);
            mContinuousImgBtn.setEnabled(false);
            mRandomImgBtn.setVisibility(View.GONE);
            mRandomImgBtn.setEnabled(false);
            break;
        case PlayMusicService.PLAY_MODE_CONTINUOUS:
            mPrevImgBtn.setVisibility(View.VISIBLE);
            mPrevImgBtn.setEnabled(true);
            mNextImgBtn.setVisibility(View.VISIBLE);
            mNextImgBtn.setEnabled(true);
            mReplayImgBtn.setVisibility(View.GONE);
            mReplayImgBtn.setEnabled(false);
            mContinuousImgBtn.setVisibility(View.VISIBLE);
            mContinuousImgBtn.setEnabled(true);
            mRandomImgBtn.setVisibility(View.GONE);
            mRandomImgBtn.setEnabled(false);
            break;
        case PlayMusicService.PLAY_MODE_RANDOM:
            mPrevImgBtn.setVisibility(View.VISIBLE);
            mPrevImgBtn.setEnabled(true);
            mNextImgBtn.setVisibility(View.VISIBLE);
            mNextImgBtn.setEnabled(true);
            mReplayImgBtn.setVisibility(View.GONE);
            mReplayImgBtn.setEnabled(false);
            mContinuousImgBtn.setVisibility(View.GONE);
            mContinuousImgBtn.setEnabled(false);
            mRandomImgBtn.setVisibility(View.VISIBLE);
            mRandomImgBtn.setEnabled(true);
            break;
        default:
            break;
        }
    }
    private void setStateButtons(int state)
    {
        switch (state) {
        case PlayMusicService.STATE_PLAYING:
            mSeekBar.setVisibility(View.VISIBLE);
            mSeekBar.setEnabled(true);
            mPlayImgBtn.setVisibility(View.GONE);
            mPlayImgBtn.setEnabled(false);
            mPauseImgBtn.setVisibility(View.VISIBLE);
            mPauseImgBtn.setEnabled(true);
            mStopImgBtn.setVisibility(View.VISIBLE);
            mStopImgBtn.setEnabled(true);
            break;
        case PlayMusicService.STATE_NONE:
        case PlayMusicService.STATE_STOPPED:
            mSeekBar.setVisibility(View.GONE);
            mSeekBar.setEnabled(false);
            mPlayImgBtn.setVisibility(View.VISIBLE);
            mPlayImgBtn.setEnabled(true);
            mPauseImgBtn.setVisibility(View.GONE);
            mPauseImgBtn.setEnabled(false);
            mStopImgBtn.setVisibility(View.VISIBLE);
            mStopImgBtn.setEnabled(false);
            break;
        case PlayMusicService.STATE_PAUSED:
            mSeekBar.setVisibility(View.VISIBLE);
            mSeekBar.setEnabled(true);
            mPlayImgBtn.setVisibility(View.VISIBLE);
            mPlayImgBtn.setEnabled(true);
            mPauseImgBtn.setVisibility(View.GONE);
            mPauseImgBtn.setEnabled(false);
            mStopImgBtn.setVisibility(View.VISIBLE);
            mStopImgBtn.setEnabled(true);
            break;
        default:
            break;
        }
    }
    private void setButtons(int playMode, int state)
    {
        setPlayModeButtons(playMode);
        setStateButtons(state);
    }
    private void updatePlaylist(ArrayAdapterPlay adapter)
    {
        List<String> playlist = mPlayMusicService.getPlayList();
        boolean empty = playlist.size() == 0;
        mSortAscImgBtn.setVisibility(empty ? View.GONE : View.VISIBLE);
        mSortAscImgBtn.setEnabled(!empty);
        mSortDescImgBtn.setVisibility(empty ? View.GONE : View.VISIBLE);
        mSortDescImgBtn.setEnabled(!empty);
        adapter.setData(playlist);  // automatically set to View.VISIBLE
//      if (mListView.getVisibility() != View.GONE)
//          mAdapter.setPositionState(mPlayMusicService.getPosition(), mPlayMusicService.getState());
    }

    private void init(ArrayAdapterPlay adapter)
    {
        int playMode = mPlayMusicService.getPlayMode();
        int state = mPlayMusicService.getState();
        setButtons(playMode, state);
        if (state == PlayMusicService.STATE_PLAYING || state == PlayMusicService.STATE_PAUSED) {
            visualizeViaSeekBar();
        } else {
            mSeekBar.setProgress(0);
        }
        updatePlaylist(adapter);
    }
    @Override
    public void onStateChange(int newState, int oldState, boolean refreshPlayList)
    {
        if (refreshPlayList) {
            updatePlaylist(mAdapter);
        }
////    Context context = (Context) getActivity();
////    if (context == null)
////        return;

        int playMode = mPlayMusicService.getPlayMode();
        setButtons(playMode, newState);
        if (mListView.getVisibility() != View.GONE)
            mAdapter.setPositionState(mPlayMusicService.getPosition(), newState);
////    String msg = PlayMusicService.STATE_STR[oldState] + " -> " + PlayMusicService.STATE_STR[newState];
////    Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
        if (oldState == PlayMusicService.STATE_READY && newState == PlayMusicService.STATE_PLAYING) {
            visualizeViaSeekBar();
//      } else {
//          mSeekBar.setProgress(0);
        }
    }
    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromTouch) {
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (mPlayMusicService == null)
            return;
        int state = mPlayMusicService.getState();
        if (state != PlayMusicService.STATE_PLAYING && state != PlayMusicService.STATE_PAUSED)
            return;
        MediaPlayer mp = mPlayMusicService.getMediaPlayer();
        if (mp == null)
            return;
        try {
            int curPos = mSeekBar.getProgress();
            mp.seekTo(curPos);
        } catch (IllegalStateException e) {e.printStackTrace();}
    }
////////////////////////////////////////////////////////////////////////////////

    private static class ArrayAdapterPlay extends ArrayAdapter<String> {
        private final LayoutInflater mInflater;

        int mPosition;
        int mState;
        public ArrayAdapterPlay(Context context, int position, int state) {
            super(context, android.R.layout.simple_list_item_1);
            mPosition = position;
            mState = state;
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void setPositionState(int position, int state)
        {
            mPosition = position;
            mState = state;
            notifyDataSetChanged();
        }        
        public void setData(List<String> entries) {
            clear();
            if (entries != null) {
                addAll(entries);
            }
        }
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            ViewHolder holder;
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.play_item, null);
                holder = new ViewHolder();
                holder.title = (TextView) convertView.findViewById(R.id.title);
                holder.play = (ImageView) convertView.findViewById(R.id.play);
                holder.pause = (ImageView) convertView.findViewById(R.id.pause);
                holder.stop = (ImageView) convertView.findViewById(R.id.stop);
                convertView.setTag(holder);
            } else {
                // Get the ViewHolder back to get fast access to the TextView
                // and the ImageView.
                holder = (ViewHolder) convertView.getTag();
            }
            holder.title.setText(getItem(position));
            if (position == mPosition) {
                if (mState == PlayMusicService.STATE_PLAYING) {
                    holder.play.setVisibility(View.VISIBLE);
                    holder.pause.setVisibility(View.GONE);
                    holder.stop.setVisibility(View.GONE);
                } else if (mState == PlayMusicService.STATE_PAUSED) {
                    holder.play.setVisibility(View.GONE);
                    holder.pause.setVisibility(View.VISIBLE);
                    holder.stop.setVisibility(View.GONE);
                } else {
                    holder.play.setVisibility(View.GONE);
                    holder.pause.setVisibility(View.GONE);
                    holder.stop.setVisibility(View.VISIBLE);
                }
            } else {
                holder.play.setVisibility(View.GONE);
                holder.pause.setVisibility(View.GONE);
                holder.stop.setVisibility(View.GONE);
            }
            return convertView;
        }
        static class ViewHolder {
            TextView title;
            ImageView play;
            ImageView pause;
            ImageView stop;
        }
    }

    private PlayMusicService mPlayMusicService;
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View playMusicLayoutView = inflater.inflate(R.layout.play_music, null);

        mSeekBar = (SeekBar) playMusicLayoutView.findViewById(R.id.seek);
        mSeekBar.setOnSeekBarChangeListener(this);
        mPrevImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.previous);
        mPlayImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.play);
        mStopImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.stop);
        mPauseImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.pause);
        mNextImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.next);
        mReplayImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.replay);
        mContinuousImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.continuous);
        mRandomImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.random);

        mPrevImgBtn.setVisibility(View.GONE);
        mPlayImgBtn.setVisibility(View.GONE);
        mStopImgBtn.setVisibility(View.GONE);
        mPauseImgBtn.setVisibility(View.GONE);
        mNextImgBtn.setVisibility(View.GONE);
        mReplayImgBtn.setVisibility(View.GONE);
        mContinuousImgBtn.setVisibility(View.GONE);
        mRandomImgBtn.setVisibility(View.GONE);

        mPrevImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.set(PlayMusicService.ACTION_PREV);
                    mPrevImgBtn.setEnabled(false);
                }
            }
        );
        mNextImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.set(PlayMusicService.ACTION_NEXT);
                    mNextImgBtn.setEnabled(false);
                }
            }
        );
        mPlayImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    int state = mPlayMusicService.getState();
                    if (state == PlayMusicService.STATE_PAUSED) {
                        mPlayMusicService.set(PlayMusicService.ACTION_TOGGLE);
                    } else {
                        mPlayMusicService.set(PlayMusicService.ACTION_PLAY);
                    }
                    mPlayImgBtn.setEnabled(false);
                }
            }
        );
        mPauseImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    int state = mPlayMusicService.getState();
                    if (state == PlayMusicService.STATE_PLAYING) {
                        mPlayMusicService.set(PlayMusicService.ACTION_TOGGLE);
                        mPauseImgBtn.setEnabled(false);
                    }
                }
            }
        );
        mStopImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    int state = mPlayMusicService.getState();
                    if (state == PlayMusicService.STATE_PLAYING || state == PlayMusicService.STATE_PAUSED) {
                        mPlayMusicService.set(PlayMusicService.ACTION_STOP);
                        mStopImgBtn.setEnabled(false);
                    }
                }
            }
        );

        // PLAY_MODE_CONTINUOUS -> PLAY_MODE_RANDOM -> PLAY_MODE_REPLAY
        mReplayImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.setPlayMode(PlayMusicService.PLAY_MODE_CONTINUOUS);
                    setPlayModeButtons(PlayMusicService.PLAY_MODE_CONTINUOUS);
                }
            }
        );
        mContinuousImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //ImageButton ib = (ImageButton) v;
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.setPlayMode(PlayMusicService.PLAY_MODE_RANDOM);
                    setPlayModeButtons(PlayMusicService.PLAY_MODE_RANDOM);
                }
            }
        );
        mRandomImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.setPlayMode(PlayMusicService.PLAY_MODE_REPLAY);
                    setPlayModeButtons(PlayMusicService.PLAY_MODE_REPLAY);
                }
            }
        );

        mSortAscImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.sort_asc);
        mSortDescImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.sort_desc);
        mUpImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.up);
        mDownImgBtn = (ImageButton) playMusicLayoutView.findViewById(R.id.down);
        mListView = (ListView) playMusicLayoutView.findViewById(android.R.id.list);
        mSortAscImgBtn.setVisibility(View.VISIBLE);
        mSortDescImgBtn.setVisibility(View.VISIBLE);
        mUpImgBtn.setVisibility(View.VISIBLE);
        mDownImgBtn.setVisibility(View.GONE);
        mListView.setVisibility(View.VISIBLE);
        mSortAscImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.sort(PlayMusicService.SORT_BY_ASC);
                    updatePlaylist(mAdapter);
                }
            }
        );
        mSortDescImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mPlayMusicService == null)
                        return;
                    mPlayMusicService.sort(PlayMusicService.SORT_BY_DESC);
                    updatePlaylist(mAdapter);
                }
            }
        );
        mUpImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    mListView.setVisibility(View.GONE);
                    mSortAscImgBtn.setVisibility(View.GONE);
                    mSortAscImgBtn.setEnabled(false);
                    mSortDescImgBtn.setVisibility(View.GONE);
                    mSortDescImgBtn.setEnabled(false);
                    mUpImgBtn.setVisibility(View.GONE);
                    mUpImgBtn.setEnabled(false);
                    mDownImgBtn.setVisibility(View.VISIBLE);
                    mDownImgBtn.setEnabled(true);
                }
            }
        );
        mDownImgBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    mListView.setVisibility(View.VISIBLE);
                    mAdapter.setPositionState(mPlayMusicService.getPosition(), mPlayMusicService.getState());
                    mSortAscImgBtn.setVisibility(View.VISIBLE);
                    mSortAscImgBtn.setEnabled(true);
                    mSortDescImgBtn.setVisibility(View.VISIBLE);
                    mSortDescImgBtn.setEnabled(true);
                    mUpImgBtn.setVisibility(View.VISIBLE);
                    mUpImgBtn.setEnabled(true);
                    mDownImgBtn.setVisibility(View.GONE);
                    mDownImgBtn.setEnabled(false);
                }
            }
        );

        mListView.setOnItemClickListener(
            new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> lv, View v, int position, long id)
                {
                    boolean r = mPlayMusicService.setPosition(position);
                    if (!r)
                        return;

                    int state = mPlayMusicService.getState();
                    if (state == PlayMusicService.STATE_PLAYING || state == PlayMusicService.STATE_PAUSED) {
                        mPlayMusicService.set(PlayMusicService.ACTION_STOP);
                        if (state == PlayMusicService.STATE_PLAYING)
                            mPlayMusicService.set(PlayMusicService.ACTION_PLAY);
                    } else {
                        mAdapter.setPositionState(mPlayMusicService.getPosition(), state);
                    }
                }
            }
        );
        TextView emptyText = (TextView) playMusicLayoutView.findViewById(android.R.id.empty);
        mListView.setEmptyView(emptyText);
        mAdapter = new ArrayAdapterPlay((Context) getActivity(), 0, PlayMusicService.STATE_STOPPED);
        mListView.setAdapter(mAdapter);
        mPlayMusicService = LoadActivity.mPlayMusicService;
        mPlayMusicService.registerOnStateChangeListener(PlayMusicMenuDialogFragment.this);
        init(mAdapter);

        mCancelBtn = (Button) playMusicLayoutView.findViewById(R.id.cancel);
        mBrowseBtn = (Button) playMusicLayoutView.findViewById(R.id.browse);
        mAssetsBtn = (Button) playMusicLayoutView.findViewById(R.id.assets);
        mCancelBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    PlayMusicMenuDialogFragment.this.getDialog().cancel();
                }
            }
        );
        mBrowseBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //PlayMusicMenuDialogFragment.buildAlertDialog((Context) getActivity(), "hello, world").show();
                    boolean browseAssets = false;
                    String homeDir = browseAssets ? "" : (LoadActivity.getDickRootPath().getAbsolutePath() + "/../");
                    DialogFragment fileBrowserFragment = FileBrowserDialogFragment.newInstance(homeDir, browseAssets, true);
                    fileBrowserFragment.setCancelable(false);
                    fileBrowserFragment.show(getFragmentManager(), "file browser dialog");
                LoadActivity.FOLDER_EXPLORER.reset();
                }
            }
        );
        mAssetsBtn.setVisibility(View.GONE);
        mAssetsBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //PlayMusicMenuDialogFragment.buildAlertDialog((Context) getActivity(), "hello, world").show();
                    boolean browseAssets = true;
                    String homeDir = browseAssets ? "" : (LoadActivity.getDickRootPath().getAbsolutePath() + "/../");
                    DialogFragment fileBrowserFragment = FileBrowserDialogFragment.newInstance(homeDir, browseAssets, true);
                    fileBrowserFragment.setCancelable(false);
                    fileBrowserFragment.show(getFragmentManager(), "file browser dialog");
                }
            }
        );
        return new AlertDialog.Builder(getActivity())
            .setCancelable(false)
            .setView(playMusicLayoutView)
/**
 * in another activity
 */
/*
            .setNegativeButton(
                "browse",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        Context context = (Context) getActivity();
                        Intent intent = new Intent(context, FileBrowserActivity.class);
                        boolean browseAssets = false;
                        intent.putExtra(FileBrowserDialogFragment.HOME_DIR, browseAssets ? "" : (LoadActivity.getDickRootPath().getAbsolutePath() + "/../"));
                        intent.putExtra(FileBrowserDialogFragment.BROWSE_ASSETS, browseAssets);
                        ////context.startActivity(intent);
                        startActivityForResult(intent, GET_CODE);
                    }
                }
            )
            .setNeutralButton(
                "assets",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        Context context = (Context) getActivity();
                        Intent intent = new Intent(context, FileBrowserActivity.class);
                        boolean browseAssets = true;
                        intent.putExtra(FileBrowserDialogFragment.HOME_DIR, browseAssets ? "" : (LoadActivity.getDickRootPath().getAbsolutePath() + "/../"));
                        intent.putExtra(FileBrowserDialogFragment.BROWSE_ASSETS, browseAssets);
                        ////context.startActivity(intent);
                        startActivityForResult(intent, GET_CODE);
                    }
                }
            )
 */
            .create();
    }
}
