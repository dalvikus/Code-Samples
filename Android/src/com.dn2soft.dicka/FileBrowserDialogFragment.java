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
import android.content.Intent;

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



public class FileBrowserDialogFragment
    extends
        DialogFragment
    implements
        AdapterView.OnItemClickListener,
        AdapterView.OnItemLongClickListener
{
    private static final String TAG = "FileBrowserDialogFragment";

    public static final String HOME_DIR = "home directory";
    public static final String BROWSE_ASSETS = "browse assets";
    public static final String SIMULATE_BACK = "simulate back";


/*
    // similar to ZipFileExIteration.StackElement
    static class StackElement
    {
        // Type 1: entries in file system or assets
        // Type 2: entries in a zip file
        // Type 3: entries in a specific directory of the zip file
                                    //  Type 1@     Type 2      Type 3@     @: only visible
        String pathname;            //  non-null*   null        non-null*   *: for file system, starts with '/'; for others(assets, zip) without one
        List<EntryEx> entries;       //  non-null@   null        non-null@   @: used in setData
        String[] zipFiles;          //  null        non-null    null+
        String[] zipEntries;        //  null        non-null    null+
                                    //                                      +: to get zipFiles and zipEntries, use indexZipFiles
        Integer indexZipFiles;      //  null        null        non-null

        StackElement(String p, List<EntryEx> e, String[] z, String[] ze, Integer i) {
            pathname = p;
            entries = e;
            zipFiles = z;
            zipEntries = ze;
            indexZipFiles = i;
        }
    }
    ArrayList<StackElement> mStackList;  // possible ways:
                                        // Type 1,...,Type 1,Type2,(Type 2,Type3)*
    ArrayList<StackElement> stackList() {return mStackList;}
    // show i-th StackElement
    boolean show(int index)
    {
        if (index < 0 || index >= mStackList.size())
            return false;
        // remove all "tail" elements: index + 1, ...
        while (mStackList.size() > (1 + index))
            mStackList.remove(mStackList.size() - 1);
        StackElement se = mStackList.get(index);
        // Type 1 or Type 3 only
        if (se.zipFiles != null) {  // Type 2
            // should raise an exception
            return false;
        }
        if (se.indexZipFiles == null) { // Type 1
            mZipFiles = new String[0];
            mZipEntries = new String[0];
        } else {                        // Type 3
            StackElement ze = mStackList.get(se.indexZipFiles);
            mZipFiles = ze.zipFiles;
            mZipEntries = ze.zipEntries;
        }
        mAdapter.setData(se.entries);
        return true;
    }
    boolean pop()
    {
        if (mStackList.size() == 0)
            return false;
        mStackList.remove(mStackList.size() - 1);
        if (mStackList.size() == 0)
            return false;
        StackElement se = mStackList.get(mStackList.size() - 1);
        // Type 1 or Type 2 or Type 3
        if (se.zipFiles != null) {  // Type 2
            mStackList.remove(mStackList.size() - 1);
        }
        int len = mFolderHistory.size();    // len >= 2
        mFolderHistory.remove(len - 1);
        mCurrentFolder = mFolderHistory.get(len - 2);
        printFolderTree(mCurrentFolder.getIdStr());
        return show(mStackList.size() - 1);
    }
 */

    // This is the Adapter being used to display the list's data.
    private FolderAdapter mAdapter;
    FolderAdapter adapter() {return mAdapter;}

    /* final */ String mHomeDir;    // "" for assets, "/" or "/ano/ther/" for file system
    String homeDir() {return mHomeDir;}
    /* final */ boolean mBrowseAssets;
    boolean browseAssets() {return mBrowseAssets;}
    /* final */ boolean mSimulateBack;
    boolean mSimulateBackInProgress;
    /* final */ ZipFileExFileFilter mFileFilter;
    ZipFileExFileFilter fileFilter() {return mFileFilter;}
    String[] mZipFiles;
    String[] zipFiles() {return mZipFiles;}

    boolean showFolder(int index)
    {
        return showFolder(index, false);
    }
    boolean showFolder(int index, boolean setDataForced)
    {
        List<Folder> folderHistory = LoadActivity.FOLDER_EXPLORER.folderHistory();
        int size = folderHistory.size();
        if (index < 0 || index >= size) // invalid index
            return false;
        if (index == size - 1) {  // already in the folder
            if (setDataForced) {
                Folder folder = folderHistory.get(index);
                mAdapter.setData(folder.getEntryExList());
            }
            return true;
        }
        while (folderHistory.size() > (1 + index))
            folderHistory.remove(folderHistory.size() - 1);
        Folder folder = folderHistory.get(index);
        LoadActivity.FOLDER_EXPLORER.setCurrentFolder(folder);
        LoadActivity.FOLDER_EXPLORER.printFolderTree(folder.getIdStr());
        mZipFiles = folder.getZipFiles();
        mAdapter.setData(folder.getEntryExList());
        // D1 / D2 / D3...
        // k = index
        // k = 0, 1, 2, 3, ...
        // i = 0, 2, 4, 6, ...
        //   = 2*k
        // n = 2*k + 1, so 
        mDirLayout.removeViews(2 * index + 1, mDirLayout.getChildCount() - (2 * index + 1) - 2);    // 2: two extra views
        ////mDirLayout.removeViews(index + 1, size - index - 1);
        return true;
    }

    public FileBrowserDialogFragment() {}     // should be empty

    static FileBrowserDialogFragment newInstance(String homeDir, boolean browseAssets, boolean simulateBack) {
        final FileBrowserDialogFragment f = new FileBrowserDialogFragment();
        f.setStyle(DialogFragment.STYLE_NO_TITLE, 0);
        Bundle args = new Bundle();
        args.putString(FileBrowserDialogFragment.HOME_DIR, homeDir);
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
        mHomeDir = args.getString(FileBrowserDialogFragment.HOME_DIR);
        mBrowseAssets = args.getBoolean(FileBrowserDialogFragment.BROWSE_ASSETS);
        mSimulateBack = args.getBoolean(FileBrowserDialogFragment.SIMULATE_BACK);
        mSimulateBackInProgress = false;
        mFileFilter = mBrowseAssets ? LoadActivity.ASSET_AUDIO_FILTER : LoadActivity.FILE_SYSTEM_AUDIO_FILTER;
        mZipFiles = new String[0];
        LoadActivity.FOLDER_EXPLORER.reset();
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
                        DialogFragment folderExplorer = FolderExplorerDialogFragment.newInstance(mBrowseAssets, true);
                        folderExplorer.setCancelable(false);
                        folderExplorer.setTargetFragment(FileBrowserDialogFragment.this, 1);
                        folderExplorer.show(getFragmentManager(), "file browser dialog");
                    }
                }
            );
            treeView.setText("<[+]>");
            treeView.setPadding(16, 8, 16, 16);
            treeView.setTextAppearance(mContext, R.style.DisplayFont);
            mDirLayout.addView(treeView);

            TextView closeView = new TextView(mContext, null, R.style.DisplayFont);
            closeView.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        FileBrowserDialogFragment.this.dismiss();
                    }
                }
            );
            closeView.setText("<[X]>");
            closeView.setPadding(16, 8, 16, 16);
            closeView.setTextAppearance(mContext, R.style.DisplayFont);
            mDirLayout.addView(closeView);
        }
        TextView dirView = new TextView(mContext, null, R.style.DisplayFont);
        dirView.setPadding(16, 8, 16, 16);
        ////final int k = mDirLayout.getChildCount();
        if (n > 0) {
            TextView dividerView = new TextView(mContext, null, R.style.DisplayFont);
            dividerView.setText("/");
            mDirLayout.addView(dividerView, n - 2); // 2: two extra views
        }
        // ""
        // D1 a b
        // D1 / D2 a b
        // D1 / D2 / D3 a b
        //
        // n = 0, 3, 5, 7, ...
        // j = X, 1, 3, 5, ...  ; /
        //   = n - 2
        // k = 0, 1, 2, 3, ...  ; D in mFolderHistory
        //   = ?, (n - 1) / 2
        // i = 0, 2, 4, 6, ...  ; D in mDirLayout
        //   = 2 * k
        final int k = n == 0 ? 0 : (n - 1) / 2;
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
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        mDirLayout.removeViews(0, mDirLayout.getChildCount());
        List<Folder> folderHistory = LoadActivity.FOLDER_EXPLORER.folderHistory();
        for (Folder folder: folderHistory) {
            addViewToDirLayout(folder.getDisplayFilename());
        }
        showCurrentDirView();
        showFolder(folderHistory.size() - 1, true);
    }

    @Override
    public void onStart()
    {
        super.onStart();
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
    @Override
    public void onDetach()
    {
        super.onDetach();
    }
 */

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

        View rootView = null;
        rootView = inflater.inflate(R.layout.file_list, container, false);
/*
Log.i(TAG, "W: " + rootView.getMinimumWidth());
Log.i(TAG, "H: " + rootView.getMinimumHeight());
rootView.setMinimumWidth(100);
 */
        mDirScrollView = (HorizontalScrollView) rootView.findViewById(R.id.dir_scroll);       
        mDirLayout = (LinearLayout) rootView.findViewById(R.id.dir_layout);
        ListView listView = (ListView) rootView.findViewById(android.R.id.list);
        listView.setEmptyView(rootView.findViewById(android.R.id.empty));
        listView.setLongClickable(true);
        listView.setOnItemLongClickListener(this);
        listView.setOnItemClickListener(this);
        mAdapter = null;
        mAdapter = new FolderAdapter(
            getActivity(), this,
            (LinearLayout) rootView.findViewById(R.id.menu_layout),
            (TextView) rootView.findViewById(R.id.status)
        );
        listView.setAdapter(mAdapter);
        return rootView;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        // set mRootFolder, mCurrentFolder via setCurrentFolder
        new FileBrowserLoad(this, mHomeDir).execute();
    }

    @Override   // AdapterView.OnItemClickListener
    public void onItemClick(AdapterView<?> parent, View view, int position, long id)
    {
        ListView listView = (ListView) parent;
        EntryEx item = mAdapter.getItem(position);
        int type = item.getType();
        String pathname = item.getName();
        boolean selectionMode = ((FolderAdapter) listView.getAdapter()).isSelectionMode();
        if (selectionMode) {
            if (item.isSelectable()) {
                mAdapter.toggle(position);
            }
            return;
        }

        if (type == EntryEx.TYPE_DIR) {
            if (mZipFiles.length > 0) {
                new FileBrowserLoad(this, pathname).execute(mZipFiles);
            } else {
                new FileBrowserLoad(this, pathname).execute();
            }
        } else if (type == EntryEx.TYPE_ZIP) {
            int len = mZipFiles.length;
            mZipFiles = Arrays.copyOf(mZipFiles, len + 1);
            mZipFiles[len] = pathname;
            new FileBrowserLoad(this, "").execute(mZipFiles);
        }
    }

    @Override   // AdapterView.OnItemLongClickListener
    public boolean onItemLongClick (AdapterView<?> parent /* ListView */, View layout, int position, long id)
    {
        ListView listView = (ListView) parent;

        FolderAdapter adapter = (FolderAdapter) listView.getAdapter();
        if (adapter.isSelectionMode())
            return true;

        EntryEx item = (EntryEx) ((ListAdapter) adapter).getItem(position);
        boolean isSelectable = item.isSelectable();
        adapter.setSelectionMode(true, isSelectable ? position : -1);
        return true;    // handled here
    }

    public void done(List<FileElement> audioFiles)
    {
        Log.i(TAG, "# of audios = " + audioFiles.size());
        for (FileElement fe: audioFiles) {
            System.out.println(fe.msg());
            System.out.println("    |" + fe.tmpPathname + "|");
            System.out.println("    |" + ZipFileExIteration.FileElement.setJsonStr(fe) + "|");
        }
        LoadActivity.addAudioFiles(audioFiles);
        simulateBack();
        FileBrowserDialogFragment.this.getDialog().dismiss();
    }

    public boolean simulateBack()
    {
        if (mAdapter.isSelectionMode()) {
            mAdapter.setSelectionModeOff();
            return true;
        }
        return showFolder(LoadActivity.FOLDER_EXPLORER.folderHistory().size() - 2);
    }
}
