package com.dn2soft.dicka;

import android.util.Log;

import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;

import java.util.List;
import java.util.ArrayList;
import com.dn2soft.dicka.LoadActivity.Entry;


public class RootMenuDialogFragment extends DialogFragment {
    private static final String TAG = "RootMenuDialogFragment";

    private static final String ROOT_SENSE_ARRAYLIST = "Root Menu: ArrayList<String>";

    public static RootMenuDialogFragment newInstance(ArrayList<String> rootSenseArrayList)
    {
        RootMenuDialogFragment fragment = new RootMenuDialogFragment();
        Bundle args = new Bundle();
        args.putStringArrayList(ROOT_SENSE_ARRAYLIST, rootSenseArrayList);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final Activity activity = getActivity();
        final Context context = (Context) activity;

        final ArrayList<String> rootSenseArrayList = getArguments().getStringArrayList(ROOT_SENSE_ARRAYLIST);
        int length = rootSenseArrayList.size() / 2;
        ArrayAdapterEx adapter = new ArrayAdapterEx(context, ArrayAdapterEx.TYPE_TWO_ITEMS);
        List<Entry> entries = new ArrayList<Entry>(length);
        for (int i = 0; i < length; ++i) {
            entries.add(
                new Entry(
                    rootSenseArrayList.get(2 * i),
                    rootSenseArrayList.get(2 * i + 1)
                )
            );
        }
        adapter.setData(entries);
        if (length == 0) {
            return new AlertDialog.Builder(context)
                .setTitle("Roots...")
                .setMessage("no root(?)")
                .setNeutralButton(
                    "okay",
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which)
                        {
                            //Log.i(TAG, "okay: onClick");
                        }
                    }
                 )
                .create();
        }
        return new AlertDialog.Builder(context)
            .setTitle("Roots...")
            .setAdapter(
                adapter,
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
//Log.i(TAG, "|" + rootSenseArrayList.get(2 * which) + "|: |" + rootSenseArrayList.get(2 * which + 1) + "|");
                        new RootLoad(activity, rootSenseArrayList.get(2 * which)).execute();
                    }
                }
             )
            .create();
    }
}
