package com.dn2soft.www.app;
import com.dn2soft.dicka.R;

import android.widget.Button;
import android.widget.EditText;

import android.util.Log;

import android.app.DialogFragment;
import android.app.Dialog;
import android.app.AlertDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;


public class SearchVideoMenuDialogFragment extends DialogFragment {
    private static final String TAG = "SearchVideoMenuDialogFragment";

    public static SearchVideoMenuDialogFragment newInstance()
    {
        SearchVideoMenuDialogFragment fragment = new SearchVideoMenuDialogFragment();
        return fragment;
    }


    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View searchVideoLayoutView = inflater.inflate(R.layout.search_video, null);

        final EditText searchEditText = (EditText) searchVideoLayoutView.findViewById(R.id.title);
        Button cancelBtn = (Button) searchVideoLayoutView.findViewById(R.id.cancel);
        Button searchVideoBtn = (Button) searchVideoLayoutView.findViewById(R.id.search);
        cancelBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    SearchVideoMenuDialogFragment.this.getDialog().cancel();
                }
            }
        );
        searchVideoBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    DialogFragment sv = SearchVideoDialogFragment.newInstance(
                        searchEditText.getText()
                    );
                    sv.setCancelable(false);
                    sv.show(getFragmentManager(), "search video");
                }
            }
        );

        return new AlertDialog.Builder(getActivity())
            .setCancelable(false)
            .setView(searchVideoLayoutView)
            .create();
    }
}
