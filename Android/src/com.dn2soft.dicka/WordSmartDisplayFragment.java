package com.dn2soft.dicka;

import android.view.ActionMode;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ClickableSpan;
import android.text.style.ForegroundColorSpan;
import android.text.style.StyleSpan;
import android.graphics.Typeface;

import android.app.Activity;
import android.text.method.LinkMovementMethod;
import android.content.res.Resources;
import android.util.Log;
import android.content.Context;

import java.util.List;
import java.util.ArrayList;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import android.view.Menu;
import android.view.MenuItem;

public class WordSmartDisplayFragment extends Fragment {
    private static final String TAG = "WordSmartDisplayFragment";
    public static final String ARG_OBJECT = "Word Smart: The Words";

    private static ActionMode mActionMode = null;
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
        final Activity  arg_activity = getActivity();
        String theWordsJsonStr = args.getString(ARG_OBJECT);
////    Log.i(TAG, "theWordsJsonStr: |" + theWordsJsonStr + "|");
        WordSmartContract.WordSmartJson.TheWords theWords = WordSmartContract.setTheWords(theWordsJsonStr);
/*
{
        StringBuilder   sb = new StringBuilder();
        int iBook = (new Integer(theWords.book_no)).intValue();
        sb.append(String.format(
            "%s (%s) %s %s (%s)%n"
            ,
            theWords.word,
            theWords.pron,
            theWords.pos,
            theWords.sense,
            iBook == 3
                ? "Word Smart for the GRE"
                : String.format("Word Smart %s <%s>", iBook == 1 ? "I" : "II", theWords.unit)
        ));
        sb.append(theWords.examples);
        Log.i(TAG, "|" + sb.toString());
}
 */

        Resources   r = ((Context) getActivity()).getResources();
        TextView textView = (TextView) rootView.findViewById(R.id.display);
////    textView.setMovementMethod(LinkMovementMethod.getInstance());
/*
public void setTextIsSelectable (boolean selectable)

When you call this method to set the value of textIsSelectable, it sets the flags focusable, focusableInTouchMode, clickable, and longClickable to the same value. These flags correspond to the attributes android:focusable, android:focusableInTouchMode, android:clickable, and android:longClickable. To restore any of these flags to a state you had set previously, call one or more of the following methods: setFocusable(), setFocusableInTouchMode(), setClickable() or setLongClickable().
 */
        textView.setTextIsSelectable(true);
//      textView.setFocusable(true);
//      textView.setFocusableInTouchMode(true);
/*
public void setCustomSelectionActionModeCallback (ActionMode.Callback actionModeCallback)

Note that text selection mode is not started when a TextView receives focus and the selectAllOnFocus flag has been set. The content is highlighted in that case, to allow for quick replacement.
 */
        ActionMode.Callback callback = textView.getCustomSelectionActionModeCallback();
        final TextView tv = textView;
        final int SEARCH = 0;
        final ActionMode.Callback actionModeCallback = new ActionMode.Callback() {
            @Override
            public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
                return false;
/*
Log.i(TAG, "onPrepareActionMode");
//              if (true)
//                  return false;
Log.i(TAG, "# of menus = " + menu.size());
                for (int i = 0; i < menu.size(); ++i) {
Log.i(TAG, "menu[" + i + "]: |" + menu.getItem(i).getTitle() + "|");
                }
                menu.clear();
                // Remove the "select all" option
                menu.removeItem(android.R.id.selectAll);
                // Remove the "cut" option
                menu.removeItem(android.R.id.cut);
                // Remove the "copy all" option
                menu.removeItem(android.R.id.copy);
                return true;
 */
            }

            @Override
            public boolean onCreateActionMode(ActionMode mode, Menu menu) {
                menu.clear();
//MenuInflater inflater = mode.getMenuInflater();
//inflater.inflate(R.menu.style, menu);
                // Called when action mode is first created. The menu supplied
                // will be used to generate action buttons for the action mode

                // Here is an example MenuItem
                menu.add(0, SEARCH, 0, "Search");//.setIcon(R.drawable.ic_action_book);
                return true;
            }

            @Override
            public void onDestroyActionMode(ActionMode mode) {
                // Called when an action mode is about to be exited and
                // destroyed
                mActionMode = null;
            }

            @Override
            public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
                switch (item.getItemId()) {
                    case SEARCH:
                        try {
                            if (!tv.isFocused())
                                return true;
                            int selStart = tv.getSelectionStart();
                            int selEnd = tv.getSelectionEnd();
                            CharSequence text = tv.getText();
                            int min, max;
                            if (selStart < selEnd) {
                                min = selStart;
                                max = selEnd;
                            } else {
                                Log.e(TAG, "selStart(" + selStart + ") >= selEnd(" + selEnd + ")");
                                min = selEnd;
                                max = selStart;
                            }
                            if (min < 0) {
                                Log.e(TAG, "min(" + min + ") < 0");
                                min = 0;
                            }
                            int length = text.length();
                            if (max > length) {
                                Log.e(TAG, "max(" + max + ") > length(" + length + ")");
                                max = length;
                            }
                            if (min == max) {
                                Log.e(TAG, "min(" + min + ") = max(" + max + ")");
                                return true;
                            }
                            final CharSequence selectedText = text.subSequence(min, max);
                            String wordStr = selectedText.toString();
                            boolean isFloat;
                            try {
                                Float.parseFloat(wordStr);
                                isFloat = true;
                            } catch (NumberFormatException e) {
                                isFloat = false;
                            }
                            if (isFloat) {
                                // check if wordStr is a form of "<#52>"
                                String unitStr = null;
                                if (
                                    min >= 2 &&
                                    text.charAt(min - 2) == '<' &&
                                    text.charAt(min - 1) == '#' &&
                                    max < length &&
                                    text.charAt(max) == '>'
                                ) {
                                    unitStr = wordStr;
                                    new BookLoad(arg_activity, "#" + unitStr).execute();
                                    return true;
                                }
                                if (unitStr != null) {
                                    // find book_no
                                    //  Word Smart I <#..>
                                    //  Word Smart II <#..>
                                    //  Word Smart for the GRE <#..>
                                }
                                // just a number
                                ////Log.i(TAG, wordStr + ": just a number");
                            }
                            ////Log.i(TAG, "wordStr: |" + wordStr + "|");
                            new Search(arg_activity, wordStr).execute();
                            return true;
                        } finally {
                            mode.finish();
                        }
                    default:
                        break;
                }
                return false;
            }
        };
        textView.setCustomSelectionActionModeCallback(actionModeCallback);
/*
        final String WORD = theWords.word;
        textView.setOnLongClickListener(
            new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View pView) {
                    // Do something when your hold starts here.
                    Log.i(TAG, "hello, world: |" + WORD + "|");
//                  return false;
                    if (mActionMode != null)
                        return false;
                    mActionMode = getActivity().startActionMode(actionModeCallback);
                    tv.setSelected(true);
                    return true;
                }
            }
        );
textView.setOnFocusChangeListener(new OnFocusChangeListener(){
        @Override
        public void onFocusChange(View view, boolean hasFocus) {
Log.i(TAG, "onFocusChange: view = " + view + ", isFocused? " + view.isFocused() + ", hasFocus? " + hasFocus);
        }
});
 */
        textView.setText("");
        textView.scrollTo(0, 0);
        textView.setTextSize(r.getDimension(R.dimen.textsize_DEBUG));
        textView.setTextColor(r.getColor(R.color.text));
        List<Integer[]> ial = new ArrayList<Integer[]>();
////    Log.i(TAG, "|" + theWords.offsets + "|");
        if (!theWords.offsets.isEmpty()) {
/*
            textView.append(theWords.offsets);
            textView.append("\n");
 */
            String sa[] = theWords.offsets.split(" ");
            Integer[] ia = null;
            for (int i = 0; i < sa.length; ++i) {
                int k = (new Integer(sa[i])).intValue();
                if (i % 4 == 0) {
                    ia = new Integer[4];
                    ial.add(ia);
                }
                ia[i % 4] = k;
            }
            sa = null;
            ia = null;
        }
        {
            int iBook = (new Integer(theWords.book_no)).intValue();
            int iUnit = (new Integer(theWords.unit)).intValue();
            String  word = theWords.word;
            Spannable wordSpan = new SpannableString(word);
            final String    arg_w = word;
            ClickableSpan lookupSpan = new ClickableSpan() {
                @Override
                public void onClick(View mTextView) {
                    new Search(arg_activity, arg_w).execute(null);
                }
            };
            wordSpan.setSpan(lookupSpan, 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            wordSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.word)), 0, word.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(wordSpan);
            textView.append("  ");
            String  pron = "/" + theWords.pron + "/";
            Spannable ipaSpan = new SpannableString(pron);
            ipaSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.ipa)), 0, pron.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(ipaSpan);
            textView.append("  ");
            String  pos = "(" + theWords.pos + ")";
            Spannable posSpan = new SpannableString(pos);
            posSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.pos)), 0, pos.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(posSpan);
            textView.append("  ");
/*
            String  sense = theWords.sense;
            Spannable defSpan = new SpannableString(sense);
            defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, sense.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            textView.append(defSpan);
            textView.append("  ");
 */
            {
                int lastIndex = 0;
                String s;
                for (Integer[] ib: ial) {
                    if (ib[0] != 5)
                        continue;
                    s = theWords.sense.substring(lastIndex, ib[2]);
                    Spannable defSpan = new SpannableString(s);
                    defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(defSpan);
                    //textView.append("|" + theWords.examples.substring(lastIndex, ib[2]) + "|\n");
                    lastIndex = ib[2] + ib[3];
                    //textView.append("<<" + theWords.examples.substring(ib[2], lastIndex) + ">>\n");
                    s = theWords.sense.substring(ib[2], lastIndex);
                    Spannable emSpan = new SpannableString(s);
                    emSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(emSpan);
                }
                s = theWords.sense.substring(lastIndex);
                Spannable defSpan = new SpannableString(s);
                defSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.def)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(defSpan);
                textView.append("  ");
            }
            {
                String  bookStr;
                bookStr = String.format("Word Smart %s", iBook == 1 ? "I" : (iBook == 2 ? "II" : "for the GRE"));
                String unitStr = "<#" + iBook + "." + iUnit + ">";
                Spannable bookSpan = new SpannableString("(" + bookStr + " " + unitStr + ")");
                final String lookupUnitStr = "#" + iBook + "." + iUnit;
                ClickableSpan lookupUnitSpan = new ClickableSpan() {
                    @Override
                    public void onClick(View mTextView) {
                        new BookLoad(arg_activity, lookupUnitStr).execute();
                    }
                };
                bookSpan.setSpan(lookupUnitSpan, 1 + bookStr.length() + 1, 2 + bookStr.length() + unitStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                bookSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.gw)), 0, 2 + bookStr.length() + 1 + unitStr.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(bookSpan);
            }
            textView.append("\n");
/*
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
 */
            {
                int lastIndex = 0;
                String s;
                for (Integer[] ib: ial) {
                    if (ib[0] != 6)
                        continue;
                    s = theWords.examples.substring(lastIndex, ib[2]);
                    Spannable exSpan = new SpannableString(s);
                    exSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.example)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(exSpan);
                    //textView.append("|" + theWords.examples.substring(lastIndex, ib[2]) + "|\n");
                    lastIndex = ib[2] + ib[3];
                    //textView.append("<<" + theWords.examples.substring(ib[2], lastIndex) + ">>\n");
                    s = theWords.examples.substring(ib[2], lastIndex);
                    Spannable emSpan = new SpannableString(s);
                    emSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    textView.append(emSpan);
                }
                s = theWords.examples.substring(lastIndex);
                Spannable exSpan = new SpannableString(s);
                exSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.example)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                textView.append(exSpan);
                textView.append("\n");
            }
        }

        return rootView;
    }
}
