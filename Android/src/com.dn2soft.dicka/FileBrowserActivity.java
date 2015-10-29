package com.dn2soft.dicka;

import android.content.Intent;

import android.app.Activity;
import android.os.Bundle;
import android.app.FragmentManager;

public class FileBrowserActivity extends Activity {
    private static final String TAG = "FileBrowserActivity";

    private FileBrowserDialogFragment mFileBrowserDialogFragment;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        String homeDir = intent.getStringExtra(FileBrowserDialogFragment.HOME_DIR);
        boolean browseAssets = intent.getBooleanExtra(FileBrowserDialogFragment.BROWSE_ASSETS, false);
        FragmentManager fm = getFragmentManager();
        if (fm.findFragmentById(android.R.id.content) == null) {
            mFileBrowserDialogFragment = FileBrowserDialogFragment.newInstance(homeDir, browseAssets, false);
            fm.beginTransaction().add(android.R.id.content, mFileBrowserDialogFragment).commit();
        }
    }

    @Override
    public void onBackPressed()
    {
        if (!mFileBrowserDialogFragment.simulateBack())
            super.onBackPressed();
    }
}
