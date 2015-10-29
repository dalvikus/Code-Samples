package com.dn2soft.dicka;
import com.dn2soft.dicka.FolderExplorer.EntryEx;
import com.dn2soft.dicka.FolderExplorer.Folder;
import com.dn2soft.dicka.ZipFileExIteration.FileElement;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileFilter;


import java.util.Arrays;
import java.util.List;


import android.app.DialogFragment;

import android.os.Bundle;
import android.os.Handler;

import android.content.Context;
import android.content.DialogInterface;

import android.util.Log;

import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.AdapterView;
import android.widget.HorizontalScrollView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;



public class FolderExplorerDialogFragment
    extends
        DialogFragment
    implements
        AdapterView.OnItemClickListener
{
    private static final String TAG = "FolderExplorerDialogFragment";

    private FolderExplorerAdapter mFolderExplorerAdapter;
    /* final */ boolean mBrowseAssets;
    /* final */ boolean mSimulateBack;
    boolean mSimulateBackInProgress;
    /* final */ ZipFileExFileFilter mFileFilter;
    ZipFileExFileFilter fileFilter() {return mFileFilter;}

    boolean showFolder(int index)
    {
        List<Folder> folderHistory = LoadActivity.FOLDER_EXPLORER.folderHistory();
        int size = folderHistory.size();
        if (index < 0 || index >= size) // invalid index
            return false;
        if (index == size - 1)  // already in the folder
            return true;
        while (folderHistory.size() > (1 + index))
            folderHistory.remove(folderHistory.size() - 1);
        Folder folder = folderHistory.get(index);
        LoadActivity.FOLDER_EXPLORER.setCurrentFolder(folder);
        List<Folder.FolderFlatten> folderFlattenList = LoadActivity.FOLDER_EXPLORER.getRootFolder().getFolderFlatten(0, folder.idStr);
        LoadActivity.FOLDER_EXPLORER.printFolderTree(folder.getIdStr(), folderFlattenList);
        mFolderExplorerAdapter.setData(folderFlattenList);
        // D1 / D2 / D3...
        // k = index
        // k = 0, 1, 2, 3, ...
        // i = 0, 2, 4, 6, ...
        //   = 2*k
        // n = 2*k + 1, so 
        mDirLayout.removeViews(2 * index + 1, mDirLayout.getChildCount() - (2 * index + 1) - 1);    // last 1 for folder explorer "[+]"
        ////mDirLayout.removeViews(index + 1, size - index - 1);
        return true;
    }

    public FolderExplorerDialogFragment() {}     // should be empty

    static FolderExplorerDialogFragment newInstance(boolean browseAssets, boolean simulateBack) {
        final FolderExplorerDialogFragment f = new FolderExplorerDialogFragment();
        f.setStyle(DialogFragment.STYLE_NO_TITLE, 0);
        Bundle args = new Bundle();
        args.putBoolean(FileBrowserDialogFragment.BROWSE_ASSETS, browseAssets);
        args.putBoolean(FileBrowserDialogFragment.SIMULATE_BACK, simulateBack);
        f.setArguments(args);
        return f;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

//SE//  mStackList = new ArrayList<StackElement>();
        Bundle args = getArguments();
        mBrowseAssets = args.getBoolean(FileBrowserDialogFragment.BROWSE_ASSETS);
        mSimulateBack = args.getBoolean(FileBrowserDialogFragment.SIMULATE_BACK);
        mSimulateBackInProgress = false;
        mFileFilter = mBrowseAssets ? LoadActivity.ASSET_AUDIO_FILTER : LoadActivity.FILE_SYSTEM_AUDIO_FILTER;
    }

    Context mContext;
    Handler mHandler = new Handler();
    HorizontalScrollView mDirScrollView;
    LinearLayout mDirLayout;
    void addViewToDirLayout(String text)
    {
        int n = mDirLayout.getChildCount();
        if (n == 0) {
            TextView treeView = new TextView(mContext, null, R.style.DisplayFont);
            treeView.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        getTargetFragment().onActivityResult(getTargetRequestCode(), 0, getActivity().getIntent());
                        FolderExplorerDialogFragment.this.dismiss();
                    }
                }
            );
            treeView.setText("<[X]>");
            treeView.setPadding(16, 8, 16, 16);
            treeView.setTextAppearance(mContext, R.style.DisplayFont);
            mDirLayout.addView(treeView);
        }
        TextView dirView = new TextView(mContext, null, R.style.DisplayFont);
        dirView.setPadding(16, 8, 16, 16);
        ////final int k = mDirLayout.getChildCount();
        if (n > 0) {
            TextView dividerView = new TextView(mContext, null, R.style.DisplayFont);
            dividerView.setText("/");
            mDirLayout.addView(dividerView, n - 1);
        }
        // ""
        // D1 a
        // D1 / D2 a
        // D1 / D2 / D3 a
        //
        // n = 0, 2, 4, 6, ...
        // j = X, 1, 3, 5, ...  ; /
        //   = ?, n - 1
        // k = 0, 1, 2, 3, ...  ; D in mFolderHistory
        //   = n / 2
        // i = 0, 2, 4, 6, ...  ; D in mDirLayout
        final int k = n / 2;
        //Log.i(TAG, "n = " + n + ", k = " + k);
        dirView.setText(k == 0 ? "ROOT" : text);
        dirView.setTextAppearance(mContext, R.style.DisplayFont);
        dirView.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    showFolder(k);
                }
            }
        );
        mDirLayout.addView(dirView, 2 * k);
    }
    void showCurrentDirView()
    {
        mHandler.postDelayed(
            new Runnable() {
                @Override
                public void run() {
                    mDirScrollView.fullScroll(HorizontalScrollView.FOCUS_RIGHT);
                }
            },
            100L
        );
    }
    @Override
    public void onStart()
    {
        super.onStart();
Log.i(TAG, "onStart");
        mDirLayout.removeViews(0, mDirLayout.getChildCount());
        List<Folder> folderHistory = LoadActivity.FOLDER_EXPLORER.folderHistory();
        for (Folder folder: folderHistory) {
            addViewToDirLayout(folder.getDisplayFilename());
        }
        showCurrentDirView();
        showFolder(folderHistory.size() - 1);
    }
/*
    @Override
    public void onResume()
    {
        super.onResume();
    }
    @Override
    public void onPause()
    {
        super.onPause();
    }
    @Override
    public void onStop()
    {
        super.onStop();
    }
    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }
 */
    @Override
    public void onDetach()
    {
        super.onDetach();
        getTargetFragment().onActivityResult(getTargetRequestCode(), 0, getActivity().getIntent());
    }

    @Override
    public View onCreateView(
        LayoutInflater  inflater,
        ViewGroup       container,
        Bundle          savedInstanceState
    )
    {
        super.onCreate(savedInstanceState);
        mContext = (Context) getActivity();

        if (mSimulateBack) {
            this.getDialog().setOnKeyListener(
                new DialogInterface.OnKeyListener() {
                    @Override
                    public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                        if (keyCode == KeyEvent.KEYCODE_BACK) {
                            if (event.getAction() == KeyEvent.ACTION_DOWN)
                                mSimulateBackInProgress = simulateBack();
                            return mSimulateBackInProgress;
                        }
                        return false;
                    }
                }
            );
        }

        View rootView = inflater.inflate(R.layout.folder_explorer, container, false);
        mDirScrollView = (HorizontalScrollView) rootView.findViewById(R.id.dir_scroll);       
        mDirLayout = (LinearLayout) rootView.findViewById(R.id.dir_layout);
        ListView listView = (ListView) rootView.findViewById(android.R.id.list);
        listView.setEmptyView(rootView.findViewById(android.R.id.empty));
        listView.setOnItemClickListener(this);
        mFolderExplorerAdapter = new FolderExplorerAdapter((Context) getActivity());
        listView.setAdapter(mFolderExplorerAdapter);
        return rootView;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        List<Folder.FolderFlatten> folderFlattenList = null;
        Folder rootFolder = LoadActivity.FOLDER_EXPLORER.getRootFolder();
        Folder currentFolder = LoadActivity.FOLDER_EXPLORER.getCurrentFolder();
        assert rootFolder != null && currentFolder != null;
        folderFlattenList = rootFolder.getFolderFlatten(0);
        mFolderExplorerAdapter.setData(folderFlattenList);
    }

    @Override   // AdapterView.OnItemClickListener
    public void onItemClick(AdapterView<?> parent, View view, int position, long id)
    {
        Folder.FolderFlatten ff = mFolderExplorerAdapter.getItem(position);
        Folder folder = ff.folder;
        int iconType = folder.getIconType();
        if (iconType == Folder.ICON_ERR)    // do nothing
            return;

        if (iconType == Folder.ICON_NOT_YET)    // set second half part
            FileBrowserLoad.setFolder(folder, mFileFilter);
        else if (iconType == Folder.ICON_CLOSED)
            folder.isOpened = true;
        else if (iconType == Folder.ICON_OPENED || iconType == Folder.ICON_OPENED_NUL)
            folder.isOpened = false;
        // redraw
        List<Folder.FolderFlatten> folderFlattenList = LoadActivity.FOLDER_EXPLORER.getRootFolder().getFolderFlatten(0);
        LoadActivity.FOLDER_EXPLORER.printFolderTree(folder.getIdStr(), folderFlattenList);
        mFolderExplorerAdapter.setData(folderFlattenList);
        if (!folder.equals(LoadActivity.FOLDER_EXPLORER.getCurrentFolder())) {
            LoadActivity.FOLDER_EXPLORER.setCurrentFolder(folder);
            LoadActivity.FOLDER_EXPLORER.addFolderToHistory(folder);
            addViewToDirLayout(folder.getDisplayFilename());
            showCurrentDirView();
        }
    }

    public boolean simulateBack()
    {
        return showFolder(LoadActivity.FOLDER_EXPLORER.folderHistory().size() - 2);
    }
}
