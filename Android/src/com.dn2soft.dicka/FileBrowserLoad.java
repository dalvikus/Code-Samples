package com.dn2soft.dicka;
import com.dn2soft.dicka.FolderExplorer.EntryEx;
import com.dn2soft.dicka.FolderExplorer.Folder;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileFilter;
//SE//import com.dn2soft.dicka.FileBrowserDialogFragment.StackElement;


import android.os.AsyncTask;

import android.util.Log;


import java.util.List;
import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;



public final class FileBrowserLoad extends AsyncTask<String, Void, List<EntryEx>> {
    private static final String TAG = "FileBrowserLoad";

    final FileBrowserDialogFragment mFileBrowserDialogFragment;
    final String mDirPathname;
    public FileBrowserLoad(FileBrowserDialogFragment fileBrowserListFraggment, String dirPathname) {
        mFileBrowserDialogFragment = fileBrowserListFraggment;
        mDirPathname = dirPathname;
    }

    @Override
    protected void onPreExecute()
    {
    }

    @Override
    protected void onPostExecute(List<EntryEx> entryExList)
    {
        // Only the original thread that created a view hierarchy can touch its views.
        mFileBrowserDialogFragment.addViewToDirLayout(LoadActivity.FOLDER_EXPLORER.getCurrentFolder().getDisplayFilename());
        mFileBrowserDialogFragment.showCurrentDirView();
        mFileBrowserDialogFragment.adapter().setData(entryExList);
    }

    @Override
    protected List<EntryEx> doInBackground(String... zipFiles)
    {
/*
    String childIdStr = Folder.getIdStr(zipFiles, mDirPathname);
    Log.i(TAG, "childIdStr: |" + childIdStr + "|");
    Folder currentFolder = LoadActivity.FOLDER_EXPLORER.getCurrentFolder();
    if (currentFolder != null) {
        Folder childFolder = currentFolder.getChildFolder(childIdStr);
        if (childFolder != null)
            childFolder.print();
    }
        String[] entries = null;
        if (zipFiles.length > 0) {
            if (mDirPathname.isEmpty()) {
                entries = EntryEx.lsZipFile(LoadActivity.CONTAINER, mFileBrowserDialogFragment.fileFilter(), zipFiles);
                mFileBrowserDialogFragment.setZipEntries(entries);
            } else {
                entries = mFileBrowserDialogFragment.zipEntries();
            }
        } else {
            entries = EntryEx.lsFolder(mFileBrowserDialogFragment.fileFilter(), mDirPathname);
        }

        List<EntryEx> entryExList = null;
//SE//  ArrayList<StackElement> stackList = mFileBrowserDialogFragment.stackList();
        if (zipFiles.length > 0) { // form zip file
            entryExList = EntryEx.makeEntryExFromZipFile(
                entries,
                //%%LoadActivity.CONTAINER, mFileBrowserDialogFragment.fileFilter(), zipFiles,
                mDirPathname
            );
//SE//      int indexZipFiles = stackList.size();
//SE//      stackList.add(new StackElement(null, null, zipFiles, entries, null));     // Type 2
//SE//      stackList.add(new StackElement("", entryExList, null, null, indexZipFiles));// Type 3
        } else {
            entryExList = EntryEx.makeEntryExFromFolder(entries);
//SE//      stackList.add(new StackElement(mDirPathname, entryExList, null, null, null)); // Type 1
        }
    Map<String, Folder> childrenFolder = new HashMap<String, Folder>();
    if (entryExList != null) {
        for (EntryEx e: entryExList) {
            String name = e.getName();
            int type = e.getType();
            if (type == EntryEx.TYPE_DIR) {
                //Log.i(TAG, "    |" + name + "|: DIR");
                childrenFolder.put(Folder.getIdStr(zipFiles, name), new Folder(zipFiles, name));
            } else if (type == EntryEx.TYPE_ZIP) {
                //Log.i(TAG, "    |" + name + "|: ZIP");
                int len = zipFiles.length;
                String[] zipFiles1 = Arrays.copyOf(zipFiles, len + 1);
                zipFiles1[len] = name;
                childrenFolder.put(Folder.getIdStr(zipFiles1, ""), new Folder(zipFiles1, ""));
            }
        }
    }
    Folder folder = new Folder(zipFiles, mDirPathname, zipFiles.length > 0 ? entries : null, entryExList, childrenFolder);
    LoadActivity.FOLDER_EXPLORER.setCurrentFolder(childIdStr, folder);
    LoadActivity.FOLDER_EXPLORER.addFolderToHistory(folder);
    LoadActivity.FOLDER_EXPLORER.printFolderTree(childIdStr);
 */
        Folder currentFolder = LoadActivity.FOLDER_EXPLORER.getCurrentFolder();
        Folder childFolder = null;
        List<EntryEx> entryExList = null;
        if (currentFolder == null) {
            // neither root folder nor current folder is set;
            // will be set through FolderExplorer.setCurrentFolder below
            currentFolder = FileBrowserLoad.setFolder(new Folder(zipFiles, mDirPathname), mFileBrowserDialogFragment.fileFilter());
            if (currentFolder == null)
                return null;
        } else {
            assert currentFolder.isAlreadySet();
            currentFolder = FileBrowserLoad.getChildFolder(currentFolder, zipFiles, mDirPathname, mFileBrowserDialogFragment.fileFilter());
            if (currentFolder == null)
                return null;
        }
        entryExList = currentFolder.entryExList;
        String childIdStr = Folder.getIdStr(zipFiles, mDirPathname);
        LoadActivity.FOLDER_EXPLORER.setCurrentFolder(childIdStr, currentFolder);
        LoadActivity.FOLDER_EXPLORER.addFolderToHistory(currentFolder);
        LoadActivity.FOLDER_EXPLORER.printFolderTree(childIdStr);
        return entryExList;
    }

    /**
     * set List<EntryEx> from Folder defined by zipFiles or pathname or both
     *      zipFiles.length pathname.isEmpty
     *      = 0             True                only for root in assets
     *      = 0             False               directory in file system or assets
     *      > 0             True                "root" in zip file
     *      > 0             False               non-root directory in zip file
     *
     * test if all are set in the folder
     *      Folder.isAlreadySet
     *      True        just return them
     *      False       first set all, and return them
     *
     * INPUT
     *  1) Folder itself with (zipFiles, pathname)
     *  2) parent Folder for child Folder with (zipFiles, pathname)
     */
    static
    Folder
    getChildFolder(
        Folder              parentFolder,
        String[]            zipFiles,
        String              pathname,
        ZipFileExFileFilter fileFilter
    )
    {
        if (parentFolder == null) {
            //return FileBrowserLoad.setFolder(new Folder(zipFiles, pathname));
            return null;
        }
        if (!parentFolder.isAlreadySet()) {
            //parentFolder = FileBrowserLoad.setFolder(parentFolder);
            return null;
        }
        String childIdStr = Folder.getIdStr(zipFiles, pathname);
        Folder childFolder = parentFolder.getChildFolder(childIdStr);
        if (childFolder == null) {  // no child with (zipFiles, pathname)
            return null;
        }
        if (childFolder.isAlreadySet()) {
Log.i(TAG, "reuse: " + childFolder);
            return childFolder;
        }
        childFolder = FileBrowserLoad.setFolder(childFolder, fileFilter);
        if (childFolder == null)
            return null;
        parentFolder.setChildFolder(childIdStr, childFolder);
        return childFolder;
    }

    static
    Folder
    setFolder(
        Folder              folder,
        ZipFileExFileFilter fileFilter
    )
    {
        if (folder.isAlreadySet())
            return folder;
        String[] zipFiles = folder.zipFiles;
        String pathname = folder.pathname;

        String[] entries = null;
        String[] zipEntries = null;
        if (zipFiles.length > 0) {
            zipEntries = EntryEx.lsZipFile(LoadActivity.CONTAINER, fileFilter, zipFiles);
            // zipEntries is null for invalid zip file;
            // even in this case, Folder is valid with zero-length mChildrenFolder;
            // see below
            entries = ZipFileEx.filterByDirectory(zipEntries, pathname);
        } else {
            entries = EntryEx.lsFolder(fileFilter, pathname);
        }

        List<EntryEx> entryExList = null;
        if (zipFiles.length > 0) {
            entryExList = EntryEx.makeEntryExFromZipFile(
                zipEntries,
                pathname
            );
        } else {
            entryExList = EntryEx.makeEntryExFromFolder(entries);
        }
        Map<String, Folder> childrenFolder = new HashMap<String, Folder>();
        if (entryExList != null) {
            for (EntryEx e: entryExList) {
                String name = e.getName();
                int type = e.getType();
                if (type == EntryEx.TYPE_DIR) {
                    //Log.i(TAG, "    |" + name + "|: DIR");
                    childrenFolder.put(Folder.getIdStr(zipFiles, name), new Folder(zipFiles, name));
                } else if (type == EntryEx.TYPE_ZIP) {
                    //Log.i(TAG, "    |" + name + "|: ZIP");
                    int len = zipFiles.length;
                    String[] zipFiles1 = Arrays.copyOf(zipFiles, len + 1);
                    zipFiles1[len] = name;
                    childrenFolder.put(Folder.getIdStr(zipFiles1, ""), new Folder(zipFiles1, ""));
                }
            }
        }
        folder.setFolderSecondHalf(zipEntries, entryExList, childrenFolder);
        return folder;
//      return new Folder(zipFiles, pathname, zipFiles.length > 0 ? zipEntries : null, entryExList, childrenFolder);
    }
}
