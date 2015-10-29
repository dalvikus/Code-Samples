package com.dn2soft.dicka;

import android.util.Log;
import android.content.Context;
import android.util.AttributeSet;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.preference.DialogPreference;

import android.widget.SeekBar;
import android.widget.TextView;
import android.view.View;

class SliderPreference extends DialogPreference implements SeekBar.OnSeekBarChangeListener {
    private static final String TAG = "SliderPreference";

    private static final int    NAME_ID_SHAKE_NONE = -1;
    private static final int    NAME_ID_SHAKE_DELAY = 0;
    private static final int    NAME_ID_SHAKE_DURATION = 1;
    private static final int    NAME_ID_SHAKE_THRESHOLD = 2;
    private int mShakeNameId = NAME_ID_SHAKE_NONE;

    final int mShakeDelayDefaultValue;
    final String mShakeDelayNameStr;
    final int mShakeDurationDefaultValue;
    final String mShakeDurationNameStr;
    final int mShakeThresholdDefaultValue;
    final String mShakeThresholdNameStr;

    private int mCurrentValue, mNewValue;

    SeekBar mSeekBar;
    TextView mProgressText;
    TextView mTrackingText;

    public SliderPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        Resources res = context.getResources();
        mShakeDelayDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeDelay);
        mShakeDelayNameStr = res.getString(R.string.pref_name_shakeDelay);
        mShakeDurationDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeDuration);
        mShakeDurationNameStr = res.getString(R.string.pref_name_shakeDuration);
        mShakeThresholdDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeThreshold);
        mShakeThresholdNameStr = res.getString(R.string.pref_name_shakeThreshold);

        for (int i = 0; i < attrs.getAttributeCount(); ++i) {
            String key = attrs.getAttributeName(i);
            String val  = attrs.getAttributeValue(i);
            int resId = attrs.getAttributeResourceValue(i, -1);
            String name = res.getString(resId);
            if (name.equalsIgnoreCase(mShakeDelayNameStr)) {
                mShakeNameId = NAME_ID_SHAKE_DELAY;
                break;
            } else if (name.equalsIgnoreCase(mShakeDurationNameStr)) {
                mShakeNameId = NAME_ID_SHAKE_DURATION;
                break;
            } else if (name.equalsIgnoreCase(mShakeThresholdNameStr)) {
                mShakeNameId = NAME_ID_SHAKE_THRESHOLD;
                break;
            }
        }

        setDialogLayoutResource(R.layout.slider_preference);
        setPositiveButtonText(android.R.string.ok);
        setNegativeButtonText(android.R.string.cancel);
        setDialogIcon(null);
//Log.i(TAG, "    mShakeThresholdDefaultValue: " + mShakeThresholdDefaultValue);
    }

    @Override
    protected View onCreateDialogView() {
//Log.i(TAG, "onCreateDialogView");
//Log.i(TAG, "    mCurrentValue = " + mCurrentValue);
//Log.i(TAG, "    mNewValue = " + mNewValue);
        View view = super.onCreateDialogView();
        mSeekBar = (SeekBar) view.findViewById(R.id.seek);
        mSeekBar.setOnSeekBarChangeListener(this);
        mProgressText = (TextView) view.findViewById(R.id.progress);
        mTrackingText = (TextView) view.findViewById(R.id.tracking);
        return view;
    }

    @Override 
    protected void onBindDialogView(View v) {
        super.onBindDialogView(v);
//Log.i(TAG, "onBindDialogView");
//Log.i(TAG, "    mCurrentValue = " + mCurrentValue);
//Log.i(TAG, "    mNewValue = " + mNewValue);
        mSeekBar.setProgress(mCurrentValue);
//      mSeekBar.setMax(mMax);
//      mSeekBar.setProgress(mValue);
    }

    @Override
    protected void onDialogClosed(boolean positiveResult) {
        // When the user selects "OK", persist the new value
//Log.i(TAG, "onDialogClosed");
//Log.i(TAG, "    positiveResult? " + positiveResult);
//Log.i(TAG, "    mCurrentValue = " + mCurrentValue);
//Log.i(TAG, "    mNewValue: " + mNewValue);
        if (positiveResult) {
            persistInt(mNewValue);
            mCurrentValue = mNewValue;
        }
    }

    @Override
    protected void onSetInitialValue(boolean restorePersistedValue, Object defaultValue) {
/*
Caution: You cannot use the defaultValue as the default value in the getPersisted*() method, because its value is always null when restorePersistedValue is true.
 */
//Log.i(TAG, "onSetInitialValue");
//Log.i(TAG, "    restorePersistedValue? " + restorePersistedValue);
//Log.i(TAG, "    defaultValue = " + (Integer) defaultValue);
//Log.i(TAG, "    getPersistedInt = " + getPersistedInt(mShakeThresholdDefaultValue));
        if (restorePersistedValue) {
            // Restore existing state
            mCurrentValue = this.getPersistedInt(mShakeThresholdDefaultValue);
        } else {
            // Set default state from the XML attribute
            mCurrentValue = (Integer) defaultValue;
            persistInt(mCurrentValue);
        }
    }

/*
http://developer.android.com/guide/topics/ui/settings.html

...
You must implement this method in order for the system to save the default value in the SharedPreferences.
...
The reason you must implement this method to extract the default value from the attribute is because you must specify a local default value for the attribute in case the value is undefined.
 */
    @Override
    protected Object onGetDefaultValue(TypedArray a, int index) {
//Log.i(TAG, "onGetDefaultValue");
//Log.i(TAG, "    getPersistedInt = " + getPersistedInt(mShakeThresholdDefaultValue));
        return a.getInt(index, mShakeThresholdDefaultValue);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromTouch) {
        mNewValue = progress;
        if (mShakeNameId == NAME_ID_SHAKE_NONE) {
        } else if (mShakeNameId == NAME_ID_SHAKE_DELAY) {
            mProgressText.setText(String.format("%.2f s", ShakeDetector.getDelay(progress)));
        } else if (mShakeNameId == NAME_ID_SHAKE_DURATION) {
            mProgressText.setText(String.format("%.2f s", ShakeDetector.getDuration(progress)));
        } else if (mShakeNameId == NAME_ID_SHAKE_THRESHOLD) {
            mProgressText.setText(String.format("%.2f g", ShakeDetector.getThreshold(progress)));
        }
////    mProgressText.setText(progress + " " + 
////            mNewValue + "=" + fromTouch);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
//      mTrackingText.setText("on");
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
////    mTrackingText.setText("off");
    }
}
