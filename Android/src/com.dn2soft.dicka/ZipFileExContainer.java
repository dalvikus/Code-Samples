package com.dn2soft.dicka;

import java.util.zip.ZipException;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

import java.io.Console;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.File;

public class ZipFileExContainer {
    interface ZipFileExZipFilter {
        boolean accept(String entryName);
    }
    ZipFileExZipFilter mZipFilter;

    private Map<String, ZipFileEx> mMapToZipFileEx;
    private File mCacheDir;
    private void init(File cacheDir, ZipFileExZipFilter zipFilter)
    {
        mMapToZipFileEx = new HashMap<String, ZipFileEx>();
        mCacheDir = cacheDir;
        mZipFilter = zipFilter;
    }
    public ZipFileExContainer() {
        init(null, null);
    }
    public ZipFileExContainer(File cacheDir, ZipFileExZipFilter zipFilter) {
        init(cacheDir, zipFilter);
    }

    public void put(String key, ZipFileEx val)
    {
        if (key == null) {
            throw new NullPointerException("ZipFileEx.put: key is null");
        }
        if (val == null) {
            if (ZipFileEx.TRACE)
                System.out.println("WARN: value is null for key = |" + key + "|");
            return;
        }
        ZipFileEx zipFileEx = mMapToZipFileEx.get(key);
        if (zipFileEx != null) {
            System.out.println("WARN: already " + zipFileEx + " for key = |" + key + "|: delete it first");
            try {
                zipFileEx.cleanup();
            } catch (IOException e) {e.printStackTrace();}
        }
        mMapToZipFileEx.put(key, val);
    }
    public ZipFileEx get(String key)
    {
        return mMapToZipFileEx.get(key);
    }
    public void closeAll()
    {
        for (Map.Entry<String, ZipFileEx> entry: mMapToZipFileEx.entrySet()) {
            if (ZipFileEx.TRACE)
                System.out.println("key: |" + entry.getKey() + "|");
            ZipFileEx zfe = entry.getValue();
            if (zfe != null) {
                try {
                    zfe.cleanup();
                } catch (IOException e) {e.printStackTrace();}
            }
        }
        mMapToZipFileEx.clear();
        mMapToZipFileEx = null;
    }


    public String[] list(String[] zipFiles) throws ZipException, FileNotFoundException, IOException
    {
        return list(null, zipFiles);
    }
    public String[] list(InputStream is, String[] zipFiles) throws ZipException, FileNotFoundException, IOException
    {
        if (zipFiles == null || zipFiles.length == 0)
            return null;

        String[] entries = null;
        final String key = zipFiles[0];
        ZipFileEx zfe = null;
        boolean isNew = false;
        try {
            zfe = get(key);
            if (zfe == null) {
                zfe = is == null ? new ZipFileEx(new File(key), mCacheDir) : ZipFileEx.getZipFileExFromInputStream(is, mCacheDir, key);
                isNew = true;
            }
            entries = zfe.list(zipFiles.length > 1 ? Arrays.copyOfRange(zipFiles, 1, zipFiles.length) : null);
        } catch (IOException e) {
            throw new IOException("Exception: |" + key + "|", e);
        }
        finally {
            if (isNew)
                put(key, zfe);
        }
        if (entries != null && mZipFilter != null) {
            List<String> entriesFiltered = new ArrayList<String>();
            for (String entryName: entries) {
                if (mZipFilter.accept(entryName))
                    entriesFiltered.add(entryName);
            }
            entries = null;
            return entriesFiltered.toArray(new String[entriesFiltered.size()]);
        }
        return entries;
    }

    public String copy(String[] zipFiles, String fromPathname, String toPathname) throws ZipException, FileNotFoundException, IOException
    {
        return copy(null, zipFiles, fromPathname, toPathname);
    }
    public String copy(InputStream is, String[] zipFiles, String fromPathname, String toPathname) throws ZipException, FileNotFoundException, IOException
    {
        if (zipFiles == null || zipFiles.length == 0 || fromPathname == null)
            return null;
        String copiedStr = null;
        final String key = zipFiles[0];
        ZipFileEx zfe = null;
        boolean isNew = false;
        try {
            zfe = get(key);
            if (zfe == null) {
                zfe = is == null ? new ZipFileEx(new File(key), mCacheDir) : ZipFileEx.getZipFileExFromInputStream(is, mCacheDir, key);
                isNew = true;
            }
            copiedStr = zfe.copy(zipFiles.length > 1 ? Arrays.copyOfRange(zipFiles, 1, zipFiles.length) : null, fromPathname, toPathname);
        }
        finally {
            if (isNew)
                put(key, zfe);
        }

        return copiedStr;
    }


/*
////////////////////////////////////////////////////////////////////////////////
// <recursion>
    static class ZipFileExInspector
    {
        public class ZipFileExInspectorException extends Exception {
            ZipFileExInspectorException(String message) {
                super(message);
            }
        }
        interface Handler {
            void handle(ZipFileExContainer container, String[] zipFiles, String pathname, String indentStr) throws ZipFileExInspectorException;
        }

        Handler mHandler;
        Handler getHandler() {return mHandler;}
        ZipFileExInspector() {mHandler = null;}
        void setHandler(Handler handler) {mHandler = handler;}
    }
    static class ZipFileExSimpleInspector extends ZipFileExInspector implements ZipFileExInspector.Handler {
        int mIndex;
        int mSize;
        int getSize() {return mSize;}
        public ZipFileExSimpleInspector(int index) {
            mIndex = index;
            mSize = 0;
            setHandler(this);
        }
        public ZipFileExSimpleInspector() {
            mIndex = -1;
            mSize = 0;
            setHandler(this);
        }

        @Override
        public void handle(ZipFileExContainer container, String[] zipFiles, String pathname, String indentStr) throws ZipFileExInspectorException
        {
            System.out.println("hello, world: pathname: |" + pathname + "|");
            if (mSize == mIndex) {
                System.out.println("handler: raised exception");
                throw new ZipFileExInspectorException(pathname);
            }
            ++mSize;
        }
    }
    static class ZipFileExGetInspector extends ZipFileExInspector implements ZipFileExInspector.Handler {
        int mIndex0;
        int mIndex;
        public ZipFileExGetInspector(int index) {
            mIndex0 = index;
            mIndex = 0;
            setHandler(this);
        }

        @Override
        public void handle(ZipFileExContainer container, String[] zipFiles, String pathname, String indentStr) throws ZipFileExInspectorException
        {
            if (mIndex == mIndex0) {
                String copiedStr = container.copy(zipFiles, pathname, null);
                if (indentStr != null) {
                    System.out.print(indentStr + "|" + pathname + "| -> ");
                    System.out.println(copiedStr == null ? "failed" : ("|" + copiedStr + "|"));
                }
                //throw new ZipFileExInspectorException(pathname);
                String msg = "|" + pathname + "| -> " + (copiedStr == null ? "NULL" : ("|" + copiedStr + "|"));
                throw new ZipFileExInspectorException(msg);
            }
            ++mIndex;
        }
    }
    static class ZipFileExSizeInspector extends ZipFileExInspector implements ZipFileExInspector.Handler {
        int mSize;
        int getSize() {return mSize;}
        public ZipFileExSizeInspector() {
            mSize = 0;
            setHandler(this);
        }

        @Override
        public void handle(ZipFileExContainer container, String[] zipFiles, String pathname, String indentStr) throws ZipFileExInspectorException
        {
            ++mSize;
        }
    }
    ZipFileExInspector mInspector;
    ZipFileExInspector getInspector() {return mInspector;}
    void setInspector(ZipFileExInspector inspector) {mInspector = inspector;}
    private boolean mTrace;
    void setTrace(boolean trace) {mTrace = trace;}

    private void listZipRecur(List<String> entries, String[] zipFiles, String dir, Integer depth) throws ZipFileExInspector.ZipFileExInspectorException
    {
        if (entries == null || entries.size() == 0)
            return;

        Integer nextDepth = null;
        String indentStr = null;
        if (depth != null) {
            int d = depth.intValue();
            if (d < 0)
                d = 0;
            nextDepth = new Integer(d + 1);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < 2 * d; ++i)
                sb.append(' ');
            indentStr = sb.toString();
        }
        if (indentStr != null)
            System.out.println(indentStr + "DIR: |" + dir + "|");
        List<String> sal = ZipFileEx.filterByDirectory(entries, dir);
        if (sal == null)
            return;
        for (String pathname: sal) {
            if (pathname == null || pathname.isEmpty())
                throw new NullPointerException(pathname == null ? "pathname is null" : "pathname is empty");
            int len = pathname.length();
            int start = len - 1;
            boolean isDir = pathname.charAt(start) == '/';
            if (isDir) {
                --len;
                --start;
            }
            String filename = pathname.substring(1 + pathname.lastIndexOf("/", start), len);
            if (isDir) {
                if (indentStr != null)
                    System.out.println(indentStr + "|" + filename + "|: D");
                listZipRecur(entries, zipFiles, pathname, nextDepth);
            } else {
                int indexDot = filename.lastIndexOf(".");
                if (indexDot > 0 && filename.substring(1 + indexDot).equalsIgnoreCase("zip")) {
                    if (indentStr != null)
                        System.out.println(indentStr + "|" + filename + "|: Z");
                    String[] zipFiles1 = Arrays.copyOf(zipFiles, zipFiles.length + 1);
                    zipFiles1[zipFiles.length] = pathname;
                    listZip(zipFiles1, nextDepth);
                } else {
                    if (indentStr != null)
                        System.out.println(indentStr + "|" + filename + "|: F");
                    if (mInspector != null && mInspector.getHandler() != null) {
                        ZipFileExInspector.Handler handler = mInspector.getHandler();
                        handler.handle(this, zipFiles, pathname, indentStr);
                    }
                }
            }
        }
    }

    public void listZip(String[] zipFiles, Integer depth) throws ZipFileExInspector.ZipFileExInspectorException
    {
        listZip(null, zipFiles, depth);
    }
    public void listZip(InputStream is, String[] zipFiles, Integer depth) throws ZipFileExInspector.ZipFileExInspectorException
    {
        if (zipFiles == null || zipFiles.length == 0)
            return;

        List<String> entries = list(is, zipFiles);
        if (entries == null)
            return;

        Integer nextDepth = null;
        if (depth != null) {
            int d = depth.intValue();
            if (d < 0)
                d = 0;
            nextDepth = new Integer(d + 1);
        }
        listZipRecur(entries, zipFiles, "", nextDepth);
    }

    public void ls_R_ZIP(
        String[] zipFiles0,
        String[] selected
    ) throws ZipFileExInspector.ZipFileExInspectorException
    {
        ls_R_ZIP(zipFiles0, selected, true);
    }
    public void ls_R_ZIP(
        String[] zipFiles0,
        String[] selected,
        boolean recursive
    ) throws ZipFileExInspector.ZipFileExInspectorException
    {
        if (zipFiles0 == null || zipFiles0.length == 0 || selected == null)
            return;

        List<String> entries = list(zipFiles0);
        if (entries == null)
            throw new NullPointerException("entries is null");

        for (String pathname: selected) {
            if (pathname == null || pathname.isEmpty())
                throw new NullPointerException(pathname == null ? "pathname is null" : "pathname is empty");
            int len = pathname.length();
            int start = len - 1;
            boolean isDir = pathname.charAt(start) == '/';
            if (isDir) {
                --len;
                --start;
            }
            String filename = pathname.substring(1 + pathname.lastIndexOf("/", start), len);
            if (isDir) {
                if (mTrace)
                    System.out.println("|" + filename + "|: D");
                if (recursive)
                    listZipRecur(entries, zipFiles0, pathname, mTrace ? 0 : null);
            } else {
                int indexDot = filename.lastIndexOf(".");
                if (indexDot > 0 && filename.substring(1 + indexDot).equalsIgnoreCase("zip")) {
                    if (mTrace)
                        System.out.println("|" + filename + "|: Z");
                    if (recursive)  {
                        String[] zipFiles = Arrays.copyOf(zipFiles0, zipFiles0.length + 1);
                        zipFiles[zipFiles0.length] = pathname;
                        listZip(zipFiles, mTrace ? 0 : null);
                    }
                } else {
                    if (mTrace)
                        System.out.println("|" + filename + "|: F");
                    if (mInspector != null && mInspector.getHandler() != null) {
                        ZipFileExInspector.Handler handler = mInspector.getHandler();
                        handler.handle(this, zipFiles0, pathname, mTrace ? "" : null);
                    }
                }
            }
        }
    }

    public void ls_R_FS(String[] selected) throws FileNotFoundException, ZipFileExInspector.ZipFileExInspectorException
    {
        if (selected == null || mFileFilter == null)
            return;

        listFSRecur(selected, true, mTrace ? 0 : null);
    }
    public void ls_R_FS(String[] selected, boolean recursive) throws FileNotFoundException, ZipFileExInspector.ZipFileExInspectorException
    {
        if (selected == null || mFileFilter == null)
            return;

        listFSRecur(selected, recursive, mTrace ? 0 : null);
    }
    private void listFSRecur(String[] sa0, boolean recursive, Integer depth) throws FileNotFoundException, ZipFileExInspector.ZipFileExInspectorException
    {
        if (sa0 == null || mFileFilter == null)
            return;

        Integer nextDepth = null;
        String indentStr = null;
        if (depth != null) {
            int d = depth.intValue();
            if (d < 0)
                d = 0;
            nextDepth = new Integer(d + 1);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < 2 * d; ++i)
                sb.append(' ');
            indentStr = sb.toString();
        }

        for (String pathname: sa0) {
            String filename = pathname.substring(1 + pathname.lastIndexOf("/"));
            if (mFileFilter.isDirectory(pathname)) {
                if (indentStr != null)
                    System.out.println(indentStr + "|" + filename + "|: D");
                if (recursive)
                    listFSRecur(mFileFilter.listFiles(pathname), true, nextDepth);
            } else {
                if (!mFileFilter.isFile(pathname)) {
                    throw new FileNotFoundException("|" + pathname + "|: not found");
                }
                int indexDot = filename.lastIndexOf(".");
                if (indexDot > 0 && filename.substring(1 + indexDot).equalsIgnoreCase("zip")) {
                    if (indentStr != null)
                        System.out.println(indentStr + "|" + filename + "|: Z");
                    try {
                        if (recursive)
                            listZip(mFileFilter.getInputStream(pathname), new String[] {pathname}, nextDepth);
                    } catch (IOException e) {e.printStackTrace();}
                } else {
                    if (indentStr != null)
                        System.out.println(indentStr + "|" + filename + "|: F");
                    if (mInspector != null && mInspector.getHandler() != null) {
                        mInspector.getHandler().handle(this, null, pathname, indentStr);
                    }
                }
            }
        }
    }
// </recursion>
////////////////////////////////////////////////////////////////////////////////
 */

////////////////////////////////////////////////////////////////////////////////
// <example>
    private static void usage()
    {
        System.out.println("extract: -e zipFile0 [zipFiles] inFilename outFilename");
        System.out.println("ls -R: -R zipFile0 [zipFiles]");
        System.out.println("ls dir: -L zipFile0 [zipFiles] dir");
    }
    private static void exec(ZipFileExContainer zipFileExContainer, String[] args)
    {
        if (zipFileExContainer == null)
            return;

        if (args.length < 1) {
            ZipFileExContainer.usage();
            return;
        }
        boolean isExtract = args[0].equals("-e");
        boolean isListAll = args[0].equals("-R");
        boolean isListDir = args[0].equals("-L");
        if (!isListAll && !isListDir && !isExtract) {
            ZipFileExContainer.usage();
            return;
        }

        if (isListAll || isListDir) {
            if ((isListAll && args.length < 2) || (isListDir && args.length < 3)) {
                ZipFileExContainer.usage();
                return;
            }

            String[] zipFiles = Arrays.copyOfRange(args, 1, isListAll ? args.length : (args.length - 1));

            String[] sal0 = null;
            try {
                sal0 = zipFileExContainer.list(zipFiles);
            } catch (ZipException e) {
                e.printStackTrace();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (sal0 == null)
                System.out.println("ls: N/A");
            else {
                System.out.println("ls: # of files = " + sal0.length);
                for (String s: sal0)
                    System.out.println("    |" + s + "|");
            }

            if (sal0 != null && isListDir) {
                String dir = args[args.length - 1];
                String[] sal = ZipFileEx.filterByDirectory(sal0, dir);
                System.out.println("ls: directory: |" + (dir.isEmpty() ? "./" : (dir.endsWith("/") ? dir : (dir + "/"))) + "|: " + (sal == null ? "N/A" : ("# of files = " + sal.length)));
                for (String s: sal)
                    System.out.println("    |" + s + "|");
            }
            return;
        }
        if (isExtract) {
            if (args.length < 4) {
                ZipFileExContainer.usage();
                return;
            }
            String[] zipFiles = Arrays.copyOfRange(args, 1, args.length - 2);
            String copiedStr = null;
            try {
                copiedStr = zipFileExContainer.copy(zipFiles, args[args.length - 2], args[args.length - 1]);
            } catch (ZipException e) {
                e.printStackTrace();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("copied? " + (copiedStr == null ? "false" : (copiedStr.isEmpty() ? "partial" : "done")));
            return;
        }
    }
    public static void main(String[] args)
    {
        ZipFileExContainer zipFileExContainer = new ZipFileExContainer();

        try {
            Console c = System.console();
            do {
                String cmd = c.readLine("> ");
                if (cmd == null) {  // ^D
                    System.out.println("");
                    break;
                }
                cmd = cmd.trim();
                if (cmd.isEmpty())
                    continue;
                ZipFileExContainer.exec(zipFileExContainer, cmd.split("\\s+"));
            } while (true);
        } finally {
            if (zipFileExContainer != null)
                zipFileExContainer.closeAll();
        }
    }
// </example>
////////////////////////////////////////////////////////////////////////////////
}
