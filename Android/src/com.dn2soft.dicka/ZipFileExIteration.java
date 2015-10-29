package com.dn2soft.dicka;
import com.dn2soft.util.Jsonable;
import com.dn2soft.util.Json;
import com.dn2soft.util.JsonWalk;

import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.zip.ZipException;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.FileNotFoundException;

class ZipFileExIteration {
    public static class FileElement implements Jsonable
    {
        public String[] zipFiles;
        public String pathname;     // unless zipFiles is null, it is entryName
        public String tmpPathname;  // meaningfull only unless zipFiles is null;
                                    // null means there is an error in creating it

        // public, empty constructor should be set for Jsonable to work
        public FileElement() {
            pathname = null;
            zipFiles = null;
            tmpPathname = null;
        }

        public FileElement(String name) {
            pathname = name;
            zipFiles = null;
            tmpPathname = null;
        }
        public FileElement(String name, String[] ze, String tmpName) {
            pathname = name;
            zipFiles = ze;
            tmpPathname = tmpName;
        }

        String msg()
        {
            if (zipFiles == null)
                return pathname + "->";
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < zipFiles.length; ++i) {
                if (i > 0)
                    sb.append(':');
                sb.append(zipFiles[i]);
            }
            sb.append(':');
            sb.append(pathname);
            sb.append("->");
            sb.append(tmpPathname == null ? "NULL" : tmpPathname);
            return sb.toString();
        }
        public static FileElement setFileElement(String fileElementJsonStr)
        {
            if (fileElementJsonStr == null)
                return null;
            FileElement   fileElement = null;
            try {
                fileElement = (FileElement) JsonWalk.setWalkClassJson(
                    FileElement.class,
                    Json.setJson(fileElementJsonStr)
                );
            }
            catch (ClassNotFoundException e) {e.printStackTrace();}
            catch (InstantiationException e) {e.printStackTrace();}
            catch (IllegalAccessException e) {e.printStackTrace();}
            return fileElement;
        }
        public static String setJsonStr(FileElement fileElement)
        {
            if (fileElement == null)
                return null;
            String jsonStr = null;
            try {
                jsonStr = JsonWalk.walkClass(FileElement.class, fileElement);
            }
            catch (ClassNotFoundException e) {e.printStackTrace();}
            catch (IllegalAccessException e) {e.printStackTrace();}
            return jsonStr;
        }
    }

    public static FileElement makeFileElement(boolean copy, ZipFileExContainer container, String[] zipFiles, String pathname, String indentStr)
    {
        if (zipFiles == null) {
            return new FileElement(pathname);
        }

        String copiedStr = null;
        if (copy) {
            try {
                copiedStr = container.copy(zipFiles, pathname, null);
            } catch (IOException e) {   // including ZipException, FileNotFoundException
                e.printStackTrace();
            }
            if (indentStr != null) {
                System.out.print(indentStr + "|" + pathname + "| -> ");
                System.out.println(copiedStr == null ? "failed" : ("|" + copiedStr + "|"));
            }
        }
        return new FileElement(pathname, zipFiles, copiedStr);
    }

    /**
     * StackElement
     *
     *  three types
     *  type 1. files in a directory
     *  type 2. entries in a zip file
     *  type 3. entries in a directory in a zip file
     *
     * In type 2., keep zipFiles for next entries
     * In type 3., have index to type 2.
     */
    static class StackElement
    {
        Integer depth;              // optional for debugging
        String[] dirEntries;
                                    //  type 1      type 2      type 3
        String[] zipFiles;          //  null        non-null    null
        Integer indexZipFiles;      //  null        null        non-null
        int indexDirEntries;    // initialized to -1 except for type 2 (irrelevant)
                                // if it equals to the size of dirEntries,
                                // the current StackElement will be discarded
    }

    interface ZipFileExAssetFilter {
        InputStream getInputStream(String pathname) throws IOException;
    }
    interface ZipFileExFileFilter extends ZipFileExAssetFilter {
        boolean isDirectory(String pathname);
        boolean isFile(String pathname);
        boolean isValidFilename(String filename);
        String[] listFiles(String pathname);    // if pathname is a directory
        String[] sort(List<String> dal, List<String> fal);
    }
    static class ZipFileExFileSystemFilter implements ZipFileExFileFilter {
        @Override
        public InputStream getInputStream(String pathname)
        {
            return null;    // not from asset
        }

        @Override
        public boolean isDirectory(String pathname)
        {
            return new File(pathname).isDirectory();
        }

        @Override
        public boolean isFile(String pathname)
        {
            return new File(pathname).isFile();
        }

        @Override
        public boolean isValidFilename(String filename)
        {
            return true;
        }

        @Override
        public String[] listFiles(String pathname)
        {
            String[] filenames = new File(pathname).list();
            String[] pathnames = new String[filenames.length];
            for (int i = 0; i < filenames.length; ++i) {
                pathnames[i] = ZipFileEx.removeTrailingSlash(pathname) + '/' + filenames[i];
            }
            return pathnames;
        }

        @Override
        public String[] sort(List<String> dal, List<String> fal)
        {
            List<String> al = new ArrayList<String>(dal.size() + fal.size());
            al.addAll(dal);
            al.addAll(dal.size(), fal);
            return al.toArray(new String[al.size()]);
        }
    }

    ZipFileExFileFilter mFileFilter;
    ZipFileExFileFilter getFileFilter() {return mFileFilter;}
    void setFileFilter(ZipFileExFileFilter fileFilter) {mFileFilter = fileFilter;}
    ZipFileExContainer mContainer;
    List<StackElement> mStack;
    private void init(ZipFileExContainer container, ZipFileExFileFilter fileFilter) {
        mContainer = container;
        mFileFilter = fileFilter;
        mStack = new ArrayList<StackElement>();
    }
    public ZipFileExIteration(ZipFileExContainer container, ZipFileExFileFilter fileFilter) {
        init(container, fileFilter);
    }
    public void clear()
    {
        if (mStack != null)
            mStack.clear();
    }

    /**
     * @throws FileNotFoundException
     *      to warn given file in the selected list does not exist; but in an error in zip file is ignored even in the case of given zip file
     */
    public void execStackElement(boolean recursive, List<FileElement> fileElementList) throws FileNotFoundException
    {
        while (mStack.size() > 0) {
            StackElement se = mStack.remove(mStack.size() - 1);
            if (se.zipFiles != null) {  // type 2
                // just discard it from stack
                continue;
            }
            ++se.indexDirEntries;
            if (se.indexDirEntries == se.dirEntries.length) {
                if (se.indexZipFiles == null) { // type 1
                } else {    // type 3
                }
                // just discard it from stack
                continue;
            }
            mStack.add(se); // push back but with increased indexDirEntries

            Integer nextDepth = null;
            String indentStr = null;
            if (se.depth != null) {
                int d = se.depth.intValue();
                if (d < 0)
                    d = 0;
                nextDepth = new Integer(d + 1);
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < 2 * d; ++i)
                    sb.append(' ');
                indentStr = sb.toString();
            }

            String entryName = se.dirEntries[se.indexDirEntries];
            // determine type of entryName
            boolean isDir = false;
            if (se.indexZipFiles == null) { // type 1
                isDir = mFileFilter.isDirectory(entryName);
            } else {    // type 3
                isDir = entryName.charAt(entryName.length() - 1) == '/';
            }
            if (isDir) {
                if (indentStr != null)
                    System.out.println(indentStr + "|" + entryName + "|: D");
                if (!recursive)
                    continue;
                String[] entries = null;
                if (se.indexZipFiles == null) { // type 1
                    entries = mFileFilter.listFiles(entryName);
                } else {
                    StackElement zipStackElement = mStack.get(se.indexZipFiles);
                    entries = ZipFileEx.filterByDirectory(zipStackElement.dirEntries, entryName);   // get entries for "root" directory
                }
                if (entries != null) {
                    addDirStackElement(entries, se.indexZipFiles, nextDepth);
                }
                continue;
            }
            // !isDir
            int len = entryName.length();
            int start = len - 1;
            if (entryName.charAt(start) == '/') {
                --len;
                --start;
            }
            String filename = entryName.substring(1 + entryName.lastIndexOf("/", start), len);
            if (se.indexZipFiles == null) { // type 1
                if (!mFileFilter.isFile(entryName))
                    throw new FileNotFoundException("|" + entryName + "|: not found");
            }
            int indexDot = filename.lastIndexOf(".");
            if (indexDot > 0 && filename.substring(1 + indexDot).equalsIgnoreCase("zip")) {
                if (indentStr != null)
                    System.out.println(indentStr + "|" + entryName + "|: Z");
                if (!recursive)
                    continue;
                String[] zipFiles1 = null;
                if (se.indexZipFiles == null) { // type 1
                    zipFiles1 = new String[] {entryName};
                } else {    // type 3
                    StackElement zipStackElement = mStack.get(se.indexZipFiles);
                    zipFiles1 = Arrays.copyOf(zipStackElement.zipFiles, zipStackElement.zipFiles.length + 1);
                    zipFiles1[zipStackElement.zipFiles.length] = entryName;
                }
                try {
                    InputStream is = null;  // type 3
                    if (se.indexZipFiles == null) { // type 1
                        is = mFileFilter.getInputStream(entryName);
                    }
                    addZipStackElement(
                        is,
                        zipFiles1,
                        null,   // not from a zip file
                        nextDepth
                    );
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {
                if (indentStr != null)
                    System.out.println(indentStr + "|" + entryName + "|: F");
                if (fileElementList != null) {
                    String[] zipFiles = null;
                    if (se.indexZipFiles == null) { // type 1
                    } else {    // type 3
                        zipFiles = mStack.get(se.indexZipFiles).zipFiles;
                    }
                    if (fileElementList != null) {
                        FileElement fe = ZipFileExIteration.makeFileElement(false, mContainer, zipFiles, entryName, indentStr);
                        fileElementList.add(fe);
                    }
                }
            }
        }
    }

    public void addDirStackElement(String[] entries, Integer indexZipFiles, Integer depth)
    {
        StackElement dirStackElement = makeDirStackElement(entries, indexZipFiles, depth);
        if (dirStackElement != null)
            mStack.add(dirStackElement);
    }
    private StackElement makeDirStackElement(String[] entries, Integer indexZipFiles, Integer depth)
    {
        if (entries == null)
            return null;

        StackElement dirStackElement = new StackElement();
        dirStackElement.depth = depth;
        dirStackElement.dirEntries = entries;
        dirStackElement.zipFiles = null;
        dirStackElement.indexZipFiles = indexZipFiles;
        dirStackElement.indexDirEntries = -1;   // before 0 (start)
        return dirStackElement;
    }

    /**
     * @throws FileNotFoundException
     *      to warn given file in the selected list does not exist; but in an error in zip file is ignored even in the case of given zip file
     */
    public void addZipStackElement(InputStream is, String[] zipFiles, String[] selected, Integer depth) throws FileNotFoundException
    {
        StackElement zipStackElement = new StackElement();
        zipStackElement.depth = depth;
        try {
            zipStackElement.dirEntries = mContainer.list(is, zipFiles);
        } catch (IOException e) {   // including ZipException, FileNotFoundException
            e.printStackTrace();
        }
        if (zipStackElement.dirEntries == null) {
            return;
        }
        if (selected != null) {
            for (int i = 0; i < selected.length; ++i) {
                boolean found = false;
                for (String s: zipStackElement.dirEntries) {
                    if (s.equals(selected[i])) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    throw new FileNotFoundException("|" + selected[i] + "|: not found");
            }
        }
        zipStackElement.zipFiles = Arrays.copyOf(zipFiles, zipFiles.length);
        zipStackElement.indexZipFiles = null;
        zipStackElement.indexDirEntries = -1;

        String[] entries = selected == null ? ZipFileEx.filterByDirectory(zipStackElement.dirEntries, "") : selected;
        StackElement dirStackElement = makeDirStackElement(entries, mStack.size(), depth);
        if (dirStackElement == null)
            return;
        mStack.add(zipStackElement);
        mStack.add(dirStackElement);
    }


    public static void main(String[] args)
    {
        ZipFileExContainer zipFileExContainer = new ZipFileExContainer();
        boolean trace = true;
        try {
            // file system example
            {
                boolean recursive = true;
                ZipFileExIteration iteration = new ZipFileExIteration(zipFileExContainer, new ZipFileExFileSystemFilter());
                try {
                    String[] selected = new String[] {
                        "/home/icentrik/work/android/ogg0/"
                    };
                    iteration.addDirStackElement(
                        selected,
                        null,   // not from zip file
                        trace ? 0 : null
                    );
                    List<FileElement> fileElementList = new ArrayList<FileElement>();
                    iteration.execStackElement(recursive, fileElementList);
                    for (ZipFileExIteration.FileElement fe: fileElementList) {
                        System.out.println("    " + fe.msg());
                        System.out.println("        |" + fe.tmpPathname + "|");
                        FileElement fe1 = ZipFileExIteration.makeFileElement(
                            false,
                            zipFileExContainer,
                            fe.zipFiles,
                            fe.pathname,
                            null    // no trace
                        );
                        System.out.println("        |" + fe1.tmpPathname + "|");
                    }
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } finally {
                    if (iteration != null)
                        iteration.clear();
                }
            }
            // zip file example
            {
                boolean recursive = true;
                ZipFileExFileFilter fileFilter = new ZipFileExFileSystemFilter();
                ZipFileExIteration iteration = new ZipFileExIteration(zipFileExContainer, fileFilter);
                try {
                    String[] zipFiles = new String[] {
                        "/home/icentrik/work/android/ogg0/a.zip"
                    };
                    String[] selected = new String[] {
                        "ogg/",
                        "tmp/",
                        "a",
                        "a.zip"
                    };
                    iteration.addZipStackElement(fileFilter.getInputStream(zipFiles[0]), zipFiles, selected, trace ? 0 : null);
                    List<FileElement> fileElementList = new ArrayList<FileElement>();
                    iteration.execStackElement(recursive, fileElementList);
                    for (ZipFileExIteration.FileElement fe: fileElementList) {
                        System.out.println("    " + fe.msg());
                        System.out.println("        |" + fe.tmpPathname + "|");
                        FileElement fe1 = ZipFileExIteration.makeFileElement(false, zipFileExContainer, fe.zipFiles, fe.pathname, null);
                        System.out.println("        |" + fe1.tmpPathname + "|");
                    }
                } catch (FileNotFoundException e) { // ZipFileExIteration.addZipStackElement
                    e.printStackTrace();
                } catch (IOException e) {   // ZipFileExAssetFilter.getInputStream
                    e.printStackTrace();
                } finally {
                    if (iteration != null)
                        iteration.clear();
                }
            }
        } finally {
            if (zipFileExContainer != null)
                zipFileExContainer.closeAll();
        }
    }
}
