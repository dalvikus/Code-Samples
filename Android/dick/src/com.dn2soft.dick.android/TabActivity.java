package com.dn2soft.dick.android;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;
import android.content.Intent;
import android.content.res.Resources;
import android.util.Log;
import com.dn2soft.dick.android.Dispatcher;

public class TabActivity extends Activity
{
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

        Intent intent = getIntent();
        String rootPath = intent.getStringExtra(DickA.EXTRA_MESSAGE_ROOT_PATH_STR);
        String wordStr = intent.getStringExtra(DickA.EXTRA_MESSAGE_WORD_STR);
        String JSONStr = intent.getStringExtra(DickA.EXTRA_MESSAGE_JSON_STR);

/*
        TextView textView = new TextView(this);
        Resources r = getApplicationContext().getResources();
        textView.setTextSize(r.getDimension(R.dimen.textsize));
        textView.setBackgroundResource(R.color.background);
        setContentView(textView);
 */
        setContentView(R.layout.main);
        TextView    textView = (TextView) findViewById(R.id.display);
        //textView.setText(JSONStr);
        mArgument = new Dispatcher.Argument(this, textView, rootPath);
        if (wordStr.isEmpty())
            Dispatcher.paint(mArgument, JSONStr);
        else if (wordStr.equals("!"))
            Dispatcher.paint(mArgument, 1, JSONStr);
        else
            Dispatcher.paint(mArgument, wordStr, JSONStr);
    }
    public void lookup(View view)
    {
        EditText editText = (EditText) findViewById(R.id.word);
        String wordStr = editText.getText().toString();
        new Dispatcher(mArgument, true).execute(wordStr);
    }
}
