package com.dn2soft.dicka;

import android.util.Log;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.FloatMath;
import android.content.Context;
import android.content.res.Resources;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class ShakeDetector implements SensorEventListener {
    private static final String TAG = "ShakeDetector";

    final Context mContext;
    final boolean mHasGravity;
    final int mShakeDelayDefaultValue;
    final int mShakeDurationDefaultValue;
    final int mShakeThresholdDefaultValue;
    ShakeDetector(Context context, boolean hasGravity) {
        mContext = context;
        mHasGravity = hasGravity;
        Resources res = context.getResources();
        mShakeDelayDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeDelay);
        mShakeDurationDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeDuration);
        mShakeThresholdDefaultValue = res.getInteger(R.integer.pref_defaultValue_shakeThreshold);
    }

    private static final float SHAKE_THRESHOLD_DELICATE = .2f;  // 20% of g
    private static final float SHAKE_THRESHOLD_MODERATE = .5f;  // 50% of g
    private static final float SHAKE_THRESHOLD_STRONG   = 1.f;  // 100% of g
    private static final int SHAKE_DELAY_TIME_MS_MIN    = 200; // at delay100 = 0
    private static final int SHAKE_DELAY_TIME_MS        = 400;     // at delay100 = 50
    private static final int SHAKE_DELAY_TIME_MS_MAX    = 600; // at delay100 = 100
    private static final int SHAKE_DURATION_TIME_MS_MIN = 500;  // at duration100 = 0
    private static final int SHAKE_DURATION_TIME_MS     = 1000;     // at duration100 = 50
    private static final int SHAKE_DURATION_TIME_MS_MAX = 2000; // at duration100 = 100
    private static final float SHAKE_THRESHOLD_G_MIN  = .1f;  // at threshold100 = 0
    private static final float SHAKE_THRESHOLD_G      = 2.f;  // at threshold100 = 50
    private static final float SHAKE_THRESHOLD_G_MAX  = 3.f;  // at threshold100 = 100
    //// how to convert value that reages 0 to 100
    // in unit of "s" (second)
    public static float getDelay(int delay100)
    {
        if (delay100 > 50) {
            // delay100 = 50, SHAKE_DELAY_TIME_MS
            // delay100 = 100, SHAKE_DELAY_TIME_MS_MAX
            // then, at 50 <= delay100 <= 100
            return ((SHAKE_DELAY_TIME_MS_MAX - SHAKE_DELAY_TIME_MS) / 50.f * (delay100 - 50) + SHAKE_DELAY_TIME_MS) * .001f;
        } else {
            // delay100 = 0, SHAKE_DELAY_TIME_MS_MIN
            // delay100 = 100, SHAKE_DELAY_TIME_MS
            // then, at 0 <= delay100 <= 50
            return ((SHAKE_DELAY_TIME_MS - SHAKE_DELAY_TIME_MS_MIN) / 50.f * (delay100 - 50) + SHAKE_DELAY_TIME_MS) * .001f;
        }
    }
    // in unit of "s" (second)
    public static float getDuration(int duration100)
    {
        if (duration100 > 50) {
            // duration100 = 50, SHAKE_DURATION_TIME_MS
            // duration100 = 100, SHAKE_DURATION_TIME_MS_MAX
            // then, at 50 <= duration100 <= 100
            return ((SHAKE_DURATION_TIME_MS_MAX - SHAKE_DURATION_TIME_MS) / 50.f * (duration100 - 50) + SHAKE_DURATION_TIME_MS) * .001f;
        } else {
            // duration100 = 0, SHAKE_DURATION_TIME_MS_MIN
            // duration100 = 100, SHAKE_DURATION_TIME_MS
            // then, at 0 <= duration100 <= 50
            return ((SHAKE_DURATION_TIME_MS - SHAKE_DURATION_TIME_MS_MIN) / 50.f * (duration100 - 50) + SHAKE_DURATION_TIME_MS) * .001f;
        }
    }
    // in unit of "g"
    public static float getThreshold(int threshold100)
    {
        if (threshold100 > 50) {
            // threshold100 = 50, SHAKE_THRESHOLD_G
            // threshold100 = 100, SHAKE_THRESHOLD_G_MAX
            // then, at 50 <= threshold100 <= 100
            return (SHAKE_THRESHOLD_G_MAX - SHAKE_THRESHOLD_G) / 50.f * (threshold100 - 50) + SHAKE_THRESHOLD_G;
        } else {
            // threshold100 = 0, SHAKE_THRESHOLD_G_MIN
            // threshold100 = 100, SHAKE_THRESHOLD_G
            // then, at 0 <= threshold100 <= 50
            return (SHAKE_THRESHOLD_G - SHAKE_THRESHOLD_G_MIN) / 50.f * (threshold100 - 50) + SHAKE_THRESHOLD_G;
        }
    }

    private long mShakeTimestamp0;
    private long mShakeTimestamp;
    private int mShakeCount;

    public interface OnShakeListener {
        public void onShake(int count);
    }
    private OnShakeListener mListener = null;
    public void setOnShakeListener(OnShakeListener listener) {
        this.mListener = listener;
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // ignore
    }

    private float gx = 0, gy = 0, gz = 0;
    @Override
    public void onSensorChanged(SensorEvent event) {
        if (mListener != null) {
            if (event.sensor.getType() == Sensor.TYPE_GRAVITY) {
                gx = event.values[0];
                gy = event.values[1];
                gz = event.values[2];
                return;
            }
            // Sensor.TYPE_ACCELEROMETER
            if (mHasGravity && gx == 0 && gy == 0 && gz == 0)  // not set yet
                return;
            float ax = event.values[0];
            float ay = event.values[1];
            float az = event.values[2];

/*
            float gX = x / SensorManager.GRAVITY_EARTH;
            float gY = y / SensorManager.GRAVITY_EARTH;
            float gZ = z / SensorManager.GRAVITY_EARTH;

            // gForce will be close to 1 when there is no movement.
            float gForce = FloatMath.sqrt(gX * gX + gY * gY + gZ * gZ);
            float da = (a - g) / a;
            if (da < 0)
                da = -da;
if (da > 0.2f) {
Log.i(TAG, "da = " + da);
Log.i(TAG, "g = " + SensorManager.GRAVITY_EARTH + ", a = " + a + ", " + ((a - g) / a * 100) + ", Force = " + gForce);
}
 */
            float dx = ax - gx;
            float dy = ay - gy;
            float dz = az - gz;
            gx = 0.f;
            gy = 0.f;
            gz = 0.f;
            float d = FloatMath.sqrt(dx * dx + dy * dy + dz * dz);
            SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(mContext);
            int delay100 = sharedPref.getInt(SettingsActivity.PREF_PLAY_MUSIC_SHAKE_DELAY, mShakeDelayDefaultValue);
            int duration100 = sharedPref.getInt(SettingsActivity.PREF_PLAY_MUSIC_SHAKE_DURATION, mShakeDurationDefaultValue);
            int threshold100 = sharedPref.getInt(SettingsActivity.PREF_PLAY_MUSIC_SHAKE_THRESHOLD, mShakeThresholdDefaultValue);
            if (mHasGravity)
                d = d / SensorManager.GRAVITY_EARTH;
            else {
                d = (d - SensorManager.GRAVITY_EARTH) / SensorManager.GRAVITY_EARTH;
                if (d < 0)
                    d = -d;
            }
////        if (d > (mHasGravity ? SHAKE_THRESHOLD_STRONG : SHAKE_THRESHOLD_MODERATE)) {
            final long now = System.currentTimeMillis();
            if (mShakeTimestamp0 + (int) (ShakeDetector.getDuration(duration100) * 1000.f) < now) {
//            if (mShakeTimestamp0 + SHAKE_DURATION_TIME_MS < now) {
                if (mShakeCount > 0) {
Log.i(TAG, "# of count = " + mShakeCount);
                    mListener.onShake(mShakeCount);
                    mShakeTimestamp = now;
                    mShakeCount = 0;
                    return;
                }
            }

            if (d > ShakeDetector.getThreshold(threshold100)) {
            Log.i(TAG, String.format("threshold100: %d (%.2f g)", threshold100, ShakeDetector.getThreshold(threshold100)));
                // ignore shake events too close to each other (500ms)
/*
                Log.i(TAG, "g: (" + gx + ", " + gy + ", " + gz + "): |g| = " + FloatMath.sqrt(gx*gx + gy*gy + gz*gz));
                Log.i(TAG, "a: (" + ax + ", " + ay + ", " + az + "): |a| = " + FloatMath.sqrt(ax*ax + ay*ay + az*az));
                Log.i(TAG, "d: (" + dx + ", " + dy + ", " + dz + "), |d| = " + d);
 */
                if (mShakeTimestamp + (int) (ShakeDetector.getDelay(delay100) * 1000.f) > now) {
                    return;
                }
/*
                Log.i(TAG, "g: (" + gx + ", " + gy + ", " + gz + "): |g| = " + FloatMath.sqrt(gx*gx + gy*gy + gz*gz));
                Log.i(TAG, "a: (" + ax + ", " + ay + ", " + az + "): |a| = " + FloatMath.sqrt(ax*ax + ay*ay + az*az));
                Log.i(TAG, "d: (" + dx + ", " + dy + ", " + dz + "), |d| = " + d);
 */

                // reset the shake count after 3 seconds of no shakes
Log.i(TAG, "got a shake");
                if (mShakeCount == 0)
                    mShakeTimestamp0 = now;
                mShakeTimestamp = now;
                ++mShakeCount;
            }
////        }
        }
    }
}
