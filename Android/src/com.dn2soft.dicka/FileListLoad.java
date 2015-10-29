package com.dn2soft.dicka;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileFilter;
import com.dn2soft.dicka.ZipFileExIteration.ZipFileExFileSystemFilter;


import java.io.File;
import java.io.FilenameFilter;
import java.io.InputStream;
import java.io.IOException;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Collections;
import java.util.List;


import android.content.res.AssetManager;

import android.util.Log;



public final class FileListLoad {
    private static final String TAG = "FileListLoad";
    private FileListLoad() {}

    static Comparator<String> COMPARATOR_STRING_WITH_COLON_IGNORE_CASE = new Comparator<String>() {
        @Override
        public int compare(String aWithColon, String bWithColon) {
            return aWithColon.substring(0, aWithColon.lastIndexOf(":")).compareToIgnoreCase(bWithColon.substring(0, bWithColon.lastIndexOf(":")));
        }
    };
    static Comparator<String> COMPARATOR_STRING_WITH_COLON_WITHOUT_EXTENSION_IGNORE_CASE = new Comparator<String>() {
        @Override
        public int compare(String aWithColon, String bWithColon) {
Log.i(TAG, "aWithColon: |" + aWithColon + "|");
Log.i(TAG, "bWithColon: |" + bWithColon + "|");
            String pathnameA = aWithColon.substring(0, aWithColon.lastIndexOf(":"));
            String filenameA = pathnameA.substring(1 + pathnameA.lastIndexOf("/"));
            String pathnameB = bWithColon.substring(0, bWithColon.lastIndexOf(":"));
            String filenameB = pathnameB.substring(1 + pathnameB.lastIndexOf("/"));
            return filenameA.substring(0, filenameA.lastIndexOf(".")).compareToIgnoreCase(filenameB.substring(0, filenameB.lastIndexOf(".")));
        }
    };
    static Comparator<String> COMPARATOR_STRING_IGNORE_CASE = new Comparator<String>() {
        @Override
        public int compare(String a, String b) {
            return a.compareToIgnoreCase(b);
        }
    };

    static boolean isValidEntry(String entryName)
    {
        if (entryName.charAt(entryName.length() - 1) == '/')
            return true;
        String filename = entryName.substring(1 + entryName.lastIndexOf("/"));
        return FileListLoad.isValidFilename(filename);
    }

    static String[] sort(List<String> dal, List<String> fal)
    {
        List<String> ret_sal = new ArrayList<String>(dal.size() + fal.size());
        Collections.sort(dal, FileListLoad.COMPARATOR_STRING_WITH_COLON_IGNORE_CASE);
        ret_sal.addAll(dal);
        Collections.sort(fal, FileListLoad.COMPARATOR_STRING_WITH_COLON_WITHOUT_EXTENSION_IGNORE_CASE);
        ret_sal.addAll(dal.size(), fal);
        return ret_sal.toArray(new String[ret_sal.size()]);
    }

    private static boolean isAssetFileADirectory(AssetManager am, String file)
    {
        InputStream is = null;
        try {
            is = am.open(file);
        } catch (IOException e) {
            //e.printStackTrace();
        }
        //Log.i(TAG, "" + is);
        boolean isDir = is == null;
        if (is != null) {
            try {
                is.close();
            } catch (IOException e) {
                //e.printStackTrace();
            }
        }
        return isDir;
    }

    public static boolean isValidFilename(String filenameOnly)
    {
        if (filenameOnly == null)
            return false;
        int indexDot = filenameOnly.lastIndexOf(".");
        if (indexDot <= 0)   // no extension
            return false;
        String lowerExtStr = filenameOnly.substring(1 + indexDot).toLowerCase();
        return lowerExtStr.equals("zip") || PlayMusicService.isSupportedMediaFileBasedOnExtension(lowerExtStr);
    }

    static class ZipFilterAudio implements ZipFileExContainer.ZipFileExZipFilter {
        @Override
        public boolean accept(String entryName)
        {
            return FileListLoad.isValidEntry(entryName);
        }
    }

    static class FileSystemAudioFilter extends ZipFileExFileSystemFilter {
        @Override
        public String[] listFiles(String pathname)
        {
            File[] fa = new File(pathname).listFiles(
                new FilenameFilter() {
                    @Override
                    public boolean accept(File dir, String filename)
                    {
                        File f = new File(dir, filename);
                        if (f.isDirectory())
                            return true;
                        return FileListLoad.isValidFilename(filename);
                    }
                }
            );
            String[] pathnames = new String[fa.length];
            for (int i = 0; i < fa.length; ++i)
                pathnames[i] = fa[i].getAbsolutePath();
            return pathnames;
        }

        @Override
        public String[] sort(List<String> dal, List<String> fal)
        {
            return FileListLoad.sort(dal, fal);
        }

        @Override
        public boolean isValidFilename(String filename)
        {
            return FileListLoad.isValidFilename(filename);
        }
    }

    static class AssetAudioFilter implements ZipFileExFileFilter {
        AssetManager mAssetManager;
        public AssetAudioFilter(AssetManager am) {
            mAssetManager = am;
        }

        @Override
        public boolean isDirectory(String pathname)
        {
            InputStream is = null;
            try {
                is = mAssetManager.open(pathname);
            } catch (IOException e) {
                //e.printStackTrace();
            }
            boolean isDir = is == null;
            if (is != null) {
                try {
                    is.close();
                } catch (IOException e) {
                    //e.printStackTrace();
                }
            }
            return isDir;
        }

        @Override
        public boolean isFile(String pathname)
        {
            return !isDirectory(pathname);
        }

        @Override
        public String[] listFiles(String pathname)
        {
            String[] filenames = null;
            try {
                filenames = mAssetManager.list(pathname);
            }
            catch (IOException e) {
                e.printStackTrace();
            }
            if (filenames == null) {
                return null;
            }
            String[] a = new String[filenames.length];
            String prefix = (pathname == null || pathname.isEmpty()) ? "" : (pathname + "/");
            int n = 0;
            for (int i = 0; i < filenames.length; ++i) {
                String pathname1 = prefix + filenames[i];
                if (
                    FileListLoad.isAssetFileADirectory(mAssetManager, pathname1) ||
                    FileListLoad.isValidFilename(filenames[i])
                ) {
                    a[n++] = pathname1;
                }
            }
            return Arrays.copyOfRange(a, 0, n);
        }

        @Override
        public InputStream getInputStream(String pathname) throws IOException
        {
            return mAssetManager.open(pathname);
        }

        @Override
        public String[] sort(List<String> dal, List<String> fal)
        {
            return FileListLoad.sort(dal, fal);
        }

        @Override
        public boolean isValidFilename(String filename)
        {
            return FileListLoad.isValidFilename(filename);
        }
    }
}
