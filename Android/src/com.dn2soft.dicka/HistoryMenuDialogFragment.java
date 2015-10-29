package com.dn2soft.dicka;

import android.util.Log;

import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;

import java.util.ArrayList;


public class HistoryMenuDialogFragment extends DialogFragment {
    private static final String TAG = "HistoryMenuDialogFragment";

    private static final String WORD_STR_LIST_IN_HISTORY = "History: ArrayList<String>";

    public static HistoryMenuDialogFragment newInstance(ArrayList<String> wordStrListInHistory)
    {
        HistoryMenuDialogFragment fragment = new HistoryMenuDialogFragment();
        Bundle args = new Bundle();
        args.putStringArrayList(WORD_STR_LIST_IN_HISTORY, wordStrListInHistory);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final Activity activity = getActivity();
        final Context context = (Context) activity;
        final ArrayList<String> wordStrListInHistory = getArguments().getStringArrayList(WORD_STR_LIST_IN_HISTORY);
        final CharSequence[] wordArray = wordStrListInHistory == null ? null : wordStrListInHistory.toArray(new CharSequence[wordStrListInHistory.size()]);
        AlertDialog.Builder builder = new AlertDialog.Builder(context).setTitle("History...");
        if (wordArray == null || wordArray.length == 0)
            return builder
                .setMessage("nothing yet")
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
        return new AlertDialog.Builder(context)
            .setTitle("History...")
            .setItems(
                wordArray,
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        Log.i(TAG, "word: |" + wordArray[which] + "|");
                        new Search(activity, wordArray[which].toString()).execute();
                    }
                }
             )
            .create();
    }
}
