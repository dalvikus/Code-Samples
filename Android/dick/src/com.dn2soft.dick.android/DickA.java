package com.dn2soft.dick.android;

import android.util.Log;

import android.app.Activity;
import android.os.Bundle;
import android.os.Build;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;

import com.dn2soft.dick.android.Dispatcher;

// test Wi-Fi
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.content.Context;
import android.content.DialogInterface;
// show Dialog
import android.app.AlertDialog;
import android.view.Gravity;
import android.graphics.Color;
// path
import java.io.File;
import android.os.Environment;

import android.content.res.Resources;
import android.text.style.ForegroundColorSpan;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.Spanned;
public class DickA extends Activity
{
    public final static boolean EMULATOR = Build.PRODUCT.equals("sdk");
    public final static boolean WORD_SMART_ONLY = true; // in case that there are no available networks
    public final static String EXTRA_MESSAGE_ROOT_PATH_STR = "com.dn2soft.dick.android.MESSAGE.ROOT_PATH_STR";
    public final static String EXTRA_MESSAGE_AUDIO_PATH_STR = "com.dn2soft.dick.android.MESSAGE.AUDIO_PATH_STR";
    public final static String EXTRA_MESSAGE_WORD_STR = "com.dn2soft.dick.android.MESSAGE.WORD_STR";
    public final static String EXTRA_MESSAGE_JSON_STR = "com.dn2soft.dick.android.MESSAGE.JSON_STR";
    private static final String TAG = "DickA";

    Dispatcher.Argument mArgument;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        if (!DickA.EMULATOR) {
            // Activity.requestWindowFeature()
            //  This is a convenience for calling getWindow().requestFeature().
            // Window.requestFeature()
            //  This must be called before setContentView()
            requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        }

        setContentView(R.layout.main);

        if (!EMULATOR) {
            DickA.getWiFiInfoStr(this, true);
        }

        String rootPath = getRootPath();
        mArgument = new Dispatcher.Argument(
            (Activity) this,
            (TextView) findViewById(R.id.display),
            rootPath
        );

        usage();
    }
    private void usage()
    {
        TextView    tv = mArgument.mTextView;
        Resources   r = mArgument.mActivity.getApplicationContext().getResources();
        tv.setText("");
        tv.scrollTo(0, 0);
        tv.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
        tv.setTextColor(r.getColor(R.color.text));

        tv.append("normally, type ");
        Spannable span = new SpannableString("word");
        span.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, 4, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        tv.append(span); tv.append(" to be looked up\n");
        tv.append("\n");

        tv.append("Word Smart: use ");
        span = new SpannableString("%");
        span.setSpan(new ForegroundColorSpan(r.getColor(R.color.error)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        tv.append(span);
        tv.append(" for search\n");
        tv.append("\tstr%: find word starting with \"str\"\n");
        tv.append("\t%str: find word ending with \"str\"\n");
        tv.append("\t%str%: find word containing \"str\"\n");
        tv.append("\n");

        tv.append("Word Smart: prepend ");
        span = new SpannableString("#");
        span.setSpan(new ForegroundColorSpan(r.getColor(R.color.error)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        tv.append(span); tv.append(" to find all words in section of a book\n");
        tv.append("\t#1.2: find all words in section 2 in Word Smart I\n");
        tv.append("\n");

        tv.append("Word Smart: prepend ");
        span = new SpannableString("@");
        span.setSpan(new ForegroundColorSpan(r.getColor(R.color.error)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        tv.append(span); tv.append(" to find root");
        tv.append("\t@[%]str[%]: find root(s) starting/ending/containing \"str\"\n");
        tv.append("\n");

        tv.append("Cancer Terms: prepend ");
        span = new SpannableString("!");
        span.setSpan(new ForegroundColorSpan(r.getColor(R.color.error)), 0, 1, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        tv.append(span);
        tv.append("\n");
        tv.append("\t![%]str[%]: find term(s) starting/ending/containing \"str\"\n");
    }

    private String getRootPath()
    {
        File rootPath;
        String state = Environment.getExternalStorageState();
        if (!Environment.MEDIA_MOUNTED.equals(state)) {
            // internal storage
            rootPath = new File(getApplicationContext().getFilesDir(), "dick");
        } else {
            // external storage
            rootPath = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "dick");
            if (rootPath.isFile()) {
                Log.e("PATH", "not a directory: |" + rootPath.getAbsolutePath() + "|");
                rootPath = new File(getApplicationContext().getFilesDir(), "dick");
            }
        }
        if (rootPath == null) {
            Log.e("PATH", "Root Path is null");
        } else {
            if (!rootPath.isDirectory() && !rootPath.mkdirs()) {
                Log.e("PATH", "failed to create a path(|" + rootPath.getAbsolutePath() + "|)");
                rootPath = null;
            }
        }
        return rootPath.toString();
    }
    private String getAudioPath(final String rootPath)
    {
        if (rootPath == null) {
            Log.e("PATH", "No Root Path");
            Log.e("PATH", "No Audio Path");
            return null;
        }
        File audioPath = new File(rootPath, "ogg");
        if (audioPath != null && audioPath.isFile()) {
            Log.e("PATH", "|" + audioPath.getAbsolutePath() + "|: Not a directory");
            // try internal storage assuming that rootPath is external onoe
            audioPath = new File(getApplicationContext().getFilesDir(), "dick/ogg");
            if (audioPath != null && audioPath.isFile()) {    // is it possible?
                Log.e("PATH", "|" + audioPath.getAbsolutePath() + "|: Not a directory");
                audioPath = null;
            }
        } else if (audioPath != null && !audioPath.isDirectory() && !audioPath.mkdirs()) {
            Log.e("PATH", "failed to create a path(|" + audioPath.getAbsolutePath() + "|)");
            audioPath = null;
        }
        return audioPath.toString();
    }

    public void lookup(View view)
    {
        EditText editText = (EditText) findViewById(R.id.word);
        String wordStr = editText.getText().toString();
        if (wordStr.isEmpty())
            return;
        new Dispatcher(mArgument, true).execute(wordStr);
    }
    public static String getWiFiInfoStr(final Activity activity, boolean showDialog)
    {
        ConnectivityManager connManager = (ConnectivityManager) activity.getSystemService(Context.CONNECTIVITY_SERVICE);
        //NetworkInfo nw = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        NetworkInfo nw = connManager.getActiveNetworkInfo();
        String msg = "";
        if (nw == null) {
            msg = "getActiveNetworkInfo() returns |null|";
        } else if (nw.getType() != ConnectivityManager.TYPE_WIFI) {
            msg = "WiFi only: " + nw.toString();
        } else if (!nw.isAvailable()) {
            msg = "network is not available: " + nw.toString();
        } else if (!nw.isConnected()) {
            msg = "network is not connected: " + nw.toString();
        } else if (nw.isRoaming()) {
            msg = "roaming is set: " + nw.toString();
        }
        if (!msg.isEmpty() && showDialog) {
            AlertDialog.Builder builder = new AlertDialog.Builder(activity);
            builder.setNeutralButton(R.string.ok, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    if (!WORD_SMART_ONLY)
                        activity.finish();
                }
            });
            TextView title = new TextView(activity);
            title.setText(msg + (WORD_SMART_ONLY ? "\nUse Word Smart Only" : ""));
            title.setBackgroundColor(Color.DKGRAY);
            title.setPadding(10, 10, 10, 10);
            title.setGravity(Gravity.CENTER);
            title.setTextColor(Color.WHITE);
            title.setTextSize(20);
            builder.setCustomTitle(title);
            AlertDialog alertDiag = builder.create();
            alertDiag.show();
        }
        return msg;
    }
}
