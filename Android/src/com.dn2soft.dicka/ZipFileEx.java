package com.dn2soft.dicka;

import java.util.Comparator;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Map;
import java.util.HashMap;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;

import java.util.zip.ZipFile;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;

import java.io.Console;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.File;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;

/**
 *
 * new ZipFileEx(ZipFile)
 * new ZipFile(new File(zipFile))
 * list
 * extract
 */
public class ZipFileEx extends ZipFile {
    private static final String TAG = "ZipFileEx";
    public static final boolean TRACE = false;

    /**
     * used in createTempFile; all ZipFileEx (node) it is the same.
     *
     * <p>If it is null, system default directory (for example, "/tmp/") is used; in Java itself null is enough but in Android App, it does not work, so it is mandatory in the Android App.
     *
     * <p> It contains just a path to a cache directory.
     */
    private File mCacheDir;

////private ZipFile mZipFile;
    /**
     * unless it is null, it should be deleted at the end of use.
     */
    private String mTmpZipPathname;
    public String getTmpZipPathname() {return mTmpZipPathname;}

    /**
     * for debug, it will show how the zip file is contructed
     */
    private String mZipPath;

    /**
     * children nodes for another zip file
     */
    private Map<String, ZipFileEx> mChildrenZipFileEx;

    /**
     * tmp files for non-zip file
     */
    private Map<String, String> mChildrenTmpFile;


    public ZipFileEx(File file) throws ZipException, IOException {
        super(file);
        mCacheDir = null;
////    mZipFile = new ZipFile(file);
        mChildrenZipFileEx = null;
        mChildrenTmpFile = null;
        mTmpZipPathname = null;
        mZipPath = file.getAbsolutePath();
    }

    public ZipFileEx(File file, File cacheDir) throws ZipException, IOException {
        super(file);
        mCacheDir = cacheDir;
////    mZipFile = new ZipFile(file);
        mChildrenZipFileEx = null;
        mChildrenTmpFile = null;
        mTmpZipPathname = null;
        mZipPath = file.getAbsolutePath();
    }

    /**
     * used in making a singly-linked-list
     */
    private ZipFileEx(File tmpFile, String zipPath, File cacheDir) throws ZipException, IOException {
        super(tmpFile);
////    mZipFile = new ZipFile(file);
        mCacheDir = cacheDir;
        mChildrenZipFileEx = null;
        mChildrenTmpFile = null;
        mTmpZipPathname = tmpFile.getAbsolutePath();
        mZipPath = zipPath;
    }

    private static String getZipPath(String zipPath)
    {
        if (!TRACE)
            return null;
        int i = zipPath.indexOf(":");
        String rootPath0 = i == -1 ? zipPath : zipPath.substring(0, i);
        String rootPath = ".../" + rootPath0.substring(1 + rootPath0.lastIndexOf("/"));
        return i == -1 ? rootPath : (rootPath + ":" + zipPath.substring(1 + i));
    }

////////////////////////////////////////////////////////////////////////////////
// <core>
    public void cleanup() throws IOException
    {
        try {
            ZipFileEx.cleanupThisOnly(this);
        } finally {
            ZipFileEx.cleanupAll(this);
        }
    }

    /**
     * 1. call ZipFile.close()
     * 2. delete temporary file
     */
    private void cleanupThisOnly() throws IOException
    {
        ZipFileEx.cleanupThisOnly(this);
    }
    private static void cleanupThisOnly(ZipFileEx zipFileEx) throws IOException
    {
        if (zipFileEx == null)
            return;
        if (ZipFileEx.TRACE)
            System.out.println("" + zipFileEx + ": ZipFile: closed");
        if (zipFileEx.mTmpZipPathname != null) {
            if (ZipFileEx.TRACE)
                System.out.println("" + zipFileEx + ": delete tmp zip file: |" + zipFileEx.mTmpZipPathname + "|");
            File file = new File(zipFileEx.mTmpZipPathname);
            boolean deleted = file.delete();
            if (ZipFileEx.TRACE)
                System.out.println("" + zipFileEx + ": tmp zip file deleted? " + deleted);
        }
        if (ZipFileEx.TRACE) {
            System.out.println("" + zipFileEx + ": close ZipFile");
            System.out.println("ZipPath=|" + ZipFileEx.getZipPath(zipFileEx.mZipPath) + "|");
        }
////    if (zipFileEx.mZipFile != null)
////        zipFileEx.mZipFile.close();
        ((ZipFile) zipFileEx).close();

        if (zipFileEx.mChildrenTmpFile != null) {
            for (Map.Entry<String, String> entry: zipFileEx.mChildrenTmpFile.entrySet()) {
                String tmpPathname = entry.getValue();
                File f = new File(tmpPathname);
                boolean deleted = f.delete();
                if (TRACE) {
                    System.out.println("tmp file: |" + tmpPathname + "|: deleted? " + deleted);
                }
            }
            zipFileEx.mChildrenTmpFile.clear();
            zipFileEx.mChildrenTmpFile = null;
        }
    }

    /**
     * recursively call to all children
     */
    private static void cleanupAll(ZipFileEx zipFileEx) throws IOException
    {
        if (zipFileEx != null && zipFileEx.mChildrenZipFileEx != null) {
            for (Map.Entry<String, ZipFileEx> entry: zipFileEx.mChildrenZipFileEx.entrySet()) {
                entry.getValue().cleanup();
            }
            zipFileEx.mChildrenZipFileEx.clear();
            zipFileEx.mChildrenZipFileEx = null;
        }
    }

    private static boolean isAlreadyCopied(File tmpFile, ZipEntry zipEntry)
    {
        if (tmpFile == null || zipEntry == null)
            return false;
        return tmpFile.isFile() && tmpFile.length() == zipEntry.getSize();
    }

    /**
     * return ZipFileEx from InputStream
     *
     * <p> create a temporary file in a cache directory.
     *
     */
    public static ZipFileEx getZipFileExFromInputStream(InputStream is, File cacheDir, String zipPath0) throws FileNotFoundException, ZipException, IOException
    {
        String zipPath = null;
        if (TRACE)
            zipPath = ZipFileEx.getZipPath(zipPath0);
        File tmpFile = null;
        ZipFileEx zipFileEx = null;
        Throwable throwable = null;
        try {
            tmpFile = File.createTempFile("ZipFileEx-", ".zip", cacheDir);
            if (ZipFileEx.TRACE)
                System.out.println("tmp zip file: |" + tmpFile.getAbsolutePath() + "| for |" + zipPath + "|: creating...");
            boolean created = ZipFileEx.cp(is, tmpFile);
            if (ZipFileEx.TRACE)
                System.out.println("tmp zip file: |" + tmpFile.getAbsolutePath() + "| for |" + zipPath + "|: ...done: " + (created ? "okay" : "oops"));
            if (created)
                zipFileEx = new ZipFileEx(tmpFile, zipPath0, cacheDir);
        }
        catch (IllegalArgumentException e) {
            // File.createTempFile(String, String, File)
            e.printStackTrace();
        }
        catch (FileNotFoundException e) {
            // ZipFileEx.cp(InputStream, File)
            throwable = e;
        }
        catch (ZipException e) {
            // ZipFileEx(File, String, File)
            throwable = e;
        }
        catch (IOException e) {
            // File.createTempFile(String, String, File)
            // ZipFileEx.cp(InputStream, File)
            // ZipFileEx(File, String, File)
            throwable = e;
        }
        if (zipFileEx == null) {
            if (tmpFile != null) {
                if (ZipFileEx.TRACE)
                    System.out.println("tmp zip file: |" + tmpFile.getAbsolutePath() + "| for |" + zipPath + "|: deleting...");
                boolean deleted = tmpFile.delete();
                if (ZipFileEx.TRACE)
                    System.out.println("tmp zip file: |" + tmpFile.getAbsolutePath() + "| for |" + zipPath + "|: deleted? " + deleted);
            }
            if (throwable != null)
                throw new IOException(
                    "ERR: some error in creating ZipFileEx",
                    throwable
                );
            return null;
        }
        return zipFileEx;
    }

    /**
     * return ZipFileEx
     *
     * @param childZipFilename
     * @param zipEntry
     */
    private ZipFileEx getZipFileEx(String childZipFilename, ZipEntry zipEntry) throws FileNotFoundException, ZipException, IOException
    {
        if (childZipFilename == null)
            return null;

        String zipPath0 = null;
        if (TRACE)
            zipPath0 = mZipPath + ":" + childZipFilename;
        String zipPath = null;
        if (TRACE)
            zipPath = ZipFileEx.getZipPath(zipPath0);

        ZipFileEx childZipFileEx0 = null;
        if (mChildrenZipFileEx != null)
            childZipFileEx0 = mChildrenZipFileEx.get(childZipFilename);

        File tmpFile0 = null;
        if (childZipFileEx0 != null) {
            tmpFile0 = new File(childZipFileEx0.mTmpZipPathname);
            if (ZipFileEx.isAlreadyCopied(tmpFile0, zipEntry)) {
                if (ZipFileEx.TRACE)
                    System.out.println("use tmp zip file |" + childZipFileEx0.mTmpZipPathname + "| for |" + ZipFileEx.getZipPath(childZipFileEx0.mZipPath) + "|");
                return childZipFileEx0;
            }
            // corrupted or...
        }
        // here, if childZipFileEx0 is not null, all children of it should be re-linked or should be removed recursively

        ZipFileEx childZipFileEx = null;
        String exceptionStr = null;
        Throwable throwable = null;
        try {
            childZipFileEx = ZipFileEx.getZipFileExFromInputStream(((ZipFileEx) this).getInputStream(zipEntry), mCacheDir, zipPath0);
        }
        catch (IllegalStateException e) {
            // ZipFile.getInputStream
            e.printStackTrace();
        }
        catch (IndexOutOfBoundsException e) {
            // ZipFile.getInputStream
            e.printStackTrace();
        }
        catch (NullPointerException e) {
            // ZipFile.getInputStream
            e.printStackTrace();
        }
        catch (FileNotFoundException e) {
            // ZipFileEx.getZipFileExFromInputStream(File, File, String)
            exceptionStr = "FileNotFoundException";
            throwable = e;
        }
        catch (ZipException e) {
            // ZipFileEx.getZipFileExFromInputStream(File, File, String)
            exceptionStr = "ZipException";
            throwable = e;
        }
        catch (IOException e) {
            // ZipFileEx.getZipFileExFromInputStream(File, File, String)
            exceptionStr = "IOException";
            throwable = e;
        }
        if (childZipFileEx == null) {
            if (childZipFileEx0 != null && childZipFileEx0.mChildrenZipFileEx != null) {
                if (TRACE) {
                    System.out.println("corrupted ZipFileEx(" + childZipFileEx0 + "): all children of type ZipFileEx should be destroyed");
                }
                childZipFileEx0.cleanup();
            }
            if (throwable != null)
                throw new IOException(exceptionStr + " occurred in creating child ZipFileEx for |" + childZipFilename + "|", throwable);
            return null;
        }

        if (childZipFileEx0 != null) {
            if (TRACE)
                System.err.println("ERR: tmp zip file: |" + tmpFile0.getAbsolutePath() + "| for |" + zipPath + "|: corrupted: re-linking...");
            childZipFileEx0.cleanupThisOnly();
            if (childZipFileEx0.mChildrenZipFileEx != null) {
                childZipFileEx.mChildrenZipFileEx = new HashMap<String, ZipFileEx>(childZipFileEx0.mChildrenZipFileEx.size());
                for (Map.Entry<String, ZipFileEx> entry: childZipFileEx0.mChildrenZipFileEx.entrySet()) {
                    ZipFileEx zipFileEx = entry.getValue();
                    childZipFileEx.mChildrenZipFileEx.put(entry.getKey(), zipFileEx);
                }
                childZipFileEx0.mChildrenZipFileEx.clear();
                childZipFileEx0.mChildrenZipFileEx = null;
            }
            if (childZipFileEx0.mChildrenTmpFile != null) {
                childZipFileEx.mChildrenTmpFile = new HashMap<String, String>(childZipFileEx0.mChildrenTmpFile.size());
                for (Map.Entry<String, String> entry: childZipFileEx0.mChildrenTmpFile.entrySet()) {
                    String tmpPathname = entry.getValue();
                    childZipFileEx.mChildrenTmpFile.put(entry.getKey(), tmpPathname);
                }
                childZipFileEx0.mChildrenTmpFile.clear();
                childZipFileEx0.mChildrenTmpFile = null;
            }
        }
        if (mChildrenZipFileEx == null)
            mChildrenZipFileEx = new HashMap<String, ZipFileEx>();
        mChildrenZipFileEx.put(childZipFilename, childZipFileEx);
        if (ZipFileEx.TRACE)
            System.out.println("tmp zip file: |" + childZipFileEx.mTmpZipPathname + "| for |" + zipPath + "|: saved in " + childZipFileEx);
        return childZipFileEx;
    }

    /**
     * return ZipFileEx in zip file
     *
     * <p>There might be nested zip files in opened zip file,
     * in which case it returns ZipFileEx for the deepest zip file.
     *
     * @param zipFiles  describes hierarchy in opened zip file;
     *                  if zipFiles is none, it returns entries in opened zip file
     * @throws FileNotFoundException if there is no file named in zipFiles
     * @throws ZipException as ZipFile(File)
     * @throws IOException as in ZipFile(File)
     */
    public ZipFileEx getDeepestZipFileEx(String[] zipFiles) throws FileNotFoundException, ZipException, IOException
    {
        if (zipFiles == null)
            return this;

        ZipFileEx nextZipFileEx = this;
        for (int i = 0; i < zipFiles.length; ++i) {
            String childZipFilename = ZipFileEx.removeTrailingSlash(zipFiles[i]);
            if (childZipFilename == null) {
                System.err.println("ERR: zipFiles[" + i + "]: null");
                return null;
            }

            // open zip file named childZipFilename(sanitized zipFile0[i])
            ZipEntry childZipEntry = null;
            try {
                childZipEntry = ((ZipFile) nextZipFileEx).getEntry(childZipFilename);
////            childZipEntry = nextZipFileEx.mZipFile.getEntry(childZipFilename);
            } catch (IllegalStateException e) {
                e.printStackTrace();
                return null;
            }
            if (childZipEntry == null) {
                System.err.println("ERR: zipFiles[" + i + "]: |" + zipFiles[i] + "|: not found");
                return null;
            }
            if (!childZipFilename.substring(1 + childZipFilename.lastIndexOf(".")).equalsIgnoreCase("zip")) {
                System.err.println("ERR: zipFiles[" + i + "]: |" + zipFiles[i] + "|: not a zip file");
                return null;
            }

            nextZipFileEx = nextZipFileEx.getZipFileEx(childZipFilename, childZipEntry);
            if (nextZipFileEx == null)
                return null;
        }
        return nextZipFileEx;
    }
// </core>
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// <list>
    public String[] list(String[] zipFiles) throws FileNotFoundException, ZipException, IOException
    {
        ZipFileEx deepestZipFileEx = getDeepestZipFileEx(zipFiles);
        if (deepestZipFileEx == null)
            return null;
        return ZipFileEx.ls((ZipFile) deepestZipFileEx);
////    return ZipFileEx.ls(deepestZipFileEx.mZipFile);
    }
    public String[] list() throws FileNotFoundException, ZipException, IOException
    {
        return list((String[]) null);
    }

    public static String[] ls(ZipFile zipFile)
    {
        if (zipFile == null)
            return null;
        List<String> ret = null;
        try {
            Enumeration<?> enumer = zipFile.entries();
            ret = new ArrayList<String>();
            while (enumer.hasMoreElements()) {
                ZipEntry zipEntry = (ZipEntry) enumer.nextElement();
                ret.add(zipEntry.getName());
            }
        } catch (IllegalStateException e) {e.printStackTrace();}
        return ret == null ? null : ret.toArray(new String[ret.size()]);
    }
// </list>
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// <copy>
    /**
     * return a "reliable" entry name; unless it is null,
     * it can be used in getting ZipEntry and in testing if it is a directory or not.
     *
     * <p>For example, both "mp3/" and "mp3" give the same entry.
     * On the other hand, ZipEntry.isDirectory() test only if last character is '/' or not,
     * so Zip.isDirectory() is not a reliable test.
     *
     * @return reliableEntryName
     */
    private static String getReliableEntryName(ZipFile zipFile, String entryName)
    {
        boolean hasSlash = entryName.endsWith("/");
        String s0 = ZipFileEx.removeTrailingSlash(entryName);
        if (hasSlash)
            s0 = s0 + "/";
        ZipEntry zipEntry = null;
        try {
            zipEntry = zipFile.getEntry(s0);
        } catch (IllegalStateException e) {e.printStackTrace();}
        if (zipEntry == null && hasSlash) {
            System.err.println("ERR: directory: |" + s0 + "|: not found");
            return null;
        }
        if (zipEntry != null && hasSlash) {     // directory
            return s0;
        }
        // !hasSlash
        // try directory with appending "/"
        String s = s0 + "/";
        ZipEntry zipEntrySlash = null;
        try {
            zipEntrySlash = zipFile.getEntry(s);
        } catch (IllegalStateException e) {e.printStackTrace();}
        if (zipEntry == null && zipEntrySlash == null) {
            System.err.println("ERR: |" + entryName + "(|" + s0 + "|): not found");
            return null;
        }
        return zipEntrySlash == null ? s0 : s;
    }

    /**
     * copy file in the deepest zip file, and return copied file
     *
     * <p> Depending on the types of inFilename (directory or not) and outFilename, a proper stuff will be done.
     *
     * <p> Assuming that list is sorted; directory comes first before any files in the directory.
     *
     * @param outFilename
     *      if outFilename is null, tmp file is created and its name is returned
     */
    public String copy(String[] zipFiles, String inFilename, String outFilename) throws FileNotFoundException, ZipException, IOException
    {
        if (inFilename == null)
            return null;

        ZipFileEx deepestZipFileEx = zipFiles == null ? this : getDeepestZipFileEx(zipFiles);
        if (deepestZipFileEx == null)
            return null;

        String reliableEntryName = ZipFileEx.getReliableEntryName((ZipFile) deepestZipFileEx, inFilename);
////    String reliableEntryName = ZipFileEx.getReliableEntryName(deepestZipFileEx.mZipFile, inFilename);
        if (reliableEntryName == null) {
            return null;
        }
        //System.out.println("reliableEntryName: |" + reliableEntryName + "|");

        ZipEntry zipEntry = ((ZipFile) deepestZipFileEx).getEntry(reliableEntryName);

        if (outFilename == null) {  // use tmp file
            if (zipEntry.isDirectory())
                return null;
            if (deepestZipFileEx.mChildrenTmpFile != null) {
                String tmpPathname = deepestZipFileEx.mChildrenTmpFile.get(inFilename);
                if (tmpPathname != null) {
                    if (TRACE)
                        System.out.println("use saved tmp file: |" + tmpPathname + "|");
                    return tmpPathname;
                }
            }
            File tmpFile = null;
            Throwable throwable = null;
            try {
                tmpFile = File.createTempFile("ZipFileEx-", null, mCacheDir);
            }
            catch (IllegalStateException e) {e.printStackTrace();}
            //catch (IOException e) {throwable = e;}
            catch (IOException e) {throwable = e;}
            if (tmpFile == null) {
                if (tmpFile != null) {
                    if (ZipFileEx.TRACE)
                        System.out.println("tmp file: |" + tmpFile.getAbsolutePath() + "| for |" + inFilename + "|: deleting...");
                    boolean deleted = tmpFile.delete();
                    if (ZipFileEx.TRACE)
                        System.out.println("tmp file: |" + tmpFile.getAbsolutePath() + "| for |" + inFilename + "|: deleted? " + deleted);
                }
                if (throwable != null)
                    throw new IOException(
                        "some error in creating tmp file for |" + inFilename + "|",
                        throwable
                    );
                return null;
            }
            boolean copied = ZipFileEx.cp(((ZipFile) deepestZipFileEx).getInputStream(zipEntry), tmpFile);
            if (!copied) {
                System.err.println("copy |" + reliableEntryName + "|: failed");
                tmpFile.delete();
                return null;
            }
            if (TRACE)
                System.out.println("copy |" + reliableEntryName + "| -> |" + tmpFile.getAbsolutePath() + "|: done");
            if (deepestZipFileEx.mChildrenTmpFile == null)
                deepestZipFileEx.mChildrenTmpFile = new HashMap<String, String>();
            deepestZipFileEx.mChildrenTmpFile.put(inFilename, tmpFile.getAbsolutePath());
            return tmpFile.getAbsolutePath();
        }

        File dirFile = new File(outFilename);
        String dirs = dirFile.getParent();
        if (dirs != null)
            new File(dirs).mkdirs();
        if (!zipEntry.isDirectory())
            return ZipFileEx.cp(
                ((ZipFile) deepestZipFileEx).getInputStream(zipEntry),
////            deepestZipFileEx.mZipFile.getInputStream(zipEntry),
                dirFile.isDirectory() ? new File(dirFile, reliableEntryName.substring(1 + reliableEntryName.lastIndexOf("/"))) : dirFile
            ) ? outFilename : null;
        // zipEntry.isDirectory()
        boolean allCopied = true;
        if (!dirFile.isDirectory()) {
            if (dirFile.isFile()) {
                System.err.println("ERR: inFilename: |" + inFilename + "| is a directory but outFilename: |" + outFilename + "| not a directory");
                return null;
            }
            dirFile.mkdirs();
        }
        int len0 = reliableEntryName.length();
        for (String s0: ZipFileEx.ls((ZipFile) deepestZipFileEx)) {
////    for (String s0: ZipFileEx.list(deepestZipFileEx.mZipFile)) {
            if (!s0.startsWith(reliableEntryName))
                continue;
            String s = s0.substring(len0);
            if (s.isEmpty())
                continue;
            File outFile = new File(dirFile, s);
            if (ZipFileEx.TRACE)
                System.out.println("copy |" + s0 + "|(|" + s + "|) to |" + outFile.getPath() + "|: directory? " + outFile.isDirectory());
            // assuming that list is sorted; directory comes first before any files in the directory
            if (s.charAt(s.length() - 1) == '/') {  // directory
                if (ZipFileEx.TRACE)
                    System.out.println("|" + s + "|: directory, mkdirs(|" + outFile.getPath() + "|");
                outFile.mkdirs();
                continue;
            }
            ZipEntry ze = null;
            boolean copied = false;
            if (ZipFileEx.TRACE)
                System.out.println("|" + s0 + "|(|" + s + "|) -> |" + outFile.getPath() + "|: copying...");
            try {
                ze = ((ZipFile) deepestZipFileEx).getEntry(s0);
////            ze = deepestZipFileEx.mZipFile.getEntry(s0);
                if (ze == null) {
                    System.err.println("ERR: |" + s0 + "|: not found");
                } else {
                    copied = ZipFileEx.cp(((ZipFile) deepestZipFileEx).getInputStream(ze), outFile);
    ////            copied = ZipFileEx.cp(deepestZipFileEx.mZipFile.getInputStream(ze), outFile);
                }
            } catch (IllegalStateException e) {e.printStackTrace();}
            if (ZipFileEx.TRACE)
                System.out.println("...done: " + (copied ? "okay" : "oops"));
            if (!copied)
                allCopied = false;
////    }
        }

        return allCopied ? outFilename : "";
    }
    public static boolean cp(InputStream is, File outFile) throws FileNotFoundException, IOException
    {
        boolean copied = false;
        try {
            BufferedInputStream bis = new BufferedInputStream(is);
            BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(outFile));
            final int BUF_LEN = 8192;
            byte[] buffer = new byte[BUF_LEN];
            int byteRead;
            while ((byteRead = bis.read(buffer, 0, BUF_LEN)) >= 0) {
                bos.write(buffer, 0, byteRead);
            }
            bis.close();
            bos.close();
            copied = true;
        }
        catch (ArrayIndexOutOfBoundsException e) {
            // BufferedOutputStream.write(...)
            e.printStackTrace();
        }
        return copied;
    }
// </copy>
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// <utility>
    /**
     * remove trailing '/' in pathname
     */
    public static final String removeTrailingSlash(String pathname)
    {
        if (pathname == null)
            return null;

        int index = pathname.length() - 1;
        while (index >= 0 && pathname.charAt(index) == '/')
            --index;
        return pathname.substring(0, 1 + index);
    }

    /**
     * Return entries in a specific directory in zip file.
     *
     * @param sal0 all entries in a zip file; got from ZipFile.entries()
     *          no leading '/' contray to ones that come from file system
     *          direnctory ends with '/'
     * @param dir0 a directory to be listed; last '/' is optional
     *          two or more tailing '/' are treated '/'
     *          if it is null, it is a root, i.e., ""
     * @return entries in the directory
     */
    public static final String[] filterByDirectory(final String[] sal0, final String dir0)
    {
        if (sal0 == null)
            return null;

        int len0 = 0;       // if dir0 is null
        String dir = "";    // if dir0 is null
        if (dir0 != null) {
            int index = dir0.length() - 1;
            while (index >= 0 && dir0.charAt(index) == '/')
                --index;
            if (index >= 0) {
                len0 = 1 + index;
                dir = dir0.substring(0, len0) + "/";
            }
        }
        final int offsetForNextSlashSearch = len0 == 0 ? 0 : (len0 + 1);
        List<String> sal = new ArrayList<String>();
        for (String s: sal0) {
            int len = s.length();
            if (len <= offsetForNextSlashSearch) {
                continue;
            }

            if (len0 != 0 && !s.startsWith(dir)) {
                continue;
            }

            // index for next '/'
            int indexSlash = s.indexOf("/", offsetForNextSlashSearch);
            if (indexSlash == -1) {         // file in the directory
                sal.add(s);
            }
            // directory
            if (indexSlash == len - 1) {    // ends with "/"
                sal.add(s);
            }
        }
        Collections.sort(
            sal,
            new Comparator<String>() {
                @Override
                public int compare(String a, String b) {
                    int lenA = a.length();
                    int lenB = b.length();
                    boolean isDirA = a.charAt(lenA - 1) == '/';
                    boolean isDirB = b.charAt(lenB - 1) == '/';
                    if (isDirA && isDirB)
                        return a.compareToIgnoreCase(b);
                    if (isDirA && !isDirB)
                        return -1;
                    if (!isDirA && isDirB)
                        return +1;
                    String filenameA = a.substring(1 + a.lastIndexOf("/"));
                    String filenameB = b.substring(1 + b.lastIndexOf("/"));
                    return filenameA.substring(0, filenameA.lastIndexOf(".")).compareToIgnoreCase(filenameB.substring(0, filenameB.lastIndexOf(".")));
                }
            }
        );
        return sal.toArray(new String[sal.size()]);
    }
// </utility>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// <example>
    private static void usage()
    {
        System.out.println("extract: -e zipFile0 [zipFiles] inFilename outFilename");
        System.out.println("ls -R: -R zipFile0 [zipFiles]");
        System.out.println("ls dir: -L zipFile0 [zipFiles] dir");
    }
    private static void exec(String[] args)
    {
        if (args.length < 1) {
            ZipFileEx.usage();
            return;
        }
        boolean isExtract = args[0].equals("-e");
        boolean isListAll = args[0].equals("-R");
        boolean isListDir = args[0].equals("-L");
        if (!isListAll && !isListDir && !isExtract) {
            ZipFileEx.usage();
            return;
        }

        if (isListAll || isListDir) {
            if ((isListAll && args.length < 2) || (isListDir && args.length < 3)) {
                ZipFileEx.usage();
                return;
            }
            String[] zipFiles = Arrays.copyOfRange(args, 1, isListAll ? args.length : (args.length - 1));

            String[] sal0 = null;
            ZipFileEx zfe = null;
            try {
                zfe = new ZipFileEx(new File(zipFiles[0]));
                sal0 = zfe.list(zipFiles.length > 1 ? Arrays.copyOfRange(zipFiles, 1, zipFiles.length) : null);
            }
            catch (FileNotFoundException e) {e.printStackTrace();}
            catch (ZipException e) {e.printStackTrace();}
            catch (IOException e) {e.printStackTrace();}
            finally {
                if (zfe != null) {
                    try {
                        zfe.cleanup();
                    } catch (IOException e) {e.printStackTrace();}
                }
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
                ZipFileEx.usage();
                return;
            }

            String[] zipFiles = Arrays.copyOfRange(args, 1, args.length - 2);

            String copiedStr = null;
            ZipFileEx zfe = null;
            try {
                zfe = new ZipFileEx(new File(zipFiles[0]));
                copiedStr = zfe.copy(zipFiles.length > 1 ? Arrays.copyOfRange(zipFiles, 1, zipFiles.length) : null, args[args.length - 2], args[args.length - 1]);
            }
            catch (FileNotFoundException e) {e.printStackTrace();}
            catch (ZipException e) {e.printStackTrace();}
            catch (IOException e) {e.printStackTrace();}
            finally {
                if (zfe != null) {
                    try {
                        zfe.cleanup();
                    } catch (IOException e) {e.printStackTrace();}
                }
            }

            System.out.println("copied? " + (copiedStr == null ? "false" : (copiedStr.isEmpty() ? "partial" : "done")));
            return;
        }
    }
    public static void main(String[] args)
    {
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
/*
            System.out.println("|" + cmd + "|");
            for (String s: cmd.split("\\s+")) {
                System.out.println("  |" + s + "|");
            }
 */
            ZipFileEx.exec(cmd.split("\\s+"));
        } while (true);
    }
// </example>
////////////////////////////////////////////////////////////////////////////////
}
