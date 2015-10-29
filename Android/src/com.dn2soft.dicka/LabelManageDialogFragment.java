package com.dn2soft.dicka;

import android.widget.Toast;
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

public class LabelManageDialogFragment extends DialogFragment {
    private static final String TAG = "LabelManageDialogFragment";

    public static LabelManageDialogFragment newInstance() {
        LabelManageDialogFragment frag = new LabelManageDialogFragment();
/*
        Bundle args = new Bundle();
        args.putString(WORD, word);
        args.putStringArrayList(UPDATE_WORD_LIST, updateDataList);
        frag.setArguments(args);
 */
        return frag;
    }
    private int[] mFromPosition = new int[] {-1};  // -1: move, > -1 (> 0): rename
    private EditText mEditLabelEditText;        // for new label if any
    private ListView mLabelListView;            // for checked item's position
    private static final String SELECTED_INDEX = "selected index";
    private static final String NEW_LABEL = "new label";
    private static final String FROM_INDEX = "from index";
    @Override
    public void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        if (mFromPosition != null)
            outState.putIntArray(FROM_INDEX, mFromPosition);
        if (mEditLabelEditText != null)
            outState.putCharSequence(NEW_LABEL, mEditLabelEditText.getText());
        if (mLabelListView != null)
            outState.putInt(SELECTED_INDEX, mLabelListView.getCheckedItemPosition());
    }

    private AlertDialog buildAlertDialog(String message)
    {
        return (new AlertDialog.Builder(getActivity()))
              .setTitle("Alert")
              .setMessage(message)
              .setCancelable(false)
              .setPositiveButton(
                "okay",
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
    private void showAlertDialog(CharSequence message, String idStr)
    {
        boolean noMoreCheckBox = false;
        if (idStr != null) {
            String noMoreIdStr = MessageDialogFragment.getNoMoreIdString(idStr);
            SharedPreferences settings = getActivity().getSharedPreferences(SettingsActivity.PREFS_NAME, Context.MODE_PRIVATE);
            noMoreCheckBox = settings.getBoolean(noMoreIdStr, false);
////        Log.i(TAG, noMoreIdStr + ": " + noMoreCheckBox);
        }
        if (idStr == null || !noMoreCheckBox) {
            DialogFragment messageFragment = MessageDialogFragment.newInstance(message, idStr);
            messageFragment.show(getFragmentManager(), "message dialog");
        }
    }
    private static final String ROOT_LABEL = "/";
    private static final int[] mLastItemIndex = new int[] {-1};
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        // get data from db
        final File dickaRootPath = LoadActivity.getDickRootPath();
        LabelDatabase.LabelOpenHelper labelHelper = new LabelDatabase.LabelOpenHelper((Context) getActivity(), dickaRootPath);
        final LabelContract.LabelForJson labelForJson = labelHelper.select();
        labelHelper.close();
        final ArrayList<String[]> displayLabelArrayList = labelForJson.getAllDisplayLabelArray();
        if (displayLabelArrayList.size() == 0) {
            Log.e(TAG, "no labels");
            return null;
        }
        String[] labelArray = new String[1 + displayLabelArrayList.size()]; // 1 for ROOT_LABEL
        labelArray[0] = ROOT_LABEL;
        for (int i = 0; i < displayLabelArrayList.size(); ++i)
            labelArray[1 + i] = "/" + displayLabelArrayList.get(i)[LabelForJson.DISPLAY_LABEL_INDEX];

        // set initial values; initialization will be done after setting views
        int initPos = 0 + 1;   // 1: skip ROOT_LABEL
        CharSequence newLabel = null;
        mFromPosition = null;
        if (savedInstanceState != null) {
            mFromPosition = savedInstanceState.getIntArray(FROM_INDEX);
            if (mFromPosition == null)
                mFromPosition = new int[] {-1};
            newLabel = savedInstanceState.getCharSequence(NEW_LABEL);
            if (newLabel == null) { // impossible
                Log.e(TAG, "newLabel is null from savedInstanceState");
            }
            int i = savedInstanceState.getInt(SELECTED_INDEX);
            if (i == 0) {   // no mapping
            } else {
                initPos = i;
            }
        }
        mLastItemIndex[0] = initPos;
        if (newLabel == null)
            newLabel = labelArray[initPos].substring(1);    // skip leading '/'
        if (mFromPosition == null)
            mFromPosition = new int[] {-1};


        // set views
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View labelLayoutView = inflater.inflate(R.layout.label_manage, null);
        mLabelListView = (ListView) labelLayoutView.findViewById(R.id.label_list);
        mLabelListView.setAdapter(
            new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_list_item_single_choice,
                labelArray
            )
        );
        mLabelListView.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);

        mEditLabelEditText = (EditText) labelLayoutView.findViewById(R.id.edit_label);
        mEditLabelEditText.selectAll();
        CheckBox moveLabelCheckBox = (CheckBox) labelLayoutView.findViewById(R.id.move_label);
        Button cancelButton = (Button) labelLayoutView.findViewById(R.id.cancel_button);
        final Button actionButton = (Button) labelLayoutView.findViewById(R.id.action_button);

        // initialization
        moveLabelCheckBox.setChecked(mFromPosition[0] > -1);
        mEditLabelEditText.setText(newLabel);
        mLabelListView.setItemChecked(initPos, true);
        if (mFromPosition[0] > -1) {
            mEditLabelEditText.setEnabled(false);
            int fromPos = mFromPosition[0];
            boolean isToSet = fromPos != initPos;
            if (isToSet) {
                actionButton.setText("move");
            } else {
                actionButton.setText("select |under|");
            }
            actionButton.setEnabled(isToSet);
////        Toast.makeText(getActivity(), "move |" + newLabel + "|", Toast.LENGTH_SHORT);
        } else {
            mEditLabelEditText.selectAll();
            actionButton.setText("rename");
            actionButton.setEnabled(!LabelForJson.getLabelOnly(displayLabelArrayList.get(initPos - 1)[LabelForJson.DISPLAY_LABEL_INDEX]).contentEquals(newLabel));
        }

        // set callbacks
        final ListView labelListView = mLabelListView;
        final EditText editLabelEditText = mEditLabelEditText;
        final int[] fromPosition = mFromPosition;
        final int[] lastItemIndex = mLastItemIndex;
        moveLabelCheckBox.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    CheckBox cb = (CheckBox) v;
                    int curPos = labelListView.getCheckedItemPosition();
                    if (curPos == 0) {
                        if (cb.isChecked()) {
                            showAlertDialog("root cannot be moved", "root not moved");
                            cb.setChecked(false);
                            return;
                        }
                        curPos = fromPosition[0];
                        labelListView.setItemChecked(curPos, true);
                    }
                    String[] cur = displayLabelArrayList.get(curPos - 1);
                    String curLabel = LabelForJson.getLabelOnly(cur[LabelForJson.DISPLAY_LABEL_INDEX]);
                    if (cb.isChecked()) {
/*
                        CharSequence newLabel = editLabelEditText.getText();
                        if (!curLabel.equals(newLabel.toString())) {
                            showAlertDialog("rename label: |" + curLabel + "| -> |" + newLabel + "|: lost", null);
                            editLabelEditText.setText(curLabel);
                        }
 */
                        actionButton.setText("select |parent|");
                        actionButton.setEnabled(false);
                        editLabelEditText.setEnabled(false);
                        fromPosition[0] = curPos;
                        return;
                    }
                    fromPosition[0] = -1;
                    actionButton.setText("rename");
                    actionButton.setEnabled(false);
                    editLabelEditText.setEnabled(true);
                    editLabelEditText.setText(curLabel);
                    editLabelEditText.selectAll();
                }
            }
        );
        mEditLabelEditText.addTextChangedListener(
            new TextWatcher() {
                @Override
                public void afterTextChanged(Editable s)
                {
                }
                @Override
                public void beforeTextChanged(CharSequence s, int start, int count, int after)
                {
                }
                @Override
                public void onTextChanged(CharSequence s, int start, int count, int after)
                {
                    if (fromPosition[0] > -1) {    // actually > 0
                        // move label
                        return;
                    }

                    if (s.length() == 0) {
                        actionButton.setEnabled(false);
                        return;
                    }
                    String newLabel = s.toString();
                    if (newLabel.contains("/")) {
                        showAlertDialog("slash(/) not allowed", null);
                        actionButton.setEnabled(false);
                        return;
                    }
                    int curPos = labelListView.getCheckedItemPosition();    // curPos > 0; handled in ListView.setOnClickListener
                    String[] cur = displayLabelArrayList.get(curPos - 1);
                    String curFullLabel = cur[LabelForJson.DISPLAY_LABEL_INDEX];
                    String curLabel = LabelForJson.getLabelOnly(curFullLabel);
                    StringBuilder sb = new StringBuilder(curFullLabel.substring(0, curFullLabel.lastIndexOf("/") + 1));    // 1: includeing last '/' if any
                    sb.append(newLabel);
                    String newFullLabel = sb.toString();
                    boolean isValid = true;
                    for (String[] sa: displayLabelArrayList) {
                        if (sa[LabelForJson.LABEL_ID_INDEX] != cur[LabelForJson.LABEL_ID_INDEX]) {
                            if (sa[LabelForJson.DISPLAY_LABEL_INDEX].equals(newFullLabel)) {
                                showAlertDialog("label |" + newLabel + "| exists as |" + sa[LabelForJson.DISPLAY_LABEL_INDEX] + "|", null);
                                isValid = false;
                                break;
                            }
                        } else {
                            if (curLabel.equals(newLabel)) {
                                //showAlertDialog("new label: |" + newLabel + "| (|" + curLabel + "|): case-insensitive, so ignored", "label case-insensitive");
                                isValid = false;
                                break;
                            }
                        }
                    }
                    actionButton.setEnabled(isValid);
                    if (isValid) {
////                    Log.i(TAG, "rename label: |" + curLabel + "|(|" + curFullLabel + "|) -> |" + newFullLabel + "|");
                    }
                }
            }
        );
        mLabelListView.setOnItemClickListener(
            new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> lv, View v, int position, long id)
                {
                    if (fromPosition[0] == -1) {   // rename label
                        if (position == 0) {
                            showAlertDialog("root(" + ROOT_LABEL + ") cannot be renamed", "root not renamed");
                            labelListView.setItemChecked(lastItemIndex[0], true);
                        } else {
                            String curLabel = LabelForJson.getLabelOnly(displayLabelArrayList.get(position - 1)[LabelForJson.DISPLAY_LABEL_INDEX]);
                            editLabelEditText.setText(curLabel);
                            editLabelEditText.selectAll();
                            actionButton.setEnabled(false);
                            lastItemIndex[0] = position;
                        }
                        return;
                    }
                    // move label
                    String[] to = position == 0 ? new String[] {"-1", null, "-1"} : displayLabelArrayList.get(position - 1);
                    String toFullLabel = to[LabelForJson.DISPLAY_LABEL_INDEX] == null ? null : "/" + to[LabelForJson.DISPLAY_LABEL_INDEX];
                    String toLabel = LabelForJson.getLabelOnly(toFullLabel);
                    int fromPos = fromPosition[0];
                    String[] from = displayLabelArrayList.get(fromPos - 1);
                    String fromFullLabel = "/" + from[LabelForJson.DISPLAY_LABEL_INDEX];
                    String fromLabel = LabelForJson.getLabelOnly(fromFullLabel);
                    boolean isToSet = fromPos != position;
                    if (fromPos == position) {
                        showAlertDialog("cannot move to itself", null);
                        labelListView.setItemChecked(lastItemIndex[0], true);
                        return;
                    }
                    if (to[LabelForJson.DISPLAY_LABEL_INDEX] != null && to[LabelForJson.DISPLAY_LABEL_INDEX].startsWith(from[LabelForJson.DISPLAY_LABEL_INDEX])) {
                        showAlertDialog("parent(" + fromLabel + " as " + fromFullLabel + ") cannot moved under child(" + toFullLabel + ")", null);
                        labelListView.setItemChecked(lastItemIndex[0], true);
                        return;
                    }
                    String newFullLabel = (toFullLabel == null ? "/" : (toFullLabel + "/")) + fromLabel;
                    if (newFullLabel.equalsIgnoreCase(fromFullLabel)) {
                        showAlertDialog("label(" + fromLabel + " as " + fromFullLabel + ") already under label(" + (toLabel == null ? "/" : toFullLabel) + ")", null);
                        labelListView.setItemChecked(lastItemIndex[0], true);
                        return;
                    }
////                Log.i(TAG, "move |" + fromLabel + "|(|" + fromFullLabel + "|) under |" + toFullLabel + "|");
                    editLabelEditText.setText(newFullLabel);
                    actionButton.setText("move");
                    actionButton.setEnabled(true);
                    lastItemIndex[0] = position;
                }
            }
        );
        cancelButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    LabelManageDialogFragment.this.getDialog().cancel();
                }
            }
        );
        actionButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v)
                {
                    int curPos = labelListView.getCheckedItemPosition();    // curPos > 0; handled in ListView.setOnClickListener
                    LabelDatabase.LabelOpenHelper labelHelper = new LabelDatabase.LabelOpenHelper((Context) getActivity(), dickaRootPath);
                    int iret;
                    if (fromPosition[0] == -1) {    // rename label
                        String[] cur = displayLabelArrayList.get(curPos - 1);
                        String curFullLabel = cur[LabelForJson.DISPLAY_LABEL_INDEX];
                        String curLabel = LabelForJson.getLabelOnly(curFullLabel);
                        CharSequence newLabel = editLabelEditText.getText();
                        StringBuilder sb = new StringBuilder(curFullLabel.substring(0, curFullLabel.lastIndexOf("/") + 1));    // 1: includeing last '/' if any
                        sb.append(newLabel);
                        String newFullLabel = sb.toString();
////                    Log.i(TAG, "rename label: |" + curLabel + "|(|" + curFullLabel + "|) -> |" + newLabel + "|(" + newFullLabel + ")|");
                        iret = labelHelper.updateLabel(cur[LabelForJson.LABEL_ID_INDEX], newLabel.toString(), null);
                    } else {
                        // move label
                        String[] to = curPos == 0 ? new String[] {"-1", null, "-1"} : displayLabelArrayList.get(curPos - 1);
                        int fromPos = fromPosition[0];
                        String[] from = displayLabelArrayList.get(fromPos - 1);
                        String fromFullLabel = "/" + from[LabelForJson.DISPLAY_LABEL_INDEX];
                        String fromLabel = LabelForJson.getLabelOnly(fromFullLabel);
                        String toFullLabel = to[LabelForJson.DISPLAY_LABEL_INDEX] == null ? null : "/" + to[LabelForJson.DISPLAY_LABEL_INDEX];
                        String toLabel = LabelForJson.getLabelOnly(toFullLabel);
                        String newFullLabel = (toFullLabel == null ? "/" : (toFullLabel + "/")) + fromLabel;
////                    Log.i(TAG, "move |" + fromLabel + "|[id = " + from[LabelForJson.LABEL_ID_INDEX] + ", parentId = " + from[LabelForJson.PARENT_ID_INDEX] +"](|" + fromFullLabel + "|) under |" + toFullLabel + "|[parentId = " + to[LabelForJson.PARENT_ID_INDEX] + "]");
                        iret = labelHelper.updateLabel(from[LabelForJson.LABEL_ID_INDEX], null, to[LabelForJson.LABEL_ID_INDEX]);
                    }
////                Log.i(TAG, "update returns " + iret);
                    labelHelper.close();
                    LabelManageDialogFragment.this.getDialog().cancel();
                }
            }
        );


        return new AlertDialog.Builder(getActivity())
            .setView(labelLayoutView)
            .setTitle("Manage Labels")
            .create();
    }
}
