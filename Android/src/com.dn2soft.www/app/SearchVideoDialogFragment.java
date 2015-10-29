package com.dn2soft.www.app;
import com.dn2soft.dicka.LoadActivity;
import com.dn2soft.www.Tvday;
import com.dn2soft.www.Tvday.Entry;
import com.dn2soft.www.Tvday.DownloadWatcher;
import com.dn2soft.dicka.R;

import android.widget.TextView;
import android.widget.ProgressBar;
import android.widget.Button;
import android.widget.AdapterView;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import android.content.Context;

import android.util.Log;

import android.app.DialogFragment;
import android.app.Dialog;
import android.app.AlertDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

import android.content.Intent;
import android.content.DialogInterface;
import android.os.AsyncTask;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.io.File;
import android.os.Looper;
import android.os.Handler;
import android.os.Message;


public class SearchVideoDialogFragment extends DialogFragment implements AdapterView.OnItemClickListener {
    private static final String TAG = "SearchVideoDialogFragment";
    private static final String SEARCH_STR = "Search String";
    private static final int REQ_CODE_UPDATE = 0;

    private String mSearchStr;
    public static SearchVideoDialogFragment newInstance(CharSequence searchStr)
    {
        SearchVideoDialogFragment fragment = new SearchVideoDialogFragment();
        Bundle args = new Bundle();
        args.putCharSequence(SEARCH_STR, searchStr);
        fragment.setArguments(args);
        return fragment;
    }
    private Handler mHandler;
    private int mCurrentPageNo;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Bundle args = getArguments();
        CharSequence cs = args.getCharSequence(SEARCH_STR, null);
        mSearchStr = cs == null ? null : cs.toString();
        mHandler = new Handler();//Looper.getMainLooper());
        mCurrentPageNo = 1;
    }


    ListView mListView;
    Button mCancelBtn;
    Button mNextBtn;
    Button mPrevBtn;
    ArrayAdapterSearch mAdapter;

    private static class ArrayAdapterSearch extends ArrayAdapter<Entry> {
        private final LayoutInflater mInflater;

        public ArrayAdapterSearch(Context context) {
            super(context, android.R.layout.simple_list_item_1);
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void setData(List<Entry> entries)
        {
            clear();
            if (entries != null)
                addAll(entries);
        }

        static class ViewHolder {
            TextView titleTextView;
            TextView dateTextView;
            ProgressBar downloadProgressBar;
        }
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            ViewHolder holder;
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.search_item, null);
                holder = new ViewHolder();
                holder.titleTextView = (TextView) convertView.findViewById(R.id.title);
                holder.dateTextView = (TextView) convertView.findViewById(R.id.date);
                holder.downloadProgressBar = (ProgressBar) convertView.findViewById(R.id.progress);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }
            Entry entry = getItem(position);
            holder.titleTextView.setText(String.format("%s%s", entry.titleStr, entry.noStr == null ? "" : (" [" + entry.noStr + "]")));
            holder.dateTextView.setText(entry.dateStr + " " + entry.timeStr);
            return convertView;
        }
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View searchListLayoutView = inflater.inflate(R.layout.search_list, null);

        mListView = (ListView) searchListLayoutView.findViewById(android.R.id.list);
        mListView.setVisibility(View.VISIBLE);

        mListView.setOnItemClickListener(this);
        TextView emptyText = (TextView) searchListLayoutView.findViewById(android.R.id.empty);
        mListView.setEmptyView(emptyText);
        mAdapter = new ArrayAdapterSearch((Context) getActivity());
        mListView.setAdapter(mAdapter);
        setData();

        mCancelBtn = (Button) searchListLayoutView.findViewById(R.id.cancel);
        mPrevBtn = (Button) searchListLayoutView.findViewById(R.id.previous_page);
        mNextBtn = (Button) searchListLayoutView.findViewById(R.id.next_page);
        mCancelBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    SearchVideoDialogFragment.this.getDialog().cancel();
                    for (Map.Entry<Integer, ThreadEx> entry: map.entrySet()) {
                        ThreadEx te = entry.getValue();
                        te.cancel();
                    }
                }
            }
        );
        mPrevBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mCurrentPageNo > 1) {
                        --mCurrentPageNo;
                        setData();
                    }
                }
            }
        );
        mNextBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    ++mCurrentPageNo;
                    setData();
                }
            }
        );

        return new AlertDialog.Builder(getActivity())
            .setCancelable(false)
            .setView(searchListLayoutView)
            .create();
    }
    Map<Integer, ThreadEx> map = new HashMap<Integer, ThreadEx>();
/*
    Map<Integer, Downloader> map = new HashMap<Integer, Downloader>();
 */
    @Override
    public void onItemClick(AdapterView<?> lv, View v, int position, long id)
    {
        File mp4Path = new File(new File(LoadActivity.getDickRootPath(), ".."), "mp4");
        mp4Path.mkdirs();
        Tvday.DOWNLOAD_DIRECTORY = mp4Path.getAbsolutePath();

        ArrayAdapterSearch adapter = (ArrayAdapterSearch) lv.getAdapter();
        Entry entry = adapter.getItem(position);
        entry.print();
        ProgressBar downloadProgressBar = (ProgressBar) v.findViewById(R.id.download);
        int visibility = downloadProgressBar.getVisibility();
        downloadProgressBar.setEnabled(false);
        if (visibility == View.VISIBLE) {   // in download or download is done
            ThreadEx te = map.get(entry.idx);
            Thread.State state = te.getState();
            switch (state) {
            case BLOCKED:
                Log.i(TAG, "Thread.State.BLOCKED");
                break;
            case NEW:
                Log.i(TAG, "Thread.State.NEW");
                break;
            case RUNNABLE:
                Log.i(TAG, "Thread.State.RUNNABLE");
                break;
            case TERMINATED:
                Log.i(TAG, "Thread.State.TERMINATED");
                break;
            case TIMED_WAITING:
                Log.i(TAG, "Thread.State.TIMED_WAITING");
                break;
            case WAITING:
                Log.i(TAG, "Thread.State.WAITING");
                break;
            default:
                Log.i(TAG, "Thread.State.default");
            }
            if (state != Thread.State.TERMINATED) {
                te.cancel();
            } else {
                te = new ThreadEx((Context) getActivity(), mHandler, downloadProgressBar, entry.idx);
                map.put(entry.idx, te);
                te.start();
                Log.i(TAG, "Result: |" + te.getResultStr() + "|");
            }
        } else {    // start download
            downloadProgressBar.setVisibility(View.VISIBLE);
    /*
            download(downloadProgressBar, entry);
     */
            ThreadEx te = new ThreadEx((Context) getActivity(), mHandler, downloadProgressBar, entry.idx);
            map.put(entry.idx, te);
            te.start();
/*
            Downloader downloader = new Downloader(downloadProgressBar, entry);
            map.put(entry.idx, downloader);
            downloader.execute();
 */
        }
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
    private static class ThreadEx extends Thread implements DownloadWatcher {
        Context mContext;
        ProgressBar mProgressBar;
        Handler mHandler;
        Integer mIdx;
        boolean mCancelled;
        ThreadEx(Context context, Handler handler, ProgressBar progressBar, Integer idx) {
            mContext = context;
            mHandler = handler;
            mProgressBar = progressBar;
            mIdx = idx;
            mCancelled = false;
        }
        void cancel()
        {
            Thread.State state = getState();
            if (state == Thread.State.TERMINATED) {
                return;
            } else if (state == Thread.State.RUNNABLE) {
                mCancelled = true;
                try {
                    join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        String mResultStr;
        String getResultStr() {return mResultStr;}
        @Override
        public void run()
        {
            String playLinkUrl = Tvday.getPlayLinkUrl(mIdx);
            Log.i(TAG, "" + playLinkUrl);
            //download(this);
            mResultStr = Tvday.download(mIdx, playLinkUrl, true, this);
            //Tvday.download(mIdx, "http://archive.ubuntu.com/ubuntu/dists/precise-updates/main/installer-i386/current/images/quantal-netboot/boot.img.gz", true, this);
            mHandler.post(
                new Runnable() {
                    @Override
                    public void run() {
                        buildAlertDialog(mContext, mResultStr).show();
                    }
                }
            );
        }

        @Override   // Tvday.DownloadWatcher
        public void setProgress(long current, long total)
        {
            int progress = (int) (current / (float) total * 100);
            mProgressBar.setProgress(progress);
        }
        @Override   // Tvday.DownloadWatcher
        public boolean cancelled()
        {
            return mCancelled;
        }
    }

    private void setData()
    {
        mAdapter.clear();
        new TvdayWrapper(mAdapter).execute(mSearchStr, String.valueOf(mCurrentPageNo));
    }

    private static class TvdayWrapper extends AsyncTask<String, Void, List<Entry>> {
        final ArrayAdapterSearch mAdapter;
        TvdayWrapper(ArrayAdapterSearch adapter) {
            mAdapter = adapter;
        }

        @Override
        protected List<Entry> doInBackground(String...searchStr)
        {
            if (searchStr.length == 0)
                return null;
            List<Entry> eal = new ArrayList<Entry>();
            int pageno = 1;
            if (searchStr.length > 1) {
                try {
                    pageno = Integer.parseInt(searchStr[1]);
                } catch (NumberFormatException e) {}
            }
            String utf8EncodedStr = null;
            if (searchStr.length > 0) {
                try {
                    utf8EncodedStr = URLEncoder.encode(searchStr[0], "UTF-8");
                } catch (UnsupportedEncodingException e) {}
            }
            Tvday.setList(eal, utf8EncodedStr, pageno, false, 1);
            return eal;
        }

        @Override
        protected void onPostExecute(List<Entry> entries)
        {
            mAdapter.setData(entries);
        }
    }
}
