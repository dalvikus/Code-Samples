package com.dn2soft.dicka;

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

import java.util.List;
import java.util.ArrayList;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

import java.io.File;

public class LabelDialogFragment extends DialogFragment {
    private static final String TAG = "LabelDialogFragment";

    private static final String WORD = "Word";
    private static final String UPDATE_WORD_LIST = "Updated Word List in Label";
    public static LabelDialogFragment newInstance(String word, ArrayList<String> updateDataList) {
        LabelDialogFragment frag = new LabelDialogFragment();
        Bundle args = new Bundle();
        args.putString(WORD, word);
        args.putStringArrayList(UPDATE_WORD_LIST, updateDataList);
        frag.setArguments(args);
        return frag;
    }
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final String word = getArguments().getString(WORD);
        final ArrayList<String> updateDataList = getArguments().getStringArrayList(UPDATE_WORD_LIST);
        File dickaRootPath = LoadActivity.getDickRootPath();
        LabelDatabase.LabelOpenHelper labelHelper = new LabelDatabase.LabelOpenHelper((Context) getActivity(), dickaRootPath);
        final LabelContract.LabelForJson labelForJson = labelHelper.select();
        labelHelper.close();
////        labelForJson.print();

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        // Get the layout inflater
        LayoutInflater inflater = getActivity().getLayoutInflater();
        final View labelLayoutView = inflater.inflate(R.layout.label, null);
        final ListView labelListView = (ListView) labelLayoutView.findViewById(R.id.label_list);
        final ArrayList<String[]> displayLabelArrayList = labelForJson.getDisplayLabelArray(word);
        String[] labelArray = new String[displayLabelArrayList.size()];
        for (int i = 0; i < displayLabelArrayList.size(); ++i)
            labelArray[i] = displayLabelArrayList.get(i)[LabelForJson.DISPLAY_LABEL_INDEX];
        labelListView.setAdapter(
            new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_list_item_multiple_choice,
                labelArray
/*
                new String[] {
                    "<h>",
                    "<e>",
                    "<l>",
                    "<l>",
                    "<o>",
                    "<,>",
                    "< >",
                    "<w>",
                    "<o>",
                    "<r>",
                    "<l>",
                    "<d>"
                }
*/
            )
        );
        labelListView.setChoiceMode(AbsListView.CHOICE_MODE_MULTIPLE);
        for (int i = 0; i < displayLabelArrayList.size(); ++i) {
            labelListView.setItemChecked(i, displayLabelArrayList.get(i)[LabelForJson.WORD_ID_OFFSET] != null);
        }
        //labelListView.setVisibility(View.GONE);

        final TextView createLabelTextView = (TextView) labelLayoutView.findViewById(R.id.msg_create_label);
        final CheckBox createLabelCheckBox = (CheckBox) labelLayoutView.findViewById(R.id.checkbox_create_label);
        createLabelCheckBox.setChecked(false);
        final EditText labelEditText = (EditText) labelLayoutView.findViewById(R.id.new_label);
        labelEditText.setVisibility(View.GONE);
        final TextView underLabelTextView = (TextView) labelLayoutView.findViewById(R.id.msg_under_label);
        underLabelTextView.setVisibility(View.GONE);
        final CheckBox underLabelCheckBox = (CheckBox) labelLayoutView.findViewById(R.id.checkbox_under_label);
        underLabelCheckBox.setVisibility(View.GONE);
        underLabelCheckBox.setChecked(false);
        final Button cancelButton = (Button) labelLayoutView.findViewById(R.id.cancel_button);
        final Button doneButton = (Button) labelLayoutView.findViewById(R.id.done_button);
        cancelButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    LabelDialogFragment.this.getDialog().cancel();
                }
            }
        );
        doneButton.setOnClickListener(
            new View.OnClickListener() {
                private AlertDialog buildAlertDialog(String message)
                {
                    return (new AlertDialog.Builder(getActivity()))
                          .setTitle("An Error Occurred")
                          .setMessage(message)
                          .setCancelable(false)
                          .setPositiveButton(
                            "confirm",
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id)
                                {
                                    dialog.dismiss();
                                }
                            }
                          )
                          .create();
                }

                @Override
                public void onClick(View v)
                {
////////////////////////////////////////////////////////////////////////
////Log.i(TAG, "create new label: " + createLabelCheckBox.isChecked());
////Log.i(TAG, "new label: |" + labelEditText.getText().toString() + "|");
////Log.i(TAG, "under the label: " + underLabelCheckBox.isChecked());
SparseBooleanArray labels = labelListView.getCheckedItemPositions();
////for (int i = 0; i < labelListView.getCount(); ++i) {
////    Log.i(TAG, "[" + i + "]: " + labels.get(i));
////}
String errorMessage = null;
LabelContract.UpdateData data = null;
try {
    data = labelForJson.setUpdateData(
        displayLabelArrayList,
        word,
        createLabelCheckBox.isChecked(), labelEditText.getText().toString(),
        underLabelCheckBox.isChecked(),
        labels
    );
////    data.print();
} catch (IllegalArgumentException e) {
    errorMessage = e.getMessage();
}
if (data == null) {
    Log.e(TAG, "Message: |" + errorMessage + "|");
    (buildAlertDialog(errorMessage)).show();
} else {
////    data.print();
    if (data.shouldUpdate().equals(Boolean.TRUE)) {
////        labelForJson.print();
        File dickaaRootPath = LoadActivity.getDickRootPath();
        LabelDatabase.LabelOpenHelper labelHelper = new LabelDatabase.LabelOpenHelper((Context) getActivity(), dickaaRootPath);
        boolean ret = labelHelper.update(data);
////        Log.i(TAG, "ret: " + ret);
        updateDataList.add(word);
        labelHelper.close();
////        labelForJson.print();
    } else {
////        Log.i(TAG, "no change");
    }

    LabelDialogFragment.this.getDialog().cancel();
}
////////////////////////////////////////////////////////////////////////
                }
            }
        );

        createLabelCheckBox.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    // Do something when your hold starts here.
                    CheckBox cb = (CheckBox) v;
                    if (cb.isChecked()) {
                        labelListView.setVisibility(View.GONE);
                        labelEditText.setVisibility(View.VISIBLE);
                        if (displayLabelArrayList.size() > 0) {
                            underLabelTextView.setVisibility(View.VISIBLE);
                            underLabelCheckBox.setVisibility(View.VISIBLE);
                            underLabelCheckBox.setChecked(false);   // start over with unchecked
                        }
                        /*
                        if (underLabelCheckBox.isChecked()) {
                            labelListView.setVisibility(View.VISIBLE);
                            doneButton.setText("create under");
                        } else {
                            labelListView.setVisibility(View.GONE);
                            doneButton.setText("create");
                        }
                         */
                    } else {
                        labelEditText.setVisibility(View.GONE);
                        underLabelTextView.setVisibility(View.GONE);
                        underLabelCheckBox.setVisibility(View.GONE);
                        labelListView.setVisibility(View.VISIBLE);
                        labelListView.setChoiceMode(AbsListView.CHOICE_MODE_MULTIPLE);
                        for (int i = 0; i < displayLabelArrayList.size(); ++i) {
                            labelListView.setItemChecked(i, displayLabelArrayList.get(i)[LabelForJson.WORD_ID_OFFSET] != null);
                        }
                        doneButton.setText("set");
                    }
                }
            }
        );
        underLabelCheckBox.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    // Do something when your hold starts here.
                    CheckBox cb = (CheckBox) v;
                    if (cb.isChecked()) {
                        labelListView.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
                        for (int i = 0; i < displayLabelArrayList.size(); ++i) {
                            labelListView.setItemChecked(i, false);
                        }
                        labelListView.setVisibility(View.VISIBLE);
                        doneButton.setText("create under");
                    } else {
                        labelListView.setVisibility(View.GONE);
                        doneButton.setText("create");
                    }
                }
            }
        );

        builder
              .setView(labelLayoutView)
              .setTitle(word)
        ;      
        return builder.create();
    }
}
