/**
 * template: ApiDemos/src/com/example/android/apis/app/LoaderCustom.java
 */
package com.dn2soft.dicka;

import com.dn2soft.dicka.ZipFileExIteration.FileElement;
import com.dn2soft.dicka.FileListLoad.FileSystemAudioFilter;
import com.dn2soft.dicka.FileListLoad.AssetAudioFilter;

////////////////////////////////////////////////////////////////////////////////
/** disable google services
import android.location.Location;
import android.location.Geocoder;
import android.location.Address;
import android.content.IntentSender;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.LocationClient;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.GooglePlayServicesClient;
import com.google.android.gms.common.GooglePlayServicesClient.ConnectionCallbacks;
import com.google.android.gms.common.GooglePlayServicesClient.OnConnectionFailedListener;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.ConnectionResult;
import android.app.Dialog;
import android.app.DialogFragment;
 */
////////////////////////////////////////////////////////////////////////////////

import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;

import android.hardware.SensorManager;
import android.hardware.Sensor;

import android.view.MotionEvent;
import android.support.v4.view.GestureDetectorCompat;
import android.view.GestureDetector;

import android.content.Intent;
import android.text.method.LinkMovementMethod;
import android.content.res.Resources;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.graphics.Typeface;

// used only in getWiFiInfoStr
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.view.Gravity;
import android.graphics.Color;

// for Data
import com.dn2soft.dickc.dictionary.Cambridge;   // Cambridge.Result
import java.util.List;

import android.view.Window;     // FEATURE_INDETERMINATE_PROGRESS

// used for getDickRootPath
import android.os.Environment;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.zip.ZipException;

//// activity (Context, Activity, Bundle) but through (FragmentManager) fragment (ListFragment)
import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.app.FragmentManager;

// logging...
import android.util.Log;

import android.widget.ListView;

// for search menu
import android.view.Menu;
import android.view.MenuItem;
import android.widget.SearchView;

// "common" views...
import android.view.View;
import android.widget.TextView;

import android.content.res.Resources;
import android.preference.PreferenceManager;
import android.content.SharedPreferences;

/**
 * main activity
 */
public class LoadActivity
    extends
        Activity
/** disable google services
    implements
        GoogleApiClient.ConnectionCallbacks,
        GoogleApiClient.OnConnectionFailedListener,
        GooglePlayServicesClient.ConnectionCallbacks,
        GooglePlayServicesClient.OnConnectionFailedListener
 */
{
    private static final String TAG = "LoadActivity";

    // see seach policy in Search.doInBackground
    public final static boolean ENFORCE_WORD_SMART_ONLY = true; // in case that there are no available (Wi-Fi) networks

    // basic data structures: Entry for list, Data for search
    public static class Entry {
        String  mWord;
        String  mSense;
        public Entry(String word, String sense) {
            mWord = word;
            mSense = sense;
        }
        public String getWord() {
            return mWord;
        }
        public String getSense() {
            return mSense;
        }
    }
    public static class Data {
        public EnglishKoreanContract.EnglishKoreanJson  englishKoreanJson;
        public WordSmartContract.WordSmartJson          wordSmartJson;
        public Cambridge.Result                         result;

        public List<Entry>   entries;    // could be null
    }

    private static File mDickRootPath;
    public static File getDickRootPath() {return mDickRootPath;}
    public static boolean isWiFiAvailable(final Activity activity)
    {
        return getWiFiInfoStr(activity, false).isEmpty();
    }

    static class MyGestureListener extends GestureDetector.SimpleOnGestureListener {
        private static final String DEBUG_TAG = "Gestures"; 
        
        @Override
        public boolean onDown(MotionEvent event)
        { 
            Log.i(TAG, "onDown: " + event.toString()); 
            return true;
        }

        @Override
        public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY) {
            Log.i(TAG, "onFling: " + event1.toString() + ", " + event2.toString());
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
            Log.i(TAG, "onScroll: " + event1.toString() + ", " + event2.toString());
            return true;
        }

        @Override
        public boolean onDoubleTap(MotionEvent event)
        { 
            Log.i(TAG, "onDoubleTap: " + event.toString()); 
            return true;
        }

        @Override
        public void onLongPress(MotionEvent event)
        { 
            Log.i(TAG, "onLongPress: " + event.toString()); 
            return;
        }

        @Override
        public boolean onSingleTapConfirmed(MotionEvent event)
        { 
            Log.i(TAG, "onSingleTapConfirmed: " + event.toString()); 
            return true;
        }
    }
    @Override 
    public boolean onTouchEvent(MotionEvent event){ 
        this.mDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }
    private GestureDetectorCompat mDetector;

    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private Sensor mGravity;
    private ShakeDetector mShakeDetector;
    private static boolean mByShakeDefaultValue;
    @Override
    public void onResume() {
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

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        final Context context = (Context) this;

        boolean b = requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
////    Log.i(TAG, "requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS): " + b);
        if (b) {
////        setProgressBarIndeterminateVisibility(false);
        }

        Intent intent = new Intent(PlayMusicService.ACTION_NONE);   // just start a service
        intent.setClass(context, PlayMusicService.class);
        context.startService(intent);


        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        Resources res = context.getResources();
        mByShakeDefaultValue = res.getBoolean(R.bool.pref_defaultValue_byShake);

/*
        mDetector = new GestureDetectorCompat(this, new MyGestureListener());

        SensorManager sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
Log.i(TAG, "" + sensorManager);
        List<Sensor> deviceSensors = sensorManager.getSensorList(Sensor.TYPE_ALL);
    Log.i(TAG, "# of sensors = " + deviceSensors.size());
        for (Sensor sensor: deviceSensors) {
            Log.i(TAG, sensor.getName() + ", " + sensor.getVendor() + ", " + sensor.toString());
        }
 */

        // ShakeDetector initialization
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
            }
        });

        mDickRootPath = getDickRootPath((Context) this);

        LoadActivity.getWiFiInfoStr(this, true);

        FragmentManager fm = getFragmentManager();

        // Create the list fragment and add it as our sole content.
        if (fm.findFragmentById(android.R.id.content) == null) {
            AppListFragment list = new AppListFragment();
            fm.beginTransaction().add(android.R.id.content, list).commit();
        }

        CONTAINER = new ZipFileExContainer(((Context) this).getCacheDir(), new FileListLoad.ZipFilterAudio());
        FOLDER_EXPLORER = new FolderExplorer();
        FILE_SYSTEM_AUDIO_FILTER = new FileSystemAudioFilter();
        ASSET_AUDIO_FILTER = new AssetAudioFilter(((Context) this).getAssets());
        File oggFile = new File(mDickRootPath, "ogg/ogg.zip");
        try {
            mZipFileExOgg = new ZipFileEx(oggFile, ((Context) this).getCacheDir());
        }
        catch (FileNotFoundException e) {e.printStackTrace();}
        catch (ZipException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}
        File mp3File = new File(mDickRootPath, "mp3/mp3.zip");
        try {
            mZipFileExMp3 = new ZipFileEx(mp3File, ((Context) this).getCacheDir());
        }
        catch (FileNotFoundException e) {e.printStackTrace();}
        catch (ZipException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}

/** disable google services
        servicesConnected();
        mLocationClient = new LocationClient(this, this, this);
        mGoogleApiClient = new GoogleApiClient.Builder(this)
            .addApi(LocationServices.API)
            .addConnectionCallbacks(this)
            .addOnConnectionFailedListener(this)
            .build();
 */
    }
    public static FileSystemAudioFilter FILE_SYSTEM_AUDIO_FILTER;
    public static AssetAudioFilter ASSET_AUDIO_FILTER;
    public static ZipFileEx mZipFileExOgg;
    public static ZipFileEx mZipFileExMp3;
    public static ZipFileExContainer CONTAINER;
    public static FolderExplorer FOLDER_EXPLORER;

////////////////////////////////////////////////////////////////////////////////
/** disable google services
    public static GoogleApiClient mGoogleApiClient;
    public static LocationClient mLocationClient;
    // Global constants
    //
    // Define a request code to send to Google Play services
    // This code is returned in Activity.onActivityResult
    ///
    private final static int CONNECTION_FAILURE_RESOLUTION_REQUEST = 9000;
    // Define a DialogFragment that displays the error dialog
    public static class ErrorDialogFragment extends DialogFragment {
        // Global field to contain the error dialog
        private Dialog mDialog;
        // Default constructor. Sets the dialog field to null
        public ErrorDialogFragment() {
            super();
            mDialog = null;
        }
        // Set the dialog to display
        public void setDialog(Dialog dialog) {
            mDialog = dialog;
        }
        // Return a Dialog to the DialogFragment.
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            return mDialog;
        }
    }
    private boolean servicesConnected() {
        // Check that Google Play services is available
        int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable(this);
        // If Google Play services is available
        if (ConnectionResult.SUCCESS == resultCode) {
            // In debug mode, log the status
            Log.d(TAG, "Location Updates: Google Play services is available.");
            // Continue
            return true;
        // Google Play services was not available for some reason.
        // resultCode holds the error code.
        } else {
            // Get the error dialog from Google Play services
            Dialog errorDialog = GooglePlayServicesUtil.getErrorDialog(
                    resultCode,
                    this,
                    CONNECTION_FAILURE_RESOLUTION_REQUEST);

            // If Google Play services can provide an error dialog
            if (errorDialog != null) {
                // Create a new DialogFragment for the error dialog
                ErrorDialogFragment errorFragment = new ErrorDialogFragment();
                // Set the dialog in the DialogFragment
                errorFragment.setDialog(errorDialog);
                // Show the error dialog in the DialogFragment
                ////errorFragment.show(getSupportFragmentManager(), "Location Updates");
                errorFragment.show(getFragmentManager(), "Location Updates");
            }
        }
        return true;
    }
    // GoogleApiClient.ConnectionCallbacks.onConnected
    // GooglePlayServicesClient.ConnectionCallbacks.onConnected
    @Override
    public void onConnected(Bundle connectionHint)
    {
        Location location = null;
        location = mLocationClient.getLastLocation();
        if (location == null)
            return;
        Geocoder geocoder = new Geocoder(this);
        List<Address> addresses = null;
        try {
            addresses = geocoder.getFromLocation(location.getLatitude(), location.getLongitude(), 5);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
        if (addresses != null) {
            for (Address address: addresses) {
                String addressText = String.format(
                        "%s, %s, %s",
                        // If there's a street address, add it
                        address.getMaxAddressLineIndex() > 0 ?
                                address.getAddressLine(0) : "",
                        // Locality is usually a city
                        address.getLocality(),
                        // The country of the address
                        address.getCountryName());
Log.i(TAG, addressText);
            }
        } else {
            Log.i(TAG, "no address not found");
        }
    }
    // GooglePlayServicesClient.ConnectionCallbacks.onConnected
    @Override
    public void onDisconnected()
    {
Log.i(TAG, "ConnectionCallbacks.onDisconnected()");
    }
    @Override
    // GoogleApiClient.ConnectionCallbacks.onConnectionSuspended
    public void onConnectionSuspended(int cause)
    {
Log.i(TAG, "onConnectionSuspended: cause = " + cause);
    }
    // GooglePlayServicesClient.OnConnectionFailedListener.onConnectionFailed
    // GoogleApiClient.OnConnectionFailedListener.onConnectionFailed
    //
    // Called by Location Services if the attempt to
    // Location Services fails.
    ///
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult) {
        //
        // Google Play services can resolve some errors it detects.
        // If the error has a resolution, try sending an Intent to
        // start a Google Play services activity that can resolve
        // error.
        ///
        if (connectionResult.hasResolution()) {
            try {
                // Start an Activity that tries to resolve the error
                connectionResult.startResolutionForResult(
                        this,
                        CONNECTION_FAILURE_RESOLUTION_REQUEST);
                //
                // Thrown if Google Play services canceled the original
                // PendingIntent
                ///
            } catch (IntentSender.SendIntentException e) {
                // Log the error
                e.printStackTrace();
            }
        } else {
            //
            // If no resolution is available, display a dialog to the
            // user with the error.
            ///
//          showErrorDialog(connectionResult.getErrorCode());
            Log.i(TAG, "showErrorDialog: " + connectionResult.getErrorCode());
        }
    }
 */
////////////////////////////////////////////////////////////////////////////////


    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        getActionBar().setSubtitle("Loading assets...");
    }

    public static PlayMusicService mPlayMusicService;
    public static void addAudioFiles(List<FileElement> audioFiles)
    {
        if (mPlayMusicService != null)
            mPlayMusicService.addAudioFiles(audioFiles);
    }
    private boolean mBound;
    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            PlayMusicService.PlayMusicBinder binder = (PlayMusicService.PlayMusicBinder) service;
            mPlayMusicService = binder.getService();
Log.i(TAG, "" + mPlayMusicService);
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            mPlayMusicService = null;
            mBound = false;
        }
    };
    @Override
    public void onStart()
    {
        super.onStart();
        // Bind to LocalService
        Intent intent = new Intent((Context) this, PlayMusicService.class);
        ((Context) this).bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
/** disable google services
        mLocationClient.connect();
 */
    }
    @Override
    public void onStop()
    {
        // Unbind from the service
        if (mBound) {
            ((Context) this).unbindService(mConnection);
            mBound = false;
        }
/** disable google services
        mLocationClient.disconnect();
 */
        super.onStop();
    }
    @Override
    public void onDestroy()
    {
        super.onDestroy();
        stopService(new Intent(this, PlayMusicService.class));
        try {
            if (mZipFileExOgg != null) {
                mZipFileExOgg.cleanup();
            }
            if (mZipFileExMp3 != null) {
                mZipFileExMp3.cleanup();
            }
        } catch (IOException e) {e.printStackTrace();}
        if (CONTAINER != null)
            CONTAINER.closeAll();
    }


    public static class AppListFragment extends ListFragmentEx {

        // This is the Adapter being used to display the list's data.
        private ArrayAdapterEx mAdapter;

        @Override public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);

            // Give some text to display if there is no data.  In a real
            // application this would come from a resource.
            setEmptyText("");

            // We have a menu item to show in action bar.
            setHasOptionsMenu(true);

            // Create an empty adapter we will use to display the loaded data.
            mAdapter = new ArrayAdapterEx((Context) getActivity(), ArrayAdapterEx.TYPE_TWO_ITEMS);
            setListAdapter(mAdapter);

            ListView lv = getListView();
            TextView tv = (TextView) lv.getEmptyView();
/*
            final GestureDetectorCompat detector = new GestureDetectorCompat((Context) getActivity(), new MyGestureListener());

            tv.setOnTouchListener(
                new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        return detector.onTouchEvent(event);
                    }
                }
            );
 */
            Resources r = ((Context) getActivity()).getResources();
            tv.setGravity(Gravity.LEFT | Gravity.TOP);
            tv.setMovementMethod(LinkMovementMethod.getInstance());
            tv.setText("");
            tv.scrollTo(0, 0);
            tv.setTextSize(r.getDimension(R.dimen.textsize));
            tv.setTextColor(r.getColor(R.color.text));
            tv.setPadding(50, 50, 50, 50);
            tv.setTypeface(Typeface.SANS_SERIF);

            String[][] usage = {
                {"!ab." , "search cancer terms"     , ""},
                {"ab."  , "search words or phrases" , ""},
            };
            for (int i = 0; i < usage.length; ++i) {
                String x = usage[i][0];
                Spannable xSpan = new SpannableString(x);
                xSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.info)), 0, x.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                tv.append(xSpan);
                tv.append("\n");
                String y = usage[i][1];
                Spannable ySpan = new SpannableString(y);
                ySpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.example)), 0, y.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                tv.append(ySpan);
                String z = usage[i][2];
                if (!z.isEmpty()) {
                    tv.append("  ");
                    z = "(note " + z + ")";
                    Spannable zSpan = new SpannableString(z);
                    zSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.gw)), 0, z.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                    tv.append(zSpan);
                }
                tv.append("\n");
            }
            tv.append("\n\n");
            String s = "Note \".\" means \"*\" (any), so use \".\" as many.";
            Spannable sSpan = new SpannableString(s);
            sSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.ipa)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            tv.append(sSpan);
            tv.append("\n");
            s = "but \"...\" (three times) switches dictionaries.";
            sSpan = new SpannableString(s);
            sSpan.setSpan(new ForegroundColorSpan(r.getColor(R.color.ipa)), 0, s.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            tv.append(sSpan);
            tv.append("\n");
        }

        @Override
        public void onListItemClick(ListView l, View v, int position, long id)
        {
            // Insert desired behavior here.
////        Log.i(TAG, "Item clicked: " + id);
        }

        @Override
        public void onPrepareOptionsMenu(Menu menu)
        {
////        Log.i(TAG, "onPrepareOptionsMenu: " + mSearchItem);
            new DeployAsset(getActivity(), mSearchItem).execute(
                "ogg/ogg.zip",
                "mp3/mp3.zip",
                HistoryDatabase.DATABASE_NAME,
                LabelDatabase.DATABASE_NAME,
                VocabularyDatabase.DATABASE_NAME,
                CambridgeDatabase.DATABASE_NAME,
                WordSmartDatabase.DATABASE_NAME,
                EnglishKoreanDatabase.DATABASE_NAME
/*
                "sevenyearlove___white.mp3",
                "cocktail___marronnier.mp3",
                "my___sgwannabe.mp3",
                "happy___pharrell.mp3",
                "now_and_forever.mp3",
                "you_and_i___iu.mp3"
 */
////////        EnglishKoreanDatabase.DATABASE_NAME,
////////        NationalCancerInstituteDatabase.DATABASE_NAME
            );
        }
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
            builder.setNeutralButton("ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    if (!ENFORCE_WORD_SMART_ONLY)
                        activity.finish();
                }
            });
            TextView title = new TextView(activity);
            title.setText(msg + (ENFORCE_WORD_SMART_ONLY ? "\nUse Word Smart Only" : ""));
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

    public static File getDickRootPath(Context context)
    {
        String state = Environment.getExternalStorageState();
////    Log.i(TAG, "state: " + state);
        File dickaRootPath;
        if (!Environment.MEDIA_MOUNTED.equals(state)) {
            // internal storage
            dickaRootPath = new File(context.getFilesDir(), "dicka");
        } else {
            // external storage
            dickaRootPath = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "dicka");
                Log.e(TAG, "not a directory: |" + dickaRootPath.getAbsolutePath() + "|");
            if (dickaRootPath.isFile()) {
                Log.e(TAG, "not a directory: |" + dickaRootPath.getAbsolutePath() + "|");
                dickaRootPath = new File(context.getFilesDir(), "dicka");
            }
        }
        if (dickaRootPath == null) {
            Log.e(TAG, "Root Path is null");
        } else {
            if (!dickaRootPath.isDirectory() && !dickaRootPath.mkdirs()) {
                Log.e(TAG, "failed to create a path(|" + dickaRootPath.getAbsolutePath() + "|)");
                dickaRootPath = null;
            }
        }
        if (dickaRootPath != null) {
            File audioPath;
            audioPath = new File(dickaRootPath, "ogg");
            audioPath.mkdirs();
            audioPath = new File(dickaRootPath, "mp3");
            audioPath.mkdirs();
        }
        return dickaRootPath;
    }

    public static boolean onQueryTextChange(final String newText, final Context context, final SearchView searchView, final MenuItem searchItem) {
        ////Log.i(TAG, "onQueryTextChange: " + newText);
        if (newText.endsWith(PlayMusicService.ACTION_TOGGLE_STR)) {
            searchItem.collapseActionView();
Log.i(TAG, "PlayMusicService.ACTION_TOGGLE_STR");
        }
        if (newText.endsWith(PlayMusicService.ACTION_NEXT_STR)) {
Log.i(TAG, "PlayMusicService.ACTION_NEXT_STR");
            searchItem.collapseActionView();
        }
        if (newText.endsWith(Search.SWITCH_VOCABULARY)) {
            boolean onlyInWordSmart = !newText.startsWith(DictionaryProvider.VOCABULARY_CHAR);
            String text = onlyInWordSmart ? DictionaryProvider.VOCABULARY_CHAR : "";
            text += newText.substring(onlyInWordSmart ? 0 : 1, newText.length() - Search.SWITCH_VOCABULARY.length());
            searchView.setQuery(text, false);
        }
        return true;
    }
}
