package com.dn2soft.dicka;

import com.dn2soft.dicka.NationalCancerInstituteContract.NationalCancerInstituteJson;

import android.text.Spannable;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ClickableSpan;
import android.text.style.ForegroundColorSpan;

import android.app.Activity;
import android.text.method.LinkMovementMethod;
import android.content.res.Resources;
import com.dn2soft.dicka.LoadActivity;
import android.util.Log;
import android.content.Context;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.io.File;

public class NationalCancerInstituteDisplayFragment extends Fragment {
    private static final String TAG = "NationalCancerInstituteDisplayFragment";
    public static final String ARG_OBJECT = "Word Smart: The Words";

    @Override
    public View onCreateView(
        LayoutInflater  inflater,
        ViewGroup       container,
        Bundle          savedInstanceState
    )
    {
        super.onCreate(savedInstanceState);
        View rootView = inflater.inflate(R.layout.display, container, false);
        Bundle args = getArguments();
        String termsDefJsonStr = args.getString(ARG_OBJECT);
////    Log.i(TAG, "termsDefJsonStr: |" + termsDefJsonStr + "|");
        NationalCancerInstituteJson.TermsDef termsDef = NationalCancerInstituteContract.setTermsDef(termsDefJsonStr);
////    Log.i(TAG, "termsDef: " + termsDef);

        File dickaRootPath = LoadActivity.getDickRootPath();

        Resources   r = ((Context) getActivity()).getResources();
//          TextView textView = (TextView) rootView.findViewById(android.R.id.text1);
        TextView textView = (TextView) rootView.findViewById(R.id.display);
        textView.setMovementMethod(LinkMovementMethod.getInstance());
        textView.setText("");
        textView.scrollTo(0, 0);
        textView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
        textView.setTextColor(r.getColor(R.color.text));
        {
            String src = termsDef.src;
            String terms = termsDef.terms;
            final String mp3 = termsDef.mp3;
            String mtd = termsDef.mtd;
            String def = termsDef.def;

            Spannable termsSpan = new SpannableString(terms);
            if (!mp3.isEmpty()) {
                final Activity activity = getActivity();
                final String audioPath = dickaRootPath.getAbsolutePath() + "/mp3/";
                ClickableSpan playSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View view) {
                        PlaySound.play(activity, audioPath, mp3);
                    }
                };
                termsSpan.setSpan(playSpan, 0, terms.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            }
            termsSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, terms.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(termsSpan);
            if (!mtd.isEmpty()) {
                textView.append(" ");
                String  pron = "(" + mtd + ")";
                Spannable ipaSpan = new SpannableString(pron);
                ipaSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.ipa)), 0, pron.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(ipaSpan);
            }
            textView.append(" ");
            src = src.equals("1") ? "<Genetics Terms>" : "<Cancer Terms>";
            Spannable srcSpan = new SpannableString(src);
            srcSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.gw)), 0, src.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(srcSpan);
            textView.append("\n");
            Spannable defSpan = new SpannableString(def);
            defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, def.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(defSpan);
            textView.append("\n");
            textView.append("\n");
        }

        final String TERMS = termsDef.terms;
        textView.setOnLongClickListener(
            new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View pView) {
                    // Do something when your hold starts here.
                    Log.i(TAG, "hello, world: |" + TERMS + "|");
                    return true;
                }
            }
        );

        return rootView;
    }
}
