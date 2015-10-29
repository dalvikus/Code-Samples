package com.dn2soft.dicka;

import android.util.Log;

import com.dn2soft.dicka.LabelContract.LabelForJson;
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
import java.util.Arrays;
import android.text.TextUtils;


public class LabelMenuDialogFragment extends DialogFragment {
    private static final String TAG = "LabelMenuDialogFragment";

    private static final String STRING_ARRAY_ARRAYLIST_JSON_STR = "Label Menu: JSON string of StringArrayArrayList";

    public static LabelMenuDialogFragment newInstance(String saalJsonStr)
    {
        LabelMenuDialogFragment fragment = new LabelMenuDialogFragment();
        Bundle args = new Bundle();
        args.putString(STRING_ARRAY_ARRAYLIST_JSON_STR, saalJsonStr);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final Activity activity = getActivity();
        final Context context = (Context) activity;

        final List<List<String>> displayLabelList = StringArrayArrayList.set(getArguments().getString(STRING_ARRAY_ARRAYLIST_JSON_STR)).mA;
        ArrayAdapterEx adapter = new ArrayAdapterEx(context, ArrayAdapterEx.TYPE_TWO_ITEMS);
        List<Entry> entries = new ArrayList<Entry>();
        for (List<String> sl: displayLabelList) {   // ["word", "sense", ...]
            String[] sa = sl.toArray(new String[sl.size()]);
            entries.add(
                new Entry(
                    sa[LabelForJson.DISPLAY_LABEL_INDEX],
                    TextUtils.join(", ", Arrays.copyOfRange(sa, LabelForJson.WORD_ID_OFFSET, sa.length))
                )
            );
        }
        adapter.setData(entries);

        AlertDialog.Builder builder = new AlertDialog.Builder(context).setTitle("Labels...");
        if (displayLabelList.size() == 0) {
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
        }
        AlertDialog.Builder adb = new AlertDialog.Builder(context)
            .setTitle("Labels...")
            .setAdapter(
                adapter,
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        String displayLabel = displayLabelList.get(which).get(1);
                        Log.i(TAG, "label: |" + displayLabel + "|");
                        new LabelLoad(
                            activity,
                            displayLabel.substring(1 + displayLabel.lastIndexOf("/")),
                            null,
                            null
                        ).execute();
                    }
                }
             );
        if (displayLabelList.size() > 0)
            adb = adb.setNeutralButton(
                "manage labels",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        DialogFragment manageLabel = LabelManageDialogFragment.newInstance();
                        manageLabel.show(getFragmentManager(), "dialog");
                    }
                }
            );
        return adb.create();
    }
}
