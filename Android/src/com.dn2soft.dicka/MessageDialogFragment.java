package com.dn2soft.dicka;

import android.text.Editable;
import android.text.TextWatcher;
import android.widget.AdapterView;
import com.dn2soft.dicka.LabelContract;
import com.dn2soft.dicka.LabelContract.LabelForJson;
import com.dn2soft.dicka.LabelDatabase;
import android.util.SparseBooleanArray;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;
import android.widget.EditText;
import android.widget.ArrayAdapter;
import android.widget.AbsListView;
import android.widget.ListView;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
//import android.app.FragmentTransaction;
/*
/disk/android/sdk/samples/android-16/ApiDemos/src/com/example/android/apis/app/AlertDialogSamples.java
/disk/android/sdk/samples/android-16/ApiDemos/src/com/example/android/apis/app/FragmentDialog.java
/disk/android/sdk/samples/android-16/ApiDemos/src/com/example/android/apis/view/ExpandableList3.java
/disk/android/sdk/samples/android-16/ApiDemos/res/layout/list_12.xml
/disk/android/sdk/samples/android-16/ApiDemos/res/layout/fragment_menu.xml
http://developer.android.com/guide/topics/ui/dialogs.html
http://developer.android.com/reference/android/app/DialogFragment.html
 */

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS
import android.util.Log;
import android.content.Context;
import android.content.SharedPreferences;

import java.util.List;
import java.util.ArrayList;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

import java.io.File;

public class MessageDialogFragment extends DialogFragment {
    private static final String TAG = "MessageDialogFragment";

    private static final String MESSAGE = "message";
    private static final String ID_STR = "id string";
    public static MessageDialogFragment newInstance(CharSequence msg, String idStr)
    {
        MessageDialogFragment frag = new MessageDialogFragment();
        Bundle args = new Bundle();
        args.putCharSequence(MESSAGE, msg);
        args.putString(ID_STR, idStr);
        frag.setArguments(args);
        return frag;
    }
    CharSequence mMessage;
    String mIdStr;
    CheckBox mNoMoreCheckBox;
    private static final String NO_MORE = "no more";
    public static final String getNoMoreIdString(String idStr)
    {
        return NO_MORE + "/" + idStr;
    }
    @Override
    public void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        if (mMessage != null)
            outState.putCharSequence(MESSAGE, mMessage);
        if (mIdStr != null) {
            outState.putString(ID_STR, mIdStr);
            if (mNoMoreCheckBox != null)
                outState.putBoolean(NO_MORE, mNoMoreCheckBox.isChecked());
        }
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Bundle arg = getArguments();
        mMessage = arg.getCharSequence(MESSAGE);
        mIdStr = arg.getString(ID_STR);

        boolean isChecked = false;
        if (mIdStr != null && savedInstanceState != null) {
            isChecked = savedInstanceState.getBoolean(NO_MORE, false);
        }

        // set views
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View messageLayoutView = inflater.inflate(R.layout.alert_message, null);
        TextView messageTextView = (TextView) messageLayoutView.findViewById(R.id.message);
        messageTextView.setText(mMessage);
        CheckBox mNoMoreCheckBox = (CheckBox) messageLayoutView.findViewById(R.id.no_more);
        mNoMoreCheckBox.setChecked(isChecked);
        if (mIdStr == null) {
            TextView noMoreTextTextView = (TextView) messageLayoutView.findViewById(R.id.no_more_text);
            mNoMoreCheckBox.setVisibility(View.GONE);
            noMoreTextTextView.setVisibility(View.GONE);
        }

        final CheckBox noMoreCheckBox = mNoMoreCheckBox;
        final String idStr = mIdStr;
        return new AlertDialog.Builder(getActivity())
            .setView(messageLayoutView)
            .setTitle("Alert Message")
            .setNeutralButton(
                "okay",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        if (idStr != null && noMoreCheckBox.isChecked()) {
                            SharedPreferences settings = getActivity().getSharedPreferences(SettingsActivity.PREFS_NAME, Context.MODE_PRIVATE);
                            SharedPreferences.Editor editor = settings.edit();
                            editor.putBoolean(MessageDialogFragment.getNoMoreIdString(idStr), true);
                            editor.commit();
                        }
                        MessageDialogFragment.this.getDialog().cancel();
                    }
                }
            )
            .create();
    }
}
