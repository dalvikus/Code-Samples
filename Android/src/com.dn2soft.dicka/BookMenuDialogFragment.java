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

import com.dn2soft.dicka.LoadActivity.Entry;
import java.util.ArrayList;
import android.text.TextUtils;
import android.os.AsyncTask;
import java.io.File;


public class BookMenuDialogFragment extends DialogFragment {
    private static final String TAG = "BookMenuDialogFragment";

    public static BookMenuDialogFragment newInstance()
    {
        BookMenuDialogFragment fragment = new BookMenuDialogFragment();
        return fragment;
    }

    private static void showBookDialog(final Activity activity, final int book)
    {
        final Context context = (Context) activity;

        (new AsyncTask<Void, Void, List<List<String>>>() {
            @Override
            protected List<List<String>> doInBackground(Void...none)
            {
                WordSmartDatabase.WordSmartOpenHelper wsHelper = null;
                List<List<String>> saal = null;
                try {
                    File dickaRootPath = LoadActivity.getDickRootPath();
                    wsHelper = new WordSmartDatabase.WordSmartOpenHelper(context, dickaRootPath);
                    if (wsHelper != null)
                        saal = wsHelper.selectBook(book);
                } finally {
                    if (wsHelper != null)
                        wsHelper.close();
                }
                return saal;
            }

            @Override
            protected void onPostExecute(final List<List<String>> saal)
            {
                final ArrayAdapterEx adapter = new ArrayAdapterEx(context, ArrayAdapterEx.TYPE_TWO_ITEMS);
                List<Entry> entries = new ArrayList<Entry>();
                int unit = 0;
                for (List<String> sa: saal) {   // ["word", "sense", ...]
                    entries.add(
                        new Entry(
                            String.format("<#%d.%d>", book, ++unit),
                            TextUtils.join(", ", sa.toArray(new String[sa.size()]))
                        )
                    );
                }
                adapter.setData(entries);
                new AlertDialog.Builder(context)
                    .setTitle("Word Smart " + (book == 1 ? "I" : (book == 2 ? "II" : "for the GRE")))
                    .setAdapter(
                        adapter,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which)
                            {
                                String bookUnitStr = String.format("#%d.%d", book, 1 + which);
                                Log.i(TAG, "<" + bookUnitStr + ">");
                                List<String> sa = saal.get(which);
                                new BookLoad(activity, bookUnitStr).execute();
                            }
                        }
                     )
                    .create()
                    .show();
            }
        }).execute();
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final Activity activity = getActivity();
        final Context context = (Context) activity;

        return new AlertDialog.Builder(context)
            .setTitle("Books...")
            .setItems(
                new CharSequence[] {
                    "Word Smart I",
                    "Word Smart II",
                    "Word Smart for the GRE"
                },
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        BookMenuDialogFragment.showBookDialog(activity, 1 + which);                        
                    }
                }
             )
            .create();
    }
}
