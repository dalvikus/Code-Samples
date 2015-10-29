package com.dn2soft.dicka;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileFilter;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;



class FolderExplorer
{
    static class EntryEx {
        public static final int TYPE_NUL = -1;
        public static final int TYPE_DIR = 0;
        public static final int TYPE_ZIP = 1;
        public static final int TYPE_MP3 = 2;
        public static final String getTypeStr(int type)
        {
            return type == TYPE_DIR ? "DIR" : (type == TYPE_ZIP ? "ZIP" : (type == TYPE_MP3 ? "MP3" : "NUL"));
        }
        final int mType;
        final String mName;
        final int mN;
        EntryEx(int type, String name, int n) {
            mType = type;
            mName = name;
            mN = n;
        }
        @Override
        public String toString()
        {
            return mName + ": " + EntryEx.getTypeStr(mType);
        }

        int getType() {return mType;}
        String getName() {return mName;}
        int getN() {return mN;}
        boolean isSelectable() {
            return true;
            //%%return ((mType == TYPE_DIR || mType == TYPE_ZIP) && (mN <= 0)) ? false : true;
        }
        String getDisplayName() {
            int len0 = mName.length();
            int startIndexForLastIndexOf;
            int endIndexForSubstring;
            if (mType == TYPE_DIR && mName.charAt(len0 - 1) == '/') {   // directory in BROWSE_FILES_IN_ASSETS
                startIndexForLastIndexOf = len0 - 1 - 1;    // skip last '/'
                endIndexForSubstring = len0 - 1;            // skip last '/'
            } else {
                startIndexForLastIndexOf = len0 - 1;
                endIndexForSubstring = len0;
            }
            return mName.substring(1 + mName.lastIndexOf("/", startIndexForLastIndexOf), endIndexForSubstring);
        }

        public static final String[] lsZipFile(ZipFileExContainer container, ZipFileExFileFilter fileFilter, String[] zipFiles)
        {
            try {
                return container.list(fileFilter.getInputStream(zipFiles[0]), zipFiles);
            } catch (java.io.IOException e) {e.printStackTrace();}
            return null;
        }

        public static String[] lsFolder(ZipFileExFileFilter fileFileter, String folder)
        {
            if (!fileFileter.isDirectory(folder))
                return null;
            String[] pathnames = fileFileter.listFiles(folder);
            List<String> dal = new ArrayList<String>();
            List<String> fal = new ArrayList<String>();
            for (String pathname: pathnames) {
                if (fileFileter.isDirectory(pathname)) {
                    dal.add(pathname + ":0");
                } else {
                    String filename = pathname.substring(1 + pathname.lastIndexOf("/"));
                    if (fileFileter.isValidFilename(filename)) {
                        String lowerExtStr = filename.substring(1 + filename.lastIndexOf(".")).toLowerCase();
                        if (lowerExtStr.equals("zip")) {
                            fal.add(pathname + ":-2");
                        } else {
                            fal.add(pathname + ":-1");
                        }
                    }
                }
            }
            return fileFileter.sort(dal, fal);
        }

        static List<EntryEx> makeEntryExFromZipFile(
            String[] zipEntries,
            //%%ZipFileExContainer container, ZipFileExFileFilter fileFilter, String[] zipFiles,
            String dir
        )
        {
            if (zipEntries == null)
                return null;
            String[] sa = ZipFileEx.filterByDirectory(zipEntries, dir);
            if (sa == null)
                return null;
            List<EntryEx> entryExList = new ArrayList<EntryEx>();
            for (String pathname: sa) {
                if (pathname.endsWith("/")) {
                    String d = pathname.substring(0, pathname.length() - 1);
                    String[] a = ZipFileEx.filterByDirectory(zipEntries, d);
                    entryExList.add(new EntryEx(EntryEx.TYPE_DIR, pathname, a.length));
                } else {
                    String filename = pathname.substring(1 + pathname.lastIndexOf("/"));
                    String lowerExtStr = filename.substring(1 + filename.lastIndexOf(".")).toLowerCase();
                    if (lowerExtStr.equals("zip")) {
                        //%%String[] args = Arrays.copyOf(zipFiles, zipFiles.length + 1);
                        //%%args[zipFiles.length] = pathname;
                        //%%String[] a = EntryEx.lsZipFile(container, fileFilter, args);
                        //%%a = ZipFileEx.filterByDirectory(a, "");
                        //%%entryExList.add(new EntryEx(EntryEx.TYPE_ZIP, pathname, a == null ? Integer.MIN_VALUE : a.length));
                        //%%entryExList.add(new EntryEx(EntryEx.TYPE_ZIP, pathname, a == null ? Integer.MIN_VALUE : a.length));
                        entryExList.add(new EntryEx(EntryEx.TYPE_ZIP, pathname, 0));
                    } else {
                        entryExList.add(new EntryEx(EntryEx.TYPE_MP3, pathname, -1));
                    }
                }
            }
            return entryExList;
        }
        static List<EntryEx> makeEntryExFromFolder(String[] entries)
        {
            if (entries == null)
                return null;
            List<EntryEx> entryExList = new ArrayList<EntryEx>();
            for (String pathnameWithColon: entries) {
                int i = pathnameWithColon.lastIndexOf(":");
                String pathname = pathnameWithColon.substring(0, i);
                int n0 = Integer.parseInt(pathnameWithColon.substring(i + 1));
                int type = EntryEx.TYPE_NUL;
                if (n0 >= 0)
                    type = EntryEx.TYPE_DIR;
                else {
                    String filename = pathname.substring(1 + pathname.lastIndexOf("/"));
                    String lowerExtStr = filename.substring(1 + filename.lastIndexOf(".")).toLowerCase();
                    type = lowerExtStr.equals("zip") ? EntryEx.TYPE_ZIP : EntryEx.TYPE_MP3;
                }
                int n = n0 >= 0 ? n0 : (n0 == -1 ? -1 : (n0 == Integer.MIN_VALUE ? Integer.MIN_VALUE : (-n0 - 2)));
                entryExList.add(new EntryEx(type, pathname, n));
            }
            return entryExList;
        }
    }

    static class Folder {
        String[] zipFiles;  // other zip files than the first one do not have a leading '/'
                            // zero-length is equivalent to null
        String[] getZipFiles() {return zipFiles;}
        String pathname;    // non-null
        String getPathname() {return pathname;}
        // if (zipFiles == null || zipFiles.length == 0) {
        //      // "pure" directory with path = pathname
        // } else { // zipFiles != null && zipFiles.length > 0
        //  if (pathname == "") {
        //      // zip path constructed from zipFiles
        //  } else {
        //      // other directory than "root" in a zip file
        //      // with path = pathname (no leading '/')
        //  }
        // }
        String idStr;   // each folder can be identified by this string; key in mFolderMap
        String getIdStr() {return idStr;}
        String[] zipEntries;    // non-null only if zipFiles.length > 0 and pathname == ""

        // both can be null; folder not explored yet; icon = "[?]"
        // zero-length means no entries
        List<EntryEx> entryExList;
        List<EntryEx> getEntryExList() {return entryExList;}
        boolean isAlreadySet() {return entryExList != null;}
        Map<String, Folder> childrenFolder;
        Folder getChildFolder(String idStr)
        {
            //return childrenFolder == null ? null : childrenFolder.get(idStr);
            return childrenFolder.get(idStr);
        }
        /**
         * return Folder whose id string is idStr
         *
         * <p>if there is no folder whose id string is idStr,
         * save the pair(idStr, folder) and return a given folder itself; Otherwise return saved folder
         */
        Folder setChildFolder(String idStr, Folder folder)
        {
            //if (childrenFolder == null)
            //    childrenFolder = new HashMap<String, Folder>();
            Folder f = childrenFolder.get(idStr);
            if (f == null || !f.isAlreadySet()) {
                childrenFolder.put(idStr, folder);
                return folder;
            }
            return f;
        }

        boolean isOpened;   // true: folder explored and child folders shown if any; icon = "[-]"
                            // false: folder explored and child folder not shown if any; icon = "[+]"
                            //  combined with childrenFolder, it determines the type of a folder
        public static final int ICON_NOT_YET    = 0;    // "[?]"; starting from 0
        public static final int ICON_CLOSED     = 1;    // "[+]"
        public static final int ICON_OPENED     = 2;    // "[-]"
        public static final int ICON_OPENED_NUL = 3;    // "[.]": no entries
        public static final int ICON_ERR        = 4;    // "[X]": not a zip
        public static final String[] ICON = new String[] {"[?]", "[+]", "[-]", "[.]", "[X]"};
        public int getIconType()
        {
            if (entryExList == null) {
                return childrenFolder == null ? ICON_NOT_YET : ICON_ERR;
            }
            // assert(childrenFolder != null)
            return isOpened ? (childrenFolder.size() > 0 ? ICON_OPENED : ICON_OPENED_NUL) : ICON_CLOSED;
        }

        public void setFolderSecondHalf(String[] ze, List<EntryEx> e, Map<String, Folder> childrenFolder)
        {
            this.zipEntries = ze;
            this.entryExList = e;
            this.childrenFolder = childrenFolder;
            isOpened = true;
        }
        Folder(String[] z, String p, String[] ze, List<EntryEx> e, Map<String, Folder> childrenFolder) {
            zipFiles = z;
            pathname = p;
            idStr = Folder.getIdStr(z, p);
            setFolderSecondHalf(ze, e, childrenFolder);
        }
        Folder(String[] z, String p) {
            zipFiles = z;
            pathname = p;
            idStr = Folder.getIdStr(z, p);

            zipEntries = null;  // zipFiles.length > 0: not set yet
            entryExList = null;
            childrenFolder = null;
            isOpened = false;
        }

        void print()
        {
            System.out.println("idStr: |" + idStr + "|");
            System.out.println("zipFiles: " + Arrays.asList(zipFiles));
            System.out.println("pathname: |" + pathname + "|");
            if (zipEntries != null)
                for (String s: zipEntries)
                    System.out.println("  |" + s + "|");
            if (entryExList == null)
                System.out.println("entryExList: null (not set yet)");
            else {
                for (EntryEx e: entryExList)
                    System.out.println("    |" + e.toString() + "|");
            }
            System.out.println("childrenFolder: " + childrenFolder);
        }

        /**
         * return encoded pathname
         *
         *  % - %25
         *  : - %3A
         *
         *  "%3A:" - "%%253A%3A"
         */
        static String encodedPathname(String pathname)
        {
            if (pathname == null)
                return null;
            return pathname.replaceAll("%", "%25").replaceAll(":", "%3A");
        }

        /**
         * return id string from zipFiles, pathname
         */
        static String getIdStr(String[] zipFiles, String pathname)
        {
            if (pathname == null)
                return null;
            if (zipFiles == null || zipFiles.length == 0)
                return Folder.encodedPathname(pathname);
            StringBuilder sb = new StringBuilder();
            sb.append(Folder.encodedPathname(zipFiles[0]));
            for (int i = 1; i < zipFiles.length; ++i) {
                sb.append(":");
                sb.append(Folder.encodedPathname(zipFiles[i]));
            }
            sb.append(":");
            sb.append(Folder.encodedPathname(pathname));
            return sb.toString();
        }

        public String getDisplayPathname()
        {
            if (!pathname.isEmpty()) {
                int len = pathname.length();
                if (zipFiles.length > 0)    // remove last '/'
                    --len;
                return pathname.substring(0, len);
            }
            int len = zipFiles.length;
            if (len == 0) { // "root" in assets
                return "ROOT";
            }
            return zipFiles[len - 1];
        }
        public String getDisplayFilename()
        {
            String displayPathname = getDisplayPathname();
            return displayPathname.substring(1 + displayPathname.lastIndexOf("/"));
        }
        // see ZipFileEx.filterByDirectory for sort
        int compare(Folder fB)
        {
            if (zipFiles.length == fB.zipFiles.length) {
                // both zip files or both directories
                if (pathname.isEmpty()) {   // both zip files
                    String zipFileA = zipFiles[zipFiles.length - 1];
                    String zipFileB = fB.zipFiles[fB.zipFiles.length - 1];
                    String filenameA = zipFileA.substring(1 + zipFileA.lastIndexOf("/"));
                    String filenameB = zipFileB.substring(1 + zipFileB.lastIndexOf("/"));
                    return filenameA.substring(0, filenameA.lastIndexOf(".")).compareToIgnoreCase(filenameB.substring(0, filenameB.lastIndexOf(".")));
                }
                return pathname.compareToIgnoreCase(fB.pathname);
            }
            return zipFiles.length - fB.zipFiles.length;
        }

        static class FolderFlatten {
            int depth;
            Folder folder;
            FolderFlatten(int d, Folder f) {
                depth = d;
                folder = f;
            }
        }
        static void printFolder(Folder folder, boolean current, boolean filenameOnly, int depth, String indentStr0)
        {
            int iconType = folder.getIconType();
            String iconStr = Folder.ICON[iconType];
            iconStr = current ? ("<" + iconStr + ">") : iconStr;
            String displayPathname = folder.getDisplayPathname();
            String displayFilename = null;
            if (filenameOnly)
                displayFilename = displayPathname.substring(1 + displayPathname.lastIndexOf("/"));
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < depth; ++i)
                sb.append(indentStr0);
            System.out.println(sb.toString() + iconStr + " " + (filenameOnly ? displayFilename : displayPathname));
        }
        List<FolderFlatten> getFolderFlatten(int depth)
        {
            return getFolderFlatten(depth, null);
        }
        List<FolderFlatten> getFolderFlatten(int depth, String currentIdStr)
        {
            List<FolderFlatten> folderFlattenList = new ArrayList<FolderFlatten>();
            folderFlattenList.add(new FolderFlatten(depth, this));
            boolean current = false;
            if (currentIdStr != null)
                current = idStr.equals(currentIdStr);
            int iconType = getIconType();
            if (iconType != Folder.ICON_OPENED) {
                if (childrenFolder != null && childrenFolder.size() > 0 && currentIdStr != null)
                    isOpened = true;
                else
                    return folderFlattenList;
            }
            // ICON_OPENED means childrenFolder is not null; see getIconType
            List<String> kal = new ArrayList<String>(childrenFolder.keySet());
            Collections.sort(
                kal,
                new Comparator<String>() {
                    @Override
                    public int compare(String idStrA, String idStrB) {
                        return childrenFolder.get(idStrA).compare(childrenFolder.get(idStrB));
                    }
                }
            );
            for (String keyIdStr: kal) {
                List<FolderFlatten> ffl = childrenFolder.get(keyIdStr).getFolderFlatten(depth + 1, current ? null : currentIdStr);
                folderFlattenList.addAll(ffl);
            }
            return folderFlattenList;
        }
    }

    Folder mRootFolder;
    Folder getRootFolder() {return mRootFolder;}
    Folder mCurrentFolder;
    Folder getCurrentFolder() {return mCurrentFolder;}
    void setCurrentFolder(Folder f) {mCurrentFolder = f;}
    List<Folder> mFolderHistory;
    List<Folder> folderHistory() {return mFolderHistory;}
    void reset()
    {
        mRootFolder = null;
        mCurrentFolder = null;
        mFolderHistory = new ArrayList<Folder>();
    }
    void setCurrentFolder(String idStr, Folder folder) {
        if (mRootFolder == null || !mRootFolder.isAlreadySet())
            mRootFolder = folder;
        Folder f = folder;
        if (mCurrentFolder != null) {
            f = mCurrentFolder.setChildFolder(idStr, folder);
        }
        mCurrentFolder = f;
    }
    void addFolderToHistory(Folder f)
    {
        mFolderHistory.add(f);
    }
    void printFolderTree(String currentIdStr) {
        List<Folder.FolderFlatten> folderFlattenList = null;
        if (mRootFolder != null)
            folderFlattenList = mRootFolder.getFolderFlatten(0);
        for (Folder f: mFolderHistory) {
            System.out.println("|" + f.getDisplayPathname() + "|");
        }
        boolean filenameOnly = false;
        for (Folder.FolderFlatten ff: folderFlattenList) {
            Folder.printFolder(ff.folder, ff.folder.idStr.equals(currentIdStr), filenameOnly, ff.depth, "....");
            filenameOnly = true;
        }
    }
    void printFolderTree(String currentIdStr, List<Folder.FolderFlatten> folderFlattenList) {
        for (Folder f: mFolderHistory) {
            System.out.println("|" + f.getDisplayPathname() + "|");
        }
        boolean filenameOnly = false;
        for (Folder.FolderFlatten ff: folderFlattenList) {
            Folder.printFolder(ff.folder, ff.folder.idStr.equals(currentIdStr), filenameOnly, ff.depth, "....");
            filenameOnly = true;
        }
    }
}
