package com.dn2soft.dicka;

////////////////////////////////////////
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;
////////////////////////////////////////

import android.os.Looper;
import android.util.Log;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.widget.Toast;

import android.preference.Preference;
import android.preference.CheckBoxPreference;
import android.preference.SwitchPreference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;

import android.preference.PreferenceManager;
import android.content.SharedPreferences;

import android.hardware.SensorManager;
import android.hardware.Sensor;
import android.content.Intent;

public class SettingsActivity extends Activity implements PlayMusicService.OnStateChangeListener {
    private static final String TAG = "SettingsActivity";

    public static final String PREFS_NAME = "preference";

    // res/values/preferences.xml
    public static final String PREF_CONNECTION_WIFI_ONLY = "wifiOnly";
        // <string name="pref_key_wifiOnly">wifiOnly</string>
    public static final String PREF_PLAY_MUSIC_LOOP = "loop";
        // <string name="pref_key_loop">loop</string>
    public static final String PREF_PLAY_MUSIC_BY_SHAKE = "byShake";
        // <string name="pref_key_byShake">byShake</string>
    public static final String PREF_PLAY_MUSIC_SHAKE_DELAY = "shakeDelay";
        // <string name="pref_key_shakeDelay">shakeDelay</string>
    public static final String PREF_PLAY_MUSIC_SHAKE_DURATION = "shakeDuration";
        // <string name="pref_key_shakeDuration">shakeDuration</string>
    public static final String PREF_PLAY_MUSIC_SHAKE_THRESHOLD = "shakeThreshold";
        // <string name="pref_key_shakeThreshold">shakeThreshold</string>

    private static boolean mWifiOnlyDefaultValue;
    private static boolean mLoopDefaultValue;
    private static boolean mByShakeDefaultValue;
    private static int mShakeDelayDefaultValue;
    private static int mShakeDurationDefaultValue;
    private static int mShakeThresholdDefaultValue;

    private static SensorManager mSensorManager;
    private static Sensor mAccelerometer;
    private static Sensor mGravity;
    private static ShakeDetector mShakeDetector;
    @Override
    public void onResume()
    {
        super.onResume();

        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences((Context) this);
        boolean byShake = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
        if (byShake) {
            mSensorManager.registerListener(mShakeDetector, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);//SENSOR_DELAY_UI);
            mSensorManager.registerListener(mShakeDetector, mGravity, SensorManager.SENSOR_DELAY_UI);
        }
    }
    @Override
    public void onPause() {
        // Add the following line to unregister the Sensor Manager onPause
        super.onPause();

        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences((Context) this);
        boolean byShake = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
        if (byShake) {
            mSensorManager.unregisterListener(mShakeDetector);
        }
    }

////////////////////////////////////////////////////////////////////////////////
// bind to PlayMusicService
    private static PlayMusicService mService;
    boolean mBound = false;
    /** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            PlayMusicService.PlayMusicBinder binder = (PlayMusicService.PlayMusicBinder) service;
            mService = binder.getService();
            mService.registerOnStateChangeListener(SettingsActivity.this);
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            mService.unregisterOnStateChangeListener(SettingsActivity.this);
            mService = null;
            mBound = false;
        }
    };
    @Override
    protected void onStart()
    {
        super.onStart();
        // Bind to LocalService
        Intent intent = new Intent(this, PlayMusicService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }
    @Override
    protected void onStop()
    {
        super.onStop();
        // Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
    }
    @Override
    public void onStateChange(int newState, int oldState, boolean refreshPlayList)
    {
        String msg = PlayMusicService.STATE_STR[oldState] + " -> " + PlayMusicService.STATE_STR[newState];
        Log.i(TAG, "onStateChange: " + msg);
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }
////////////////////////////////////////////////////////////////////////////////

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final Context context = (Context) this;

        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        Resources res = context.getResources();
        mWifiOnlyDefaultValue = res.getBoolean(R.bool.pref_defaultValue_wifiOnly);
        mLoopDefaultValue = res.getBoolean(R.bool.pref_defaultValue_loop);
        mByShakeDefaultValue = res.getBoolean(R.bool.pref_defaultValue_byShake);
        mShakeDelayDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeDelay);
        mShakeDurationDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeDuration);
        mShakeThresholdDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeThreshold);

        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mGravity = mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);
        mShakeDetector = new ShakeDetector(context, mGravity != null);
        mShakeDetector.setOnShakeListener(new ShakeDetector.OnShakeListener() {
            @Override
            public void onShake(int count) {
                Log.i(TAG, "# of shakes = " + count);
                SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
                boolean byShake = sharedPref.getBoolean(SettingsActivity.PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
                if (count > 0 && byShake && mService != null) {
                    mService.set(count == 1 ? PlayMusicService.ACTION_TOGGLE : (count == 2 ? PlayMusicService.ACTION_STOP : PlayMusicService.ACTION_NEXT));
mService.set(count);
                }
            }
        });

        // Display the fragment as the main content.
        getFragmentManager().beginTransaction()
                .replace(android.R.id.content, new SettingsFragment())
                .commit();
    }

    public static class SettingsFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            // Load the preferences from an XML resource
            addPreferencesFromResource(R.xml.preferences);
        }
        @Override
        public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            //Log.i(TAG, "onActivityCreated");
            SharedPreferences sharedPref = getPreferenceManager().getDefaultSharedPreferences((Context) getActivity());
            Activity activity = getActivity();
            Context context = (Context) getActivity();
            Resources res = context.getResources();

Log.i(TAG, "onActivityCreated: Looper.getMainLooper(): " + Looper.getMainLooper());
Log.i(TAG, "onActivityCreated: Looper.myLooper(): " + Looper.myLooper());
            boolean wifiOnly = sharedPref.getBoolean(PREF_CONNECTION_WIFI_ONLY, mWifiOnlyDefaultValue);
//Log.i(TAG, "    mWifiOnlyDefaultValue: " + mWifiOnlyDefaultValue);
//Log.i(TAG, "    wifiOnly: " + wifiOnly);
            Preference wifiPref = (CheckBoxPreference) findPreference(PREF_CONNECTION_WIFI_ONLY);
//Log.i(TAG, "wifiPref: " + wifiPref);
            wifiPref.setSummary(wifiOnly ? R.string.pref_summary_wifiOnly_True : R.string.pref_summary_wifiOnly_False);

            boolean loop = sharedPref.getBoolean(PREF_PLAY_MUSIC_LOOP, mLoopDefaultValue);
//Log.i(TAG, "    mLoopDefaultValue: " + mLoopDefaultValue);
//Log.i(TAG, "    loop: " + loop);
            Preference loopPref = (SwitchPreference) findPreference(PREF_PLAY_MUSIC_LOOP);
//Log.i(TAG, "loopPref: " + loopPref);
            loopPref.setSummary(loop ? R.string.pref_summary_loop_True : R.string.pref_summary_loop_False);

            //mByShakeDefaultValue = res.getBoolean(R.bool.pref_defaultValue_byShake);
            boolean byShake = sharedPref.getBoolean(PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
//Log.i(TAG, "    mByShakeDefaultValue: " + mByShakeDefaultValue);
//Log.i(TAG, "    byShake: " + byShake);
            Preference byShakePref = (SwitchPreference) findPreference(PREF_PLAY_MUSIC_BY_SHAKE);
//Log.i(TAG, "byShakePref: " + byShakePref);
            byShakePref.setSummary(byShake ? R.string.pref_summary_byShake_True : R.string.pref_summary_byShake_False);

            int shakeDelay = sharedPref.getInt(PREF_PLAY_MUSIC_SHAKE_DELAY, mShakeDelayDefaultValue);
//Log.i(TAG, "    mShakeDelayDefaultValue: " + mShakeDelayDefaultValue);
//Log.i(TAG, "    shakeDelay: " + shakeDelay);
            Preference shakeDelayPref = (SliderPreference) findPreference(PREF_PLAY_MUSIC_SHAKE_DELAY);
//Log.i(TAG, "shakeDelayPref: " + shakeDelayPref);
            shakeDelayPref.setSummary(String.format("%.2f s", ShakeDetector.getDelay(shakeDelay)));
            int shakeDuration = sharedPref.getInt(PREF_PLAY_MUSIC_SHAKE_DURATION, mShakeDurationDefaultValue);
//Log.i(TAG, "    mShakeDurationDefaultValue: " + mShakeDurationDefaultValue);
//Log.i(TAG, "    shakeDuration: " + shakeDuration);
            Preference shakeDurationPref = (SliderPreference) findPreference(PREF_PLAY_MUSIC_SHAKE_DURATION);
//Log.i(TAG, "shakeDurationPref: " + shakeDurationPref);
            shakeDurationPref.setSummary(String.format("%.2f s", ShakeDetector.getDuration(shakeDuration)));
            int shakeThreshold = sharedPref.getInt(PREF_PLAY_MUSIC_SHAKE_THRESHOLD, mShakeThresholdDefaultValue);
//Log.i(TAG, "    mShakeThresholdDefaultValue: " + mShakeThresholdDefaultValue);
//Log.i(TAG, "    shakeThreshold: " + shakeThreshold);
            Preference shakeThresholdPref = (SliderPreference) findPreference(PREF_PLAY_MUSIC_SHAKE_THRESHOLD);
//Log.i(TAG, "shakeThresholdPref: " + shakeThresholdPref);
            shakeThresholdPref.setSummary(String.format("%.2f g", ShakeDetector.getThreshold(shakeThreshold)));
        }

        @Override
        public void onResume() {
            super.onResume();
            getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
        }

        @Override
        public void onPause() {
            super.onPause();
            getPreferenceScreen().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
        }
        // SharedPreferences.OnSharedPreferenceChangeListener
        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPref, String key)
        {
            Log.i(TAG, "onSharedPreferenceChanged");
            Log.i(TAG, "key: |" + key + "|");
            Preference pref = getPreferenceManager().findPreference(key);
            if (key.equals(PREF_CONNECTION_WIFI_ONLY)) {
                boolean wifiOnly = sharedPref.getBoolean(PREF_CONNECTION_WIFI_ONLY, mWifiOnlyDefaultValue);
                pref.setSummary(wifiOnly ? R.string.pref_summary_wifiOnly_True : R.string.pref_summary_wifiOnly_False);
            } else if (key.equals(PREF_PLAY_MUSIC_LOOP)) {
                boolean loop = sharedPref.getBoolean(PREF_PLAY_MUSIC_LOOP, mLoopDefaultValue);
                pref.setSummary(loop ? R.string.pref_summary_loop_True : R.string.pref_summary_loop_False);
            } else if (key.equals(PREF_PLAY_MUSIC_BY_SHAKE)) {
                boolean byShake = sharedPref.getBoolean(PREF_PLAY_MUSIC_BY_SHAKE, mByShakeDefaultValue);
                pref.setSummary(byShake ? R.string.pref_summary_byShake_True : R.string.pref_summary_byShake_False);
                if (byShake) {
                    mSensorManager.registerListener(mShakeDetector, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);//SENSOR_DELAY_UI);
                    mSensorManager.registerListener(mShakeDetector, mGravity, SensorManager.SENSOR_DELAY_UI);
                } else {
                    mService.set(PlayMusicService.ACTION_STOP);
                    mSensorManager.unregisterListener(mShakeDetector);
                }
            } else if (key.equals(PREF_PLAY_MUSIC_SHAKE_DELAY)) {
                int shakeDelay = sharedPref.getInt(PREF_PLAY_MUSIC_SHAKE_DELAY, mShakeDelayDefaultValue);
                pref.setSummary(String.format("%.2f s", ShakeDetector.getDelay(shakeDelay)));

                Activity activity = getActivity();
                Context context = (Context) getActivity();
                Toast.makeText(context, R.string.pref_toastMessage_shake, Toast.LENGTH_SHORT).show();
            } else if (key.equals(PREF_PLAY_MUSIC_SHAKE_DURATION)) {
                int shakeDuration = sharedPref.getInt(PREF_PLAY_MUSIC_SHAKE_DURATION, mShakeDurationDefaultValue);
                pref.setSummary(String.format("%.2f s", ShakeDetector.getDuration(shakeDuration)));

                Activity activity = getActivity();
                Context context = (Context) getActivity();
                Toast.makeText(context, R.string.pref_toastMessage_shake, Toast.LENGTH_SHORT).show();
            } else if (key.equals(PREF_PLAY_MUSIC_SHAKE_THRESHOLD)) {
                int shakeThreshold = sharedPref.getInt(PREF_PLAY_MUSIC_SHAKE_THRESHOLD, mShakeThresholdDefaultValue);
                pref.setSummary(String.format("%.2f g", ShakeDetector.getThreshold(shakeThreshold)));

                Activity activity = getActivity();
                Context context = (Context) getActivity();
                Toast.makeText(context, R.string.pref_toastMessage_shake, Toast.LENGTH_SHORT).show();
            }
        }
    }
}
