package com.dn2soft.dicka;

import android.os.AsyncTask;

import android.content.DialogInterface;
import android.app.AlertDialog;
import android.app.Activity;
import com.dn2soft.dickc.dictionary.Cambridge;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ClickableSpan;
import android.text.style.ForegroundColorSpan;

import android.text.method.LinkMovementMethod;
import android.content.res.Resources;
import android.util.Log;
import android.content.Context;

import java.io.File;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;


public class CambridgeDisplayFragment extends Fragment {
    private static final String TAG = "CambridgeDisplayFragment";
    public static final String ARG_OBJECT_WORD_STR = "Cambridge Word";
    public static final String ARG_OBJECT_JSON_STR = "Cambridge JSON";

    private static void paint(final Activity activity, TextView textView, String wordStr, Cambridge.Result result)
    {
        Context context = (Context) activity;
        Resources res = context.getResources();
        textView.setMovementMethod(LinkMovementMethod.getInstance());
        textView.setText("");
        textView.scrollTo(0, 0);
        textView.setTextSize(res.getDimension(R.dimen.textsize_DEBUG));
        textView.setTextColor(res.getColor(R.color.text));

        if (result == null) {
            textView.setText("null");
            return;
        }
/*
        Log.i(TAG, "ResultBase:");
        Log.i(TAG, "    wordStr: |" + result.wordStr + "|");
        Log.i(TAG, "    WarnMessage: |" + result.WarnMessage + "|");
        Log.i(TAG, "    ErrorMessageList: " + result.ErrorMessageList);
        if (result.ErrorMessageList != null)
            for (String s: result.ErrorMessageList)
                Log.i(TAG, "        |" + s + "|");
        Log.i(TAG, "    tryWordList: " + result.tryWordList);
        if (result.tryWordList != null)
            for (String w: result.tryWordList)
                Log.i(TAG, "        |" + w + "|");
        Log.i(TAG, "Result:");
        Log.i(TAG, "    Result.doc: " + result.doc);
        if (result.doc != null)
            Log.i(TAG, "    # of documents = " + result.doc.size());
 */
        if (result.WarnMessage != null) {
            textView.append("|" + wordStr + "|\n");
            textView.append("Warning Message\n");
            textView.append(result.WarnMessage);
            return;
        }
        if (result.ErrorMessageList != null) {
            textView.append("|" + wordStr + "|\n");
            textView.append("Error Message\n");
            for (String msg: result.ErrorMessageList)
                textView.append(msg + "\n");
            return;
        }
        if (result.tryWordList != null) {
            textView.append("|" + wordStr + "|\n");
            textView.append("Not found; try these words\n");
            for (String word: result.tryWordList)
                textView.append(word + "\n");
            return;
        }

        // play in another thread
        File dickRootPath = LoadActivity.getDickRootPath();
        PlaySound.play(activity, dickRootPath.getAbsolutePath() + "/ogg/", Cambridge.getAudioURL(result));

////////String word0 = null;
        for (Cambridge.Result.Page page: result.doc) {
            for (Cambridge.Block block: page.content) {
                Cambridge.Block.Head    head = block.head;
                String word = head.word;
////////        if (word0 == null)
////////            word0 = word;
                final String audioPath = dickRootPath.getAbsolutePath() + "/ogg/";
                final String audioURL = head.getAudioURL();
                Spannable wordSpan = new SpannableString(word);
                if (audioURL != null) {
                    ClickableSpan playSpan = new ClickableSpan() {
                        @Override
                        public void onClick(View view) {
                            PlaySound.play(activity, audioPath, audioURL);
                        }
                    };
                    wordSpan.setSpan(playSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                }
                wordSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.word)), 0, head.word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(wordSpan);
                if (!head.ipa.isEmpty()) {
                    textView.append(" ");
                    int i = 0;
                    for (String s: head.ipa) {
                        if (i++ != 0) {
                            Spannable commaSpan = new SpannableString(",");
                            commaSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.word)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                            textView.append(commaSpan);
                        }
                        s = "/" + s + "/";
                        Spannable ipaSpan = new SpannableString(s);
                        ipaSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.ipa)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(ipaSpan);
                    }
                }
                if (!head.pos.isEmpty()) {
                    textView.append(" ");
                    Spannable LSpan = new SpannableString("(");
                    LSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.pos)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(LSpan);
                    int i = 0;
                    for (String s: head.pos) {
                        if (i++ != 0) {
                            Spannable commaSpan = new SpannableString(",");
                            commaSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.word)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                            textView.append(commaSpan);
                        }
                        Spannable posSpan = new SpannableString(s);
                        posSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.pos)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(posSpan);
                    }
                    Spannable RSpan = new SpannableString(")");
                    RSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.pos)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(RSpan);
                }
                if (head.gw != null && !head.gw.isEmpty()) {
                    textView.append(" ");
                    String s = "[" + head.gw + "]";
                    Spannable gwSpan = new SpannableString(s);
                    gwSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.gw)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(gwSpan);
                }
                textView.append("\n");
                for (Cambridge.Block.Body body: block.bodyList) {
                    if (body.def != null && !body.def.isEmpty()) {
                        String s = body.def;
                        Spannable defSpan = new SpannableString(s);
                        defSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.def)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(defSpan);
                        textView.append("\n");
                    }
                    for (String e: body.example) {
                        textView.append("    ");
                        Spannable dotSpan = new SpannableString("·");
                        dotSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.example_dot)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(dotSpan);
                        textView.append(" ");
                        Spannable exSpan = new SpannableString(e);
                        exSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.example)), 0, e.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(exSpan);
                        textView.append("\n");
                    }
                }
            }
            if (result.doc.size() > 1) {    // if Flag.showAll is set
                textView.append("\n");
                continue;
            }
            if (!result.doc.get(0).more.infoList.isEmpty()) {
                textView.append("\n\n");
                String s = "more:";
                Spannable moreSpan = new SpannableString(s);
                moreSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.info)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(moreSpan);
                textView.append("\n");
            }
            for (Cambridge.More.Info info: result.doc.get(0).more.infoList) {
                String  linkStr = info.word;
                if (info.pos != null && !info.pos.isEmpty()) {
                    linkStr += " (" + info.pos + ")";
                }
                if (info.gw != null && !info.gw.isEmpty()) {
                    linkStr += " [" + info.gw + "]";
                }
                final String    arg_query = info.word;
                final String    arg_href = info.href;
////            Log.i(TAG, "href: |" + arg_href + "|");
////            final Argument  arg = argument;
                ClickableSpan hrefSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View view) {
////                    new Dispatcher(arg, true).execute(href);
                        new Search(activity, arg_query, arg_href).execute();
                    }
                };
                Spannable linkSpan = new SpannableString(linkStr);
                linkSpan.setSpan(hrefSpan, 0, linkStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                //linkSpan.setSpan(hrefSpan, 0, index, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                int indexEnd = info.word.length();
                linkSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.word)), 0, indexEnd, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                if (info.pos != null && !info.pos.isEmpty()) {
                    int index = indexEnd + 1 + 1 + info.pos.length() + 1;
                    linkSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.pos)), indexEnd + 1, index, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    indexEnd = index;
                }
                if (info.gw != null && !info.gw.isEmpty()) {
                    int index = indexEnd + 1 + 1 + info.gw.length() + 1;
                    linkSpan.setSpan(new ForegroundColorSpan(res.getColor(R.color.gw)), indexEnd + 1, index, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    indexEnd = index;
                }
                textView.append("\n");
                textView.append(linkSpan);
                textView.append(" \n");
            }
        }
    }

    private static void lookup(final Activity activity, final TextView textView, final String wordStr)
    {
        final Context context = (Context) activity;
        (new AsyncTask<Void, Void, Cambridge.Result>() {
            @Override
            protected void onPreExecute()
            {
                activity.getActionBar().setSubtitle("searching... |" + wordStr + "|");
                activity.setProgressBarIndeterminateVisibility(true);
            }
            @Override
            protected Cambridge.Result doInBackground(Void...none)
            {
                CambridgeDatabase.CambridgeOpenHelper helper = null;
                Cambridge.Result result = null;
                helper = new CambridgeDatabase.CambridgeOpenHelper(context, LoadActivity.getDickRootPath());
                if (helper != null) {
                    String savedHref0 = helper.getSavedHref0(wordStr);
                    result = helper.lookup(wordStr, savedHref0);
                    helper.close();
                }
                return result;
            }
            @Override
            protected void onPostExecute(Cambridge.Result result)
            {
                activity.setProgressBarIndeterminateVisibility(false);
                activity.getActionBar().setSubtitle(null);
                CambridgeDisplayFragment.paint(activity, textView, wordStr, result);
            }
        }).execute();
    }

    @Override
    public View onCreateView(
        LayoutInflater  inflater,
        ViewGroup       container,
        Bundle          savedInstanceState
    )
    {
        super.onCreate(savedInstanceState);

        final Activity activity = getActivity();
        final Context context = (Context) activity;
        View rootView = inflater.inflate(R.layout.display, container, false);
        TextView textView = (TextView) rootView.findViewById(R.id.display);

        Bundle args = getArguments();
        final String wordStr = args.getString(ARG_OBJECT_WORD_STR);
        ////Log.i(TAG, "wordStr: |" + wordStr + "|");
        String cambridgeResultJsonStr = args.getString(ARG_OBJECT_JSON_STR);
        ////Log.i(TAG, "cambridgeResultJsonStr: |" + cambridgeResultJsonStr + "|");
        if (cambridgeResultJsonStr == null) {   // not found in Word Smart (see RootListActivity.java)
            Resources res = context.getResources();
            textView.setMovementMethod(LinkMovementMethod.getInstance());
            textView.setText("");
            textView.scrollTo(0, 0);
            textView.setTextSize(res.getDimension(R.dimen.textsize_DEBUG));
            textView.setTextColor(res.getColor(R.color.text));

            Spannable span;
            String s;
            s = wordStr;
                        span = new SpannableString(s);
                        span.setSpan(new ForegroundColorSpan(res.getColor(R.color.word)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(span);
            textView.append(" ");
            s = "not found in";
                        span = new SpannableString(s);
                        span.setSpan(new ForegroundColorSpan(res.getColor(R.color.ipa)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(span);
            textView.append(" ");
            s = "Word Smart";
                        span = new SpannableString(s);
                        span.setSpan(new ForegroundColorSpan(res.getColor(R.color.info)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(span);
            textView.append("\n");
            s = "Search it in";
                        span = new SpannableString(s);
                        span.setSpan(new ForegroundColorSpan(res.getColor(R.color.ipa)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(span);
            textView.append(" ");
            s = "Cambridge dictionary";
////            Log.i(TAG, "href: |" + arg_href + "|");
////            final Argument  arg = argument;
                ClickableSpan hrefSpan = new ClickableSpan() {
                    @Override
                    public void onClick(final View view) {
                        //Log.i(TAG, "" + view);
                        CambridgeDisplayFragment.lookup(activity, (TextView) view, wordStr);
                    }
                };
                span = new SpannableString(s);
                span.setSpan(hrefSpan, 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                span.setSpan(new ForegroundColorSpan(res.getColor(R.color.gw)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(span);
            textView.append("\n");
            return rootView;
        }

        Cambridge.Result result = CambridgeContract.setCambridgeResult(cambridgeResultJsonStr);
        ////Log.i(TAG, "Result: " + result);
        ////Log.i(TAG, "Result.wordStr: |" + result.wordStr + "|");
        result.wordStr = wordStr;
        CambridgeDisplayFragment.paint(activity, textView, wordStr, result);

/*
        textView.setOnLongClickListener(
            new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View pView) {
                    // Do something when your hold starts here.
                    new AlertDialog.Builder(context)
                        .setTitle("FYI...")
                        .setMessage("word search only in Word Smart")
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
                        .create()
                        .show();
                    return true;
                }
            }
        );
 */
        return rootView;
    }
}
