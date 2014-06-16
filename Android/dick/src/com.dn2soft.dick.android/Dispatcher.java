package com.dn2soft.dick.android;

// Activity
import android.app.Activity;
import android.content.Context;
// View
import android.view.View;
import android.content.Intent;
import android.widget.TextView;

// do task in background
import android.os.AsyncTask;
import java.io.IOException;
// call Cambridge.lookup
import com.dn2soft.dick.dictionary.Cambridge;
import com.dn2soft.dick.utility.Json;
import com.dn2soft.dick.utility.JsonWalk;
import com.dn2soft.dick.utility.AnsiText;
import java.util.List;
import java.util.Map;
import java.util.ArrayList;
// show the result
import android.content.res.Resources;
import android.text.style.ForegroundColorSpan;
import android.text.method.LinkMovementMethod;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ClickableSpan;
import android.text.style.StyleSpan;
import android.graphics.Typeface;
import android.graphics.Rect;
import android.graphics.Paint;
// play audio for pronunciation
import android.media.AudioManager;
import android.media.MediaPlayer;
// database
import com.dn2soft.dick.android.CambridgeContract.CambridgeDbHelper;
import com.dn2soft.dick.android.WordSmartContract.WordSmartDbHelper;
import com.dn2soft.dick.android.NationalCancerInstituteContract.NationalCancerInstituteDbHelper;
// log
import android.util.Log;

import java.io.File;
import java.net.URL;
import java.net.URLConnection;
import java.io.FileOutputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;

import java.util.regex.Pattern;
import java.util.regex.Matcher;

class Dispatcher extends AsyncTask<String, Void, Cambridge.Result>
{
    final static String ERROR_MSG = "Sorry for any inconvenience. An Error occurred. Please, report this:";
    String[] errorMessageArray = null;
    void setErrorMessageArray(Throwable e) {
        StackTraceElement[] eea = e.getStackTrace();
        errorMessageArray = new String[1 + eea.length];
        errorMessageArray[0] = e.toString();
        for (int i = 0; i < eea.length; ++i)
            errorMessageArray[1 + i] = "    " + eea[i].toString();
    }
    static String[] getErrorMessageArray(Throwable e) {
        StackTraceElement[] eea = e.getStackTrace();
        String[] ema = new String[1 + eea.length];
        ema[0] = e.toString();
        for (int i = 0; i < eea.length; ++i)
            ema[1 + i] = "    " + eea[i].toString();
        return ema;
    }
    static void displayErrorMessage(TextView textView, Resources r, String[] ema) {
        if (textView != null && ema != null) {
Log.i("ERROR", AnsiText.fatalStr0(Dispatcher.ERROR_MSG));
            textView.setText("");
            textView.scrollTo(0, 0);
            Spannable errorSpan = new SpannableString(ERROR_MSG);
            errorSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.error)), 0, ERROR_MSG.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(errorSpan);
            textView.append("\n");
            for (String msg: ema) {
                textView.append(msg + "\n");
            }
        }
    }
    static void displayErrorMessage(TextView textView, Resources r, List<String> errorMessageList) {
        if (textView != null && errorMessageList != null) {
            Dispatcher.displayErrorMessage(textView, r, (String[]) errorMessageList.toArray());
        }
    }

    static class Argument {
        Activity    mActivity;
        TextView    mTextView;
        String      mRootPath;
        Argument(
            Activity    activity,
            TextView    textView,
            String      rootPath
        ) {
            mActivity   = activity;
            mTextView   = textView;
            mRootPath   = rootPath;
        }
    }

    Argument    mArgument;
    boolean     mNewIntent;
    boolean     mWordSmartOnly;
    final String UNIT_MATCH_STR = "\\s*#([123])\\.(\\d+)";
    final Pattern mPattern = Pattern.compile(UNIT_MATCH_STR);
    CambridgeDbHelper   mCambridgeDbHelper;
    WordSmartDbHelper   mWordSmartDbHelper;
    NationalCancerInstituteDbHelper   mNationalCancerInstituteDbHelper;
    Dispatcher(Argument argument, boolean newIntent)
    {
        mArgument   = argument;
        mNewIntent  = newIntent;
        Context context = argument.mActivity.getApplicationContext();
        mCambridgeDbHelper = new CambridgeDbHelper(context, new File(argument.mRootPath));
        mWordSmartDbHelper = new WordSmartDbHelper(context, new File(argument.mRootPath));
        mNationalCancerInstituteDbHelper = new NationalCancerInstituteDbHelper(context, new File(argument.mRootPath));
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        errorMessageArray = null;
        if (!DickA.EMULATOR)
            mArgument.mActivity.setProgressBarIndeterminateVisibility(true);
    }

    @Override
    protected Cambridge.Result doInBackground(String... params) {
        String  wordStr = params[0];
        if (wordStr == null || wordStr.isEmpty()) {
            return null;
        }
        wordStr = wordStr.trim().replaceAll("\\s+", " ");

        boolean special =
            wordStr.startsWith("!") ||
            wordStr.startsWith("@") ||
            wordStr.startsWith("#") ||  // #3.4
            wordStr.contains("%") ||
            wordStr.toUpperCase().equals(wordStr)
        ;
        String href0 = null;
        if (!special)
            href0 = mCambridgeDbHelper.href0(wordStr);
//Log.i("DB", href0 == null ? "null" : href0);
        mWordSmartOnly = href0 == null && !DickA.EMULATOR && !DickA.getWiFiInfoStr(mArgument.mActivity, false).isEmpty() && DickA.WORD_SMART_ONLY;
        if (special || mWordSmartOnly) {
            Cambridge.Result result = new Cambridge.Result();
            result.wordStr = wordStr;
            if (wordStr.startsWith("!")) {
                String str = wordStr.substring(1);
//Log.i("NationalCancerInstituteDbHelper", str);
                if (str.startsWith("!")) {
                    str = str.substring(1);
                    if (!str.replaceAll("%", "").isEmpty())
                        result.WarnMessage = null;
                        try {
                            result.WarnMessage = mNationalCancerInstituteDbHelper.select(str);
                        }
                        catch (ClassNotFoundException e) {e.printStackTrace();}
                        catch (IllegalAccessException e) {setErrorMessageArray(e);}
                } else {
                    if (!str.replaceAll("%", "").isEmpty())
                        result.ErrorMessageList = mNationalCancerInstituteDbHelper.getTerms(str);
                }
            } else if (wordStr.startsWith("@")) {
                String str = wordStr.substring(1);
                if (!str.replaceAll("%", "").isEmpty())
                    result.ErrorMessageList = mWordSmartDbHelper.getRootWord(str);
            } else if (wordStr.contains("#")) {
                Matcher m = mPattern.matcher(wordStr);
                if (m.matches()) {
                    result.ErrorMessageList = mWordSmartDbHelper.lookupUnit(m.group(1), m.group(2));
                    if (result.ErrorMessageList.isEmpty()) {
                        result.WarnMessage = "|" + wordStr + "|: Not found: |" + UNIT_MATCH_STR + "|";
                    }
                } else {
                    result.WarnMessage = "|" + wordStr + "|: No/invalid match: |" + UNIT_MATCH_STR + "|";
                }
            } else if (wordStr.contains("%")) {
                result.ErrorMessageList = mWordSmartDbHelper.suggestWord(wordStr);
            } else {
                result.WarnMessage = null;
                try {
                    result.WarnMessage = mWordSmartDbHelper.selectJSONStr(wordStr);
                }
                catch (ClassNotFoundException e) {e.printStackTrace();}
                catch (IllegalAccessException e) {setErrorMessageArray(e);}
            }
            return result;
        }

/*
    public boolean  uk          = false;
    public boolean  showAll     = false;
    public boolean  force       = false;
    public boolean  playPron    = false;
    public boolean  showLink    = true;
    public boolean  ansi        = true;
    public boolean  verbose     = false;
    public boolean  trace       = false;
    public boolean  showHref0   = false;
    public boolean  showHref0Only   = false;// if set, showHref0 implied
    public boolean  followWord  = false;// gui mode only
    public boolean  resultOnly  = false;
*/
        Cambridge.Flag      flag = new Cambridge.Flag();
        flag.uk             = false;    // configurable
        flag.showAll        = false;    // configurable
        flag.force          = false;    // configurable
        flag.playPron       = true;     // always
        flag.showLink       = true;     // always
        flag.ansi           = false;    // always
        flag.verbose        = false;    // RELEASE: always, DEBUG: configurable
        flag.verbose        = false;    // RELEASE: always, DEBUG: configurable
        flag.showHref0      = false;    // RELEASE: always, DEBUG: configurable
        flag.showHref0Only  = false;    // RELEASE: always, DEBUG: configurable
        flag.followWord     = false;    // configurable
        flag.resultOnly     = true;     // always

        if (href0 == null) {
            try {
                href0 = Cambridge.getHref0(wordStr, flag);
            } catch (IOException e) {
                Cambridge.Result r = new Cambridge.Result();
                r.wordStr = wordStr;
                r.ErrorMessageList = Cambridge.getStackTrace(e);
                return r;
            }
        }
        if (href0 == null) {
            Cambridge.Result r = new Cambridge.Result();
            r.wordStr = wordStr;
            r.WarnMessage = wordStr + ": no luck";
            return r;
        }
        if (href0.contains("spellcheck")) {
            Cambridge.Result r = new Cambridge.Result();
            try {
                r.wordStr = wordStr;
                r.tryWordList = Cambridge.spellCheck(href0);
            } catch (IOException e) {
                r.ErrorMessageList = Cambridge.getStackTrace(e);
            }
            return r;
        }
        // sanitize href0
        int index = href0.indexOf("?"); // index == -1: URL given, index > -1: word given
        href0 = index == -1 ? href0 : href0.substring(0, index);

        // try to get info from database with href0
        String JSONStr = mCambridgeDbHelper.select0(href0, flag.showAll);
//Log.i("DB", JSONStr == null ? "null" : JSONStr);

        Cambridge.Result    result = null;
        int nAddedPage = 0;
        if (JSONStr == null) {
            try {
                result = Cambridge.getResultFromWeb(href0, flag);
            } catch (IOException e) {
                Cambridge.Result r = new Cambridge.Result();
                r.wordStr = wordStr;
                r.ErrorMessageList = Cambridge.getStackTrace(e);
                return r;
            }
        } else {
            Json.BaseType  json = Json.setJson(JSONStr);
            result = new Cambridge.Result();
            result.setDoc(json, flag.showAll);
            for (Cambridge.Result.Page page: result.doc) {
                if (page.content.size() == 0) { // not filled yet, i.e., no record in "Web" table
                    try {
                        ++nAddedPage;
                        Cambridge.lookup0(flag, page);
                    } catch (IOException e) {
                        Cambridge.Result r = new Cambridge.Result();
                        r.wordStr = wordStr;
                        r.ErrorMessageList = Cambridge.getStackTrace(e);
                        return r;
                    }
                }
            }
        }
        if (JSONStr == null || nAddedPage > 0) {
//Log.i("DB", "insert...");
            insert(result, flag.force);
        }
        result.wordStr = wordStr;
        for (Cambridge.Result.Page page: result.doc) {
            for (Cambridge.Block block: page.content) {
                WordSmartContract.WordSmartJson wordSmartJson = mWordSmartDbHelper.select(block.head.word);
                String jsonStrWordSmart = null;
                try {
                    jsonStrWordSmart = JsonWalk.walkClass(WordSmartContract.WordSmartJson.class, wordSmartJson, "    ");
                }
                catch (ClassNotFoundException e) {e.printStackTrace();}
                catch (IllegalAccessException e) {e.printStackTrace();}
                if (jsonStrWordSmart == null)
                    continue;
                int n = 0;
                n += wordSmartJson.theWordsList.size();
                n += wordSmartJson.theHitParadeList.size();
                n += wordSmartJson.rootList.size();
                if (n > 0) {
                    block.jsonStrWordSmart = jsonStrWordSmart;
                }
            }
        }
        return result;
    }

    @Override
    protected void onPostExecute(Cambridge.Result result) {
        if (!DickA.EMULATOR)
            mArgument.mActivity.setProgressBarIndeterminateVisibility(false);
        done(result);
        super.onPostExecute(result);
    }

    private void done(Cambridge.Result result)
    {
        TextView    mTextView   = mArgument.mTextView;
        Resources   r = mArgument.mActivity.getApplicationContext().getResources();
        if (result == null) {
            return;
        }
        if (errorMessageArray != null) {
            Dispatcher.displayErrorMessage(mTextView, r, errorMessageArray);
            return;
        }

        String wordStr = result.wordStr;
        boolean special =
            wordStr.startsWith("!") ||
            wordStr.startsWith("@") ||
            wordStr.startsWith("#") ||  // #3.4
            wordStr.contains("%") ||
            wordStr.toUpperCase().equals(wordStr)
        ;
        if (special || mWordSmartOnly) {
            String jsonStrWordSmart = result.WarnMessage;
            if (
                !wordStr.startsWith("@") &&
                !wordStr.startsWith("#") &&
                !wordStr.contains("%") &&
                jsonStrWordSmart != null
            ) {
                Intent intent = new Intent(mArgument.mActivity.getApplicationContext(), TabActivity.class);
                intent.putExtra(DickA.EXTRA_MESSAGE_ROOT_PATH_STR, mArgument.mRootPath);
                intent.putExtra(DickA.EXTRA_MESSAGE_WORD_STR, wordStr.startsWith("!") ? "!" : "");
                intent.putExtra(DickA.EXTRA_MESSAGE_JSON_STR, jsonStrWordSmart);
                mArgument.mActivity.startActivity(intent);
                return;
            }
            mTextView.setMovementMethod(LinkMovementMethod.getInstance());
            mTextView.setText("");
            mTextView.scrollTo(0, 0);
            mTextView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
            mTextView.setTextColor(r.getColor(R.color.text));
            if (wordStr.startsWith("#") && jsonStrWordSmart != null) {
                mTextView.setText(jsonStrWordSmart);
                return;
            }
            List<String> word_a = result.ErrorMessageList;
            if (word_a == null || word_a.isEmpty()) {
                mTextView.setText("\"" + wordStr + "\": no match");
                return;
            }
            boolean isAt = wordStr.contains("@");   // "ROOT|SENSE|WORD"
            String  root = null;
            for (String word: word_a) {
                String[] a;
                if (isAt) {
                    a = word.split("\\|");
                    if (root == null || !root.equals(a[0])) {
                        root = a[0];
                        if (root != null)
                            mTextView.append("\n");
                        Spannable rootSpan = new SpannableString(root);
                        rootSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, root.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        mTextView.append(rootSpan);
                        mTextView.append(" ");
                        Spannable defSpan = new SpannableString(a[1]);
                        defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, a[1].length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        mTextView.append(defSpan);
                        mTextView.append("\n");
                    }
                    word = a[2];
                }
                mTextView.append("    ");
                // {
                Spannable wordSpan = new SpannableString(word);
                final String    w = wordStr.startsWith("!") ? "!!" + word : word.toLowerCase();
                final Argument  argument = mArgument;
                ClickableSpan lookupSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View mTextView) {
                        new Dispatcher(argument, true).execute(w);
                    }
                };
                wordSpan.setSpan(lookupSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                mTextView.append(wordSpan);
                // }
                mTextView.append(" \n");    // to make a smooth scroll
            }
            return;
        }

        if (result.ErrorMessageList != null) {
            Dispatcher.displayErrorMessage(mTextView, r, result.ErrorMessageList);
            return;
        }
        if (result.WarnMessage != null) {
            mTextView.setText(result.WarnMessage);
            return;
        }

        if (!result.tryWordList.isEmpty()) {
            mTextView.setMovementMethod(LinkMovementMethod.getInstance());
            mTextView.setText("");
            mTextView.scrollTo(0, 0);
            mTextView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
            mTextView.setTextColor(r.getColor(R.color.text));
            String  str = "WARN: " + wordStr + " not found\n";
            Spannable warnSpan = new SpannableString(str);
            warnSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.warn)), 0, 4, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            warnSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 6, 6 + wordStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            mTextView.append(warnSpan);
            for (String word: result.tryWordList) {
                mTextView.append("    ");
                // {
                Spannable wordSpan = new SpannableString(word);
                final String    w = word;
                final Argument  argument = mArgument;
                ClickableSpan lookupSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View mTextView) {
                        new Dispatcher(argument, true).execute(w);
                    }
                };
                wordSpan.setSpan(lookupSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                mTextView.append(wordSpan);
                // }
                mTextView.append(" \n");    // to make a smooth scroll
            }
            return;
        }

        String  JSONStr = result.getJSONStr();
        if (mNewIntent) {
            Intent intent = new Intent(mArgument.mActivity.getApplicationContext(), TabActivity.class);
            intent.putExtra(DickA.EXTRA_MESSAGE_ROOT_PATH_STR, mArgument.mRootPath);
            intent.putExtra(DickA.EXTRA_MESSAGE_WORD_STR, wordStr);
            intent.putExtra(DickA.EXTRA_MESSAGE_JSON_STR, JSONStr);
            mArgument.mActivity.startActivity(intent);
            return;
        }

        Dispatcher.paint(mArgument, wordStr, JSONStr);
    }

    public void insert(final Cambridge.Result result, final boolean force)
    {
        new Thread(new Runnable() {
            public void run() {
                mCambridgeDbHelper.insert(result, force);
            }
        }).start();
    }

    public static void play(final String audioPath, final String audioURLStr)
    {
        if (audioURLStr == null || audioURLStr.isEmpty())
            return;
        new Thread(new Runnable() {
            public void run() {
                if (audioPath == null) {
                    return;
                }
//Log.i("MP3", audioPath);
//Log.i("MP3", audioURLStr.substring(audioURLStr.lastIndexOf('/') + 1));
                File f = new File(audioPath, audioURLStr.substring(audioURLStr.lastIndexOf('/') + 1));
                //Log.i("FILE", f.toString());
                // check file: exist and non-zero
                if (!f.isFile() || f.length() == 0) {   // download
                    URL audioURL = null;
                    try {
                        audioURL = new URL(audioURLStr);
                    } catch (java.net.MalformedURLException e) {
                        e.printStackTrace();
                        audioURL = null;
                    }
                    if (audioURL == null) {
                        return;
                    }
                    try {
                        BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(f));
                        URLConnection ucon = audioURL.openConnection();
                        InputStream is = ucon.getInputStream();
                        BufferedInputStream bis = new BufferedInputStream(is);
                        int b;
                        while ((b = bis.read()) != -1) {
                            bos.write(b);
                        }
                        bos.close();
                    } catch (java.io.IOException e) {
                        e.printStackTrace();
                        return;
                    }
                }
                try {
                    MediaPlayer mediaPlayer = new MediaPlayer();
                    mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
                    //mediaPlayer.setDataSource(audioURLStr);
                    FileInputStream fis = new FileInputStream(f);
                    mediaPlayer.setDataSource(fis.getFD());
                    mediaPlayer.prepareAsync();
                    mediaPlayer.setOnPreparedListener(new android.media.MediaPlayer.OnPreparedListener() {
                        @Override
                        public void onPrepared(MediaPlayer mp) {
                            mp.start();
                        }
                    });
                    mediaPlayer.setOnErrorListener(new android.media.MediaPlayer.OnErrorListener() {
                        @Override
                        public boolean onError(MediaPlayer mp, int what, int extra) {
                            return false;
                        }
                    });
                } catch (IOException e) {
                    e.printStackTrace();
                    return;
                }
            }
        }).start();
    }

    public static void paint(Argument argument, String wordStr, String JSONStr)
    {
        TextView    textView = argument.mTextView;
        Resources   r = argument.mActivity.getApplicationContext().getResources();

        Json.BaseType  json = Json.setJson(JSONStr);
        Cambridge.Result    result = new Cambridge.Result();
        result.setDoc(json, false);
        result.wordStr = wordStr;

        // play in another thread
        Dispatcher.play(argument.mRootPath + "/ogg/", Cambridge.getAudioURL(result));

        textView.setMovementMethod(LinkMovementMethod.getInstance());
        textView.setText("");
        textView.scrollTo(0, 0);
        textView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
        textView.setTextColor(r.getColor(R.color.text));
        for (Cambridge.Result.Page page: result.doc) {
            for (Cambridge.Block block: page.content) {
                Cambridge.Block.Head    head = block.head;
                String word = head.word;
                final String audioPath = argument.mRootPath + "/ogg/";
                final String audioURL = head.getAudioURL();
                Spannable wordSpan = new SpannableString(word);
                if (audioURL != null) {
                    ClickableSpan playSpan = new ClickableSpan() {
                        @Override
                        public void onClick(View view) {
                            Dispatcher.play(audioPath, audioURL);
                        }
                    };
                    wordSpan.setSpan(playSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                }
                wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, head.word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(wordSpan);
                if (!head.ipa.isEmpty()) {
                    textView.append(" ");
                    int i = 0;
                    for (String s: head.ipa) {
                        if (i++ != 0) {
                            Spannable commaSpan = new SpannableString(",");
                            commaSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                            textView.append(commaSpan);
                        }
                        s = "/" + s + "/";
                        Spannable ipaSpan = new SpannableString(s);
                        ipaSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.ipa)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(ipaSpan);
                    }
                }
                if (!head.pos.isEmpty()) {
                    textView.append(" ");
                    Spannable LSpan = new SpannableString("(");
                    LSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.pos)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(LSpan);
                    int i = 0;
                    for (String s: head.pos) {
                        if (i++ != 0) {
                            Spannable commaSpan = new SpannableString(",");
                            commaSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                            textView.append(commaSpan);
                        }
                        Spannable posSpan = new SpannableString(s);
                        posSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.pos)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(posSpan);
                    }
                    Spannable RSpan = new SpannableString(")");
                    RSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.pos)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(RSpan);
                }
                if (head.gw != null && !head.gw.isEmpty()) {
                    textView.append(" ");
                    String s = "[" + head.gw + "]";
                    Spannable gwSpan = new SpannableString(s);
                    gwSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.gw)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(gwSpan);
                }
                if (block.jsonStrWordSmart != null) {
                    textView.append(" ");
                    Spannable spanWordSmart = new SpannableString("<WS>");
                    final Argument  arg = argument;
                    final String    jsonStr = block.jsonStrWordSmart;
                    ClickableSpan lookupSpan = new ClickableSpan() {
                        @Override
                        public void onClick(View view) {
                            Intent intent = new Intent(arg.mActivity.getApplicationContext(), TabActivity.class);
                            intent.putExtra(DickA.EXTRA_MESSAGE_ROOT_PATH_STR, arg.mRootPath);
                            intent.putExtra(DickA.EXTRA_MESSAGE_WORD_STR, "");
                            intent.putExtra(DickA.EXTRA_MESSAGE_JSON_STR, jsonStr);
                            arg.mActivity.startActivity(intent);
                        }
                    };
                    spanWordSmart.setSpan(lookupSpan, 0, 4, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    spanWordSmart.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, 4, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(spanWordSmart);
                }
                textView.append("\n");
                for (Cambridge.Block.Body body: block.bodyList) {
                    if (body.def != null && !body.def.isEmpty()) {
                        String s = body.def;
                        Spannable defSpan = new SpannableString(s);
                        defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(defSpan);
                        textView.append("\n");
                    }
                    for (String e: body.example) {
                        textView.append("    ");
                        Spannable dotSpan = new SpannableString("·");
                        dotSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.example_dot)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        textView.append(dotSpan);
                        textView.append(" ");
                        Spannable exSpan = new SpannableString(e);
                        exSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.example)), 0, e.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
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
                textView.append("\n");
                String s = "more:";
                Spannable moreSpan = new SpannableString(s);
                moreSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
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
                final String href = info.href;
                final Argument  arg = argument;
                ClickableSpan hrefSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View view) {
                        new Dispatcher(arg, true).execute(href);
                    }
                };
                Spannable linkSpan = new SpannableString(linkStr);
                linkSpan.setSpan(hrefSpan, 0, linkStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                //linkSpan.setSpan(hrefSpan, 0, index, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                int indexEnd = info.word.length();
                linkSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, indexEnd, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                if (info.pos != null && !info.pos.isEmpty()) {
                    int index = indexEnd + 1 + 1 + info.pos.length() + 1;
                    linkSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.pos)), indexEnd + 1, index, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    indexEnd = index;
                }
                if (info.gw != null && !info.gw.isEmpty()) {
                    int index = indexEnd + 1 + 1 + info.gw.length() + 1;
                    linkSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.gw)), indexEnd + 1, index, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    indexEnd = index;
                }
                textView.append(linkSpan);
                textView.append(" \n");
            }
        }
    }
    public static void paint(final Argument argument, final String jsonStrWordSmart)
    {
        TextView    textView = argument.mTextView;
        Resources   r = argument.mActivity.getApplicationContext().getResources();
        WordSmartContract.WordSmartJson wordSmartJson = null;
        String[] ema = null;
        try {
            wordSmartJson = WordSmartContract.setWordSmart(jsonStrWordSmart);
        }
        catch (ClassNotFoundException e) {ema = Dispatcher.getErrorMessageArray(e);}
        catch (InstantiationException e) {ema = Dispatcher.getErrorMessageArray(e);}
        catch (IllegalAccessException e) {ema = Dispatcher.getErrorMessageArray(e);}
        if (wordSmartJson == null) {
            Dispatcher.displayErrorMessage(textView, r, ema);
            return;
        }
        textView.setMovementMethod(LinkMovementMethod.getInstance());
        textView.setText("");
        textView.scrollTo(0, 0);
        textView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
        textView.setTextColor(r.getColor(R.color.text));
        int n;
        n = 0;
        List<WordSmartContract.WordSmartJson.TheWords> theWordsList = wordSmartJson.theWordsList;
        if (theWordsList.size() > 0) {
            n = theWordsList.size();
            int i = 0;
            int k = 0;
            for (WordSmartContract.WordSmartJson.TheWords theWords: theWordsList) {
                if (i++ > 0)
                    textView.append("\n");
                int iBook = (new Integer(theWords.book_no)).intValue();
                int iUnit = (new Integer(theWords.unit)).intValue();
                String  word = theWords.word;
                Spannable wordSpan = new SpannableString(word);
                final String    w = word;
                ClickableSpan lookupSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View mTextView) {
                        new Dispatcher(argument, true).execute(w.toLowerCase());
                    }
                };
                wordSpan.setSpan(lookupSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(wordSpan);
                textView.append(" ");
                String  pron = "/" + theWords.pron + "/";
                Spannable ipaSpan = new SpannableString(pron);
                ipaSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.ipa)), 0, pron.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(ipaSpan);
                textView.append(" ");
                String  pos = "(" + theWords.pos + ")";
                Spannable posSpan = new SpannableString(pos);
                posSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.pos)), 0, pos.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(posSpan);
                textView.append(" ");
                String  sense = theWords.sense;
                Spannable defSpan = new SpannableString(sense);
                defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, sense.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(defSpan);
                textView.append(" ");
                String  bookStr;
                bookStr = String.format("Word Smart %s", iBook == 1 ? "I" : (iBook == 2 ? "II" : "for the GRE"));
                String unitStr = "<#" + iUnit + ">";
                Spannable infoSpan = new SpannableString("(" + bookStr + " " + unitStr + ")");
                final String lookupUnitStr = "#" + iBook + "." + iUnit;
                ClickableSpan lookupUnitSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View mTextView) {
                        new Dispatcher(argument, true).execute(lookupUnitStr);
                    }
                };
                infoSpan.setSpan(lookupUnitSpan, 1 + bookStr.length() + 1, 2 + bookStr.length() + unitStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                infoSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, 2 + bookStr.length() + 1 + unitStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(infoSpan);
                textView.append("\n");
                for (String e: theWords.example) {
                    //textView.append(s + "\n");
                    // searching "___xxx___"
                    List<Integer>    b = new ArrayList<Integer>();
                    StringBuilder   str = new StringBuilder();
                    int start = 0;
                    int index;
                    int INDEX = 0;  // length of accumulated "___"
                    while ((index = e.indexOf("___", start)) != -1) {
                        b.add(new Integer(index - INDEX));
                        str.append(e.substring(start, index));
                        start = index + 3;
                        INDEX += 3;
                    }
                    str.append(e.substring(start));
                    String  s = str.toString();
                    Spannable exSpan = new SpannableString(s);
                    if (b.size() % 2 == 1) {
                        Log.e("___", "unbalanced ___");
                        exSpan = new SpannableString(s + "ERR: unbalanced ___");
                    } else {
                        for (int j = 0; j < b.size(); ++j)
                            exSpan.setSpan(new StyleSpan(Typeface.ITALIC), b.get(j++), b.get(j), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        exSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.example)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    }
                    textView.append(exSpan);
                    textView.append("\n");
                }

                if (++k == WordSmartContract.TheWords.LIMIT)
                    break;
            }
            if (theWordsList.size() > WordSmartContract.TheWords.LIMIT) {
                textView.append("\nShow More (not implemented yet)\n");
            }
        }
        if (n > 0)
            textView.append("\n\n");
        List<WordSmartContract.WordSmartJson.TheHitParade> theHitParadeList = wordSmartJson.theHitParadeList;
        n = 0;
        if (theHitParadeList.size() > 0) {
            n = theHitParadeList.size();
            for (WordSmartContract.WordSmartJson.TheHitParade theHitParade: theHitParadeList) {
                Integer type = new Integer(theHitParade.type);
                String  info = String.format("(The %s Hit Parade)", type.intValue() == 1 ? "SAT" : "GRE");
                Integer book = new Integer(theHitParade.book);
                String  word = theHitParade.word;
                String  sense = theHitParade.sense;
                Spannable wordSpan = new SpannableString(word);
                final String    w = word;
                ClickableSpan lookupSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View mTextView) {
                        new Dispatcher(argument, true).execute(w.toLowerCase());
                    }
                };
                wordSpan.setSpan(lookupSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(wordSpan);
                textView.append(" ");
                Spannable defSpan = new SpannableString(sense);
                defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, sense.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(defSpan);
                textView.append(" ");
                Spannable infoSpan = new SpannableString(info);
                infoSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, info.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(infoSpan);
                textView.append("\n");
            }
        }
        if (n > 0)
            textView.append("\n\n");
        List<WordSmartContract.WordSmartJson.Root> rootList = wordSmartJson.rootList;
        n = 0;
        if (rootList.size() > 0) {
            n = rootList.size();
            int i = 0;
            for (WordSmartContract.WordSmartJson.Root root: rootList) {
                if (i++ > 0)
                    textView.append("\n");
                String  rootStr = root.root;
                String  sense = root.sense;
                Spannable rootSpan = new SpannableString(rootStr);
                rootSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, rootStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(rootSpan);
                textView.append(" ");
                Spannable defSpan = new SpannableString(sense);
                defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, sense.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(defSpan);
                textView.append("\n");
                for (String word: root.wordList) {
                    Spannable wordSpan = new SpannableString(word);
                    final String    w = word;
                    ClickableSpan lookupSpan = new ClickableSpan() {
                        @Override
                        public void onClick(View mTextView) {
                            new Dispatcher(argument, true).execute(w);
                        }
                    };
                    wordSpan.setSpan(lookupSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append("    ");
                    textView.append(wordSpan);
                    textView.append(" \n");
                }
            }
        }
    }
    public static void paint(final Argument argument, int salt, final String jsonStr)
    {
        TextView textView = argument.mTextView;
        Resources r = argument.mActivity.getApplicationContext().getResources();
        textView.setMovementMethod(LinkMovementMethod.getInstance());
        textView.setText("");
        textView.scrollTo(0, 0);
        textView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
        textView.setTextColor(r.getColor(R.color.text));

////////{
        Json.BaseType jsonType = Json.setJson(jsonStr);
        NationalCancerInstituteContract.NationalCancerInstituteJson nationalCancerInstituteJson = null;
        String[] ema = null;
        try {
            nationalCancerInstituteJson = (NationalCancerInstituteContract.NationalCancerInstituteJson) JsonWalk.setWalkClassJson(NationalCancerInstituteContract.NationalCancerInstituteJson.class, jsonType, "    ");
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {ema = Dispatcher.getErrorMessageArray(e);}
        catch (IllegalAccessException e) {ema = Dispatcher.getErrorMessageArray(e);}
        if (nationalCancerInstituteJson == null) {
            Dispatcher.displayErrorMessage(textView, r, ema);
            return;
        }
////////}
        boolean played = false;
        for (NationalCancerInstituteContract.NationalCancerInstituteJson.TermsDef termsDef: nationalCancerInstituteJson.termsDefList) {
            String src = termsDef.src;
            String terms = termsDef.terms;
            final String mp3 = termsDef.mp3;
            String mtd = termsDef.mtd;
            String def = termsDef.def;
            if (!played && !mp3.isEmpty()) {
                Dispatcher.play(argument.mRootPath + "/mp3/", mp3);
                played = true;
            }

            Spannable termsSpan = new SpannableString(terms);
            if (!mp3.isEmpty()) {
                final String audioPath = argument.mRootPath + "/mp3/";
                ClickableSpan playSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View view) {
                        Dispatcher.play(audioPath, mp3);
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
    }
}
