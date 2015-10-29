package com.dn2soft.dicka;

import android.util.Log;
import android.content.Context;

// io
import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

import android.app.Activity;    // requestWindowFeature, setProgressBarIndeterminateVisibility
import android.os.AsyncTask;
import android.view.MenuItem;   // setEnabled, setVisible
import android.app.ProgressDialog;

public final class DeployAsset extends AsyncTask<String, Integer, Void>
{
    private static final String TAG = "DeployAsset";

    private final Activity mActivity;  // setProgressBarIndeterminateVisibility
    private final MenuItem mSearch;    // setEnabled, setVisible
    public DeployAsset(Activity activity, MenuItem search) {
        mActivity = activity;
        mSearch = search;

        mSearch.setEnabled(false);
////    mSearch.setVisible(false);
    }

    private ProgressDialog mDialog;
    @Override
    protected void onPreExecute()
    {
        mDialog = new ProgressDialog(mActivity);
////    mDialog.setTitle("Indeterminate");
        mDialog.setMessage("Please wait while loading...");
        mDialog.setIndeterminate(true);
        mDialog.setCancelable(false);
        mDialog.setCanceledOnTouchOutside(false);
        mDialog.show();
        mActivity.setProgressBarIndeterminateVisibility(true);
    }

    @Override
    protected Void doInBackground(String... filesWithPath)
    {
        File dickRootPath = LoadActivity.getDickRootPath();
        int nFile =  filesWithPath.length;
        for (int i = 0; i < nFile; ++i) {
            String fileWithPath =  filesWithPath[i];
/*
            for (File file: dickRootPath.listFiles()) {
    Log.i(TAG, "|" + file.getAbsolutePath() + ": " + (file.isDirectory() ? "directory" : (file.isFile() ? "file" : "NONE")) + " (" + file.length() + ")");
            }
 */
            copyFileWithPath(dickRootPath,fileWithPath);
//          publishProgress(1 /* start */, i, nFile);
            // copy
/*
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {}
 */
//          publishProgress(0 /* end   */, i, nFile);
        }

        return null;
    }

    @Override
    protected void onProgressUpdate(Integer... progress)
    {
        boolean isStart = progress[0] == 1;
        int iFile = 1 + progress[1];
        int nFile = progress[2];
        if (isStart) {
////        Log.i(TAG, "[" + iFile + "/" + nFile + "]: start...");
        } else {  // progress[0] = 1
////        Log.i(TAG, "[" + iFile + "/" + nFile + "]: ...end");
        }
    }

    @Override
    protected void onPostExecute(Void x)
    {
        mActivity.setProgressBarIndeterminateVisibility(false);
        mSearch.setEnabled(true);
        mActivity.getActionBar().setSubtitle(null);
///     mSearch.setVisible(true);
        mDialog.cancel();
    }

    private void copyFileWithPath(File dickRootPath, String fileWithPathStr)
    {
        try {
            File fileWithPath = new File(dickRootPath, fileWithPathStr);
            if (fileWithPath.exists()) {
                ////Log.i(TAG, fileWithPath.getAbsolutePath() + ": exist");
                return;
            }
            // from...
            //final String  assets_dbname = "dicka/" + fileWithPathStr;
            final String  assets_dbname = "dicka/" + fileWithPathStr;
            BufferedInputStream bis = new BufferedInputStream(((Context) mActivity).getAssets().open(assets_dbname));
            // ...to
            BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(fileWithPath));
            final int BUF_LEN = 8192;
            byte[] buffer = new byte[BUF_LEN];
            int byteRead;
            while ((byteRead = bis.read(buffer, 0, BUF_LEN)) >= 0) {
                bos.write(buffer, 0, byteRead);
            }
            bis.close();
            bos.close();
        }
        catch (NullPointerException e) {
            // File(File, String)
            e.printStackTrace();
        }
        catch (FileNotFoundException e) {
            // FileOutputStream(String)
            e.printStackTrace();
        }
        catch (IOException e) {
            // AssetManager.open
            // BufferedInputStream.read
            // BufferedOutputStream.write
            // BufferedInputStream.close
            // BufferedOutputStream.close
            e.printStackTrace();
        }
    }
}
