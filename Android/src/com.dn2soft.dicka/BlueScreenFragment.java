package com.dn2soft.dicka;

import android.text.method.LinkMovementMethod;
import android.content.res.Resources;
import android.util.Log;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class BlueScreenFragment extends Fragment {
    private static final String TAG = "BlueScreenFragment";
    public static final String ARG_OBJECT_FATAL = "Blue Screen: FATAL";

    @Override
    public View onCreateView(
        LayoutInflater  inflater,
        ViewGroup       container,
        Bundle          savedInstanceState
    )
    {
        super.onCreate(savedInstanceState);
        View rootView = inflater.inflate(R.layout.blue_screen, container, false);
        Bundle args = getArguments();
        String message = args.getString(ARG_OBJECT_FATAL);
////    Log.i(TAG, "theWordsJsonStr: |" + message + "|");
        Resources   r = ((Context) getActivity()).getResources();
//          TextView textView = (TextView) rootView.findViewById(android.R.id.text1);
        TextView textView = (TextView) rootView.findViewById(R.id.display);
        textView.setMovementMethod(LinkMovementMethod.getInstance());
        textView.setText("");
//      textView.scrollTo(0, 0);
//      textView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
//      textView.setBackgroundColor(android.R.color.holo_blue_dark);
        textView.setText(message);
        return rootView;
    }
}
