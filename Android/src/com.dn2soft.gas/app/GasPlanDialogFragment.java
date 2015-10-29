package com.dn2soft.gas.app;
/**
    google location service
 */
import com.dn2soft.dicka.LoadActivity;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.ConnectionResult;
import android.location.Location;
import android.location.Geocoder;
import android.location.Address;
import java.util.Locale;
import java.io.IOException;

import com.dn2soft.gas.GasStationTrip;

import com.dn2soft.dicka.R;
////import com.dn2soft.xml.R;
////import com.dn2soft.xml.Viewer;

import android.content.Intent;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.CompoundButton;

import android.content.DialogInterface;

import android.widget.TextView;
import android.widget.Button;
import android.widget.AdapterView;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import android.content.Context;

import android.util.Log;

import android.app.DialogFragment;
import android.app.Dialog;
import android.app.AlertDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

import android.os.AsyncTask;
import java.util.List;


public class GasPlanDialogFragment
    extends
        DialogFragment
    implements
        View.OnClickListener,
        CompoundButton.OnCheckedChangeListener
{
    private static final String TAG = "GasPlanDialogFragment";
    private static final int REQ_CODE_SET_ADDRESS = 1;
    private static final int REQ_CODE_ADJUST_PRICES = 2;
    //
    public static final CharSequence NOT_SET_YET = "not set yet";
    // for return; see onClick
    public static final String HOME = "home address";
    public static final String HERE = "here location";
    public static final String THERE = "there location";
    public static final String MILEAGE = "mileage";
    public static final String BY_DOLLARS = "by dollars";
    public static final String AMOUNT = "amount";
    public static final int SORT_BY_CASH                   = 1;
    public static final int SORT_BY_CREDIT_CARD            = 2;
    public static final int SORT_BY_ADJUSTED_CASH          = 3;
    public static final int SORT_BY_ADJUSTED_CREDIT_CARD   = 4;


    public static void showWarning(Context context, String message)
    {
        new AlertDialog.Builder(context)
          .setTitle("Warning")
          .setMessage(message)
          .setCancelable(false)
          .setPositiveButton(
            "okay",
            new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int id)
                {
                    dialog.dismiss();
                }
            }
          )
          .create().show();
    }

    private CharSequence mHomeAddress;
    private CharSequence mHereAddress;
    private CharSequence mThereAddress;
    public static GasPlanDialogFragment newInstance(
        CharSequence homeAddress,
        CharSequence hereStr,
        CharSequence thereStr,
        Float mileage,
        boolean byDollars,
        Float amount
    )
    {
        GasPlanDialogFragment f = new GasPlanDialogFragment();
        Bundle args = new Bundle();
        args.putCharSequence(HOME, homeAddress);
        args.putCharSequence(HERE, hereStr);
        args.putCharSequence(THERE, thereStr);
        args.putFloat(MILEAGE, mileage == null ? -1.f : mileage);
        args.putBoolean(BY_DOLLARS, byDollars);
        args.putFloat(AMOUNT, amount == null ? -1.f : amount);
        f.setArguments(args);
        return f;
    }
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Bundle args = getArguments();
        mHomeAddress = args.getCharSequence(HOME, null);
        mHereAddress = args.getCharSequence(HERE, null);
        mThereAddress = args.getCharSequence(THERE, null);
        float mileage = args.getFloat(MILEAGE, -1.f);
        mAdjustPricesMileage = mileage < 0.f ? null : mileage;
        mAdjustPricesByDollars = args.getBoolean(BY_DOLLARS, true);
        float amount = args.getFloat(AMOUNT, -1.f);
        mAdjustPricesAmount = amount < 0.f ? null : amount;
    }


/*
List/
    ----------------
    ----------------
    ......
    ----------------

----------------
ID Station
   Address

    Add
    Delete(long press)


Trip/
    Home □ Set
        ________________ (____,____)

        □ Get Location
        ________________

        <Set> <Cancel>

    Here
        ● Home
        ○ Address □ Get Location
            ________________

    There
        ● Round Trip
        ○ Long Trip
        ○ Home
        ○ Address □ Get Location
            ________________

    □ Adjust Prices
        Mileage: ____ miles/gallon
        ● Amount in dollars: $___
        ○ Amount in gallons: ___ gallons

    <Update>


Result/
    ----------------
    ----------------
    ......
    ----------------

----------------
    $ Station
      Address

    Order By
        ○ Cash Prices
        ○ Credit Card Prices
        ○ Adjusted Prices
 */

    TextView mHomeAddressTextView;
    TextView mHereAddressTextView;
    TextView mThereAddressTextView;
    RadioButton mHereHomeRadioButton;
    RadioButton mHereAddressRadioButton;
    RadioButton mThereRoundTripRadioButton;
//  RadioButton mThereLongTripRadioButton;
    RadioButton mThereHomeRadioButton;
    RadioButton mThereHomeAddressRadioButton;
    CheckBox mAdjustPricesCheckBox;
    TextView mAdjustPricesTextView;
    Float mAdjustPricesMileage;
    boolean mAdjustPricesByDollars;
    Float mAdjustPricesAmount;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View gasPlanLayoutView = inflater.inflate(R.layout.gas_plan, null);
        mHomeAddressTextView = (TextView) gasPlanLayoutView.findViewById(R.id.home_address_text);
        mHomeAddressTextView.setOnClickListener(this);
        mHereAddressTextView = (TextView) gasPlanLayoutView.findViewById(R.id.here_address_text);
        mHereAddressTextView.setOnClickListener(this);
        mThereAddressTextView = (TextView) gasPlanLayoutView.findViewById(R.id.there_address_text);
        mThereAddressTextView.setOnClickListener(this);

        mHereHomeRadioButton = (RadioButton) gasPlanLayoutView.findViewById(R.id.here_home);
        mHereHomeRadioButton.setOnCheckedChangeListener(this);
        mHereAddressRadioButton = (RadioButton) gasPlanLayoutView.findViewById(R.id.here_address);
        mHereAddressRadioButton.setOnCheckedChangeListener(this);
        mThereRoundTripRadioButton = (RadioButton) gasPlanLayoutView.findViewById(R.id.there_round_trip);
        mThereRoundTripRadioButton.setOnCheckedChangeListener(this);
//      mThereLongTripRadioButton = (RadioButton) gasPlanLayoutView.findViewById(R.id.there_long_trip);
//      mThereLongTripRadioButton.setOnCheckedChangeListener(this);
        mThereHomeRadioButton = (RadioButton) gasPlanLayoutView.findViewById(R.id.there_home);
        mThereHomeRadioButton.setOnCheckedChangeListener(this);
        mThereHomeAddressRadioButton = (RadioButton) gasPlanLayoutView.findViewById(R.id.there_address);
        mThereHomeAddressRadioButton.setOnCheckedChangeListener(this);

        mAdjustPricesCheckBox = (CheckBox) gasPlanLayoutView.findViewById(R.id.adjust_prices);
        mAdjustPricesCheckBox.setOnCheckedChangeListener(this);
        mAdjustPricesTextView = (TextView) gasPlanLayoutView.findViewById(R.id.adjust_prices_text);
        mAdjustPricesTextView.setOnClickListener(this);
        init();

        Button cancelButton = (Button) gasPlanLayoutView.findViewById(R.id.cancel);
        cancelButton.setOnClickListener(this);
        Button updateButton = (Button) gasPlanLayoutView.findViewById(R.id.update);
        updateButton.setOnClickListener(this);
        return new AlertDialog.Builder(getActivity())
            .setView(gasPlanLayoutView)
            .create();
    }
    private void init()
    {
        mHomeAddressTextView.setText(mHomeAddress == null ? NOT_SET_YET : mHomeAddress);

        if (mHereAddress == null || mHereAddress.toString().equalsIgnoreCase(GasStationTrip.HOME_STR)) {
            mHereAddress = null;
            mHereHomeRadioButton.setChecked(true);
            mHereAddressRadioButton.setChecked(false);
            mHereAddressTextView.setText(NOT_SET_YET);
            mHereAddressTextView.setVisibility(View.GONE);
        } else {
            mHereHomeRadioButton.setChecked(false);
            mHereAddressRadioButton.setChecked(true);
            mHereAddressTextView.setText(mHereAddress);
            mHereAddressTextView.setVisibility(View.VISIBLE);
        }

        if (mThereAddress == null || mThereAddress.toString().equals(GasStationTrip.ROUND_TRIP_STR)) {
            mThereAddress = null;
            mThereRoundTripRadioButton.setChecked(true);
//          mThereLongTripRadioButton.setChecked(false);
            mThereHomeRadioButton.setChecked(false);
            mThereHomeAddressRadioButton.setChecked(false);
            mThereAddressTextView.setText(NOT_SET_YET);
            mThereAddressTextView.setVisibility(View.GONE);
/*
        } else if (mThereAddress.toString().equals(GasStationTrip.LONG_TRIP_STR)) {
            mThereAddress = null;
            mThereRoundTripRadioButton.setChecked(false);
//          mThereLongTripRadioButton.setChecked(true);
            mThereHomeRadioButton.setChecked(false);
            mThereHomeAddressRadioButton.setChecked(false);
            mThereAddressTextView.setText(NOT_SET_YET);
            mThereAddressTextView.setVisibility(View.GONE);
 */
        } else if (mThereAddress.toString().equalsIgnoreCase(GasStationTrip.HOME_STR)) {
            mThereAddress = null;
            mThereRoundTripRadioButton.setChecked(false);
//          mThereLongTripRadioButton.setChecked(false);
            mThereHomeRadioButton.setChecked(true);
            mThereHomeAddressRadioButton.setChecked(false);
            mThereAddressTextView.setText(NOT_SET_YET);
            mThereAddressTextView.setVisibility(View.GONE);
        } else {
            mThereRoundTripRadioButton.setChecked(false);
//          mThereLongTripRadioButton.setChecked(false);
            mThereHomeRadioButton.setChecked(false);
            mThereHomeAddressRadioButton.setChecked(true);
            mThereAddressTextView.setText(mThereAddress);
            mThereAddressTextView.setVisibility(View.VISIBLE);
        }

        mAdjustPricesCheckBox.setChecked(mAdjustPricesMileage != null);
        mAdjustPricesTextView.setVisibility(mAdjustPricesMileage != null ? View.VISIBLE : View.GONE);
        setTextForAdjustPrices();
    }
    private int getCheckedHereId()
    {
        return mHereHomeRadioButton.isChecked() ? R.id.here_home : R.id.here_address;
    }
    private int getCheckedThereId()
    {
//      return mThereRoundTripRadioButton.isChecked() ? R.id.there_round_trip : (mThereLongTripRadioButton.isChecked() ? R.id.there_long_trip : (mThereHomeRadioButton.isChecked() ? R.id.there_home : R.id.there_address));
        return mThereRoundTripRadioButton.isChecked() ? R.id.there_round_trip : (mThereHomeRadioButton.isChecked() ? R.id.there_home : R.id.there_address);
    }
    private void showWarning(String message)
    {
        GasPlanDialogFragment.showWarning((Context) getActivity(), message);
    }
    private boolean areAllSet()
    {
        // check Here
        int hereId = getCheckedHereId();
        if (hereId == R.id.here_home) {
            if (mHomeAddress == null) {
                showWarning("Home Address: not set");
                return false;
            }
            // check There
        } else if (hereId == R.id.here_address) {
            if (mHereAddress == null) {
                showWarning("Here Address: not set");
                return false;
            }
            // check There
        } else {
            // not implements yet
        }

        // check There
        int thereId = getCheckedHereId();
        if (thereId == R.id.there_home) {
            if (mHomeAddress == null) {
                showWarning("Home Address: not set");
                return false;
            }
            // check Adjust Prices
        } else if (thereId == R.id.there_address) {
            if (mThereAddress == null) {
                showWarning("There Address: not set");
                return false;
            }
            // check Adjust Prices
        } else {
            // not implements yet
        }

        // check Adjust Prices
        if (!mAdjustPricesCheckBox.isChecked())
            return true;
        if (mAdjustPricesMileage == null) {
            showWarning("Mileage: not set");
            return false;
        }
        return true;
    }

    @Override   // View.OnClickListener
    public void onClick(View v)
    {
        int id = v.getId();
        if (id == R.id.cancel) {
            GasPlanDialogFragment.this.getDialog().dismiss();
            return;
        }
        if (id == R.id.update) {
            if (!areAllSet()) {
                return;
            }
            int sortBy = mAdjustPricesCheckBox.isChecked() ? SORT_BY_ADJUSTED_CASH : SORT_BY_CASH;
            CharSequence here = getCheckedHereId() == R.id.here_home ? GasStationTrip.HOME_STR : mHereAddress;
            CharSequence there = null;
            switch (getCheckedThereId()) {
            case R.id.there_round_trip:
                there = GasStationTrip.ROUND_TRIP_STR;
                break;
            case R.id.there_home:
                there = GasStationTrip.HOME_STR;
                break;
            case R.id.there_address:
                there = mThereAddress;
                break;
            default:
                break;
            }
            getTargetFragment().onActivityResult(
                getTargetRequestCode(),
////        Viewer.onActivityResultEx(
////            Viewer.REQ_CODE_GAS_PLAN,
                sortBy, // resultCode
                new Intent()
                    .putExtra(HOME, mHomeAddress)
                    .putExtra(HERE, here)
                    .putExtra(THERE, there)
                    .putExtra(MILEAGE, mAdjustPricesMileage)
                    .putExtra(BY_DOLLARS, mAdjustPricesByDollars)
                    .putExtra(AMOUNT, mAdjustPricesAmount)
            );
            GasPlanDialogFragment.this.getDialog().dismiss();
            return;
        }
        if (id == R.id.adjust_prices_text) {
            showAdjustPricesDialog();
            return;
        }
        showSetAddressDialog(id);
    }

    private void showSetAddressDialog(int id)
    {
        String title = null;
        CharSequence address = null;
        switch (id) {
        case R.id.home_address_text:
            title = "Home Address";
            address = mHomeAddress;
            break;
        case R.id.here_address_text:
            title = "Here Address";
            address = mHereAddress;
            break;
        case R.id.there_address_text:
            title = "There Address";
            address = mThereAddress;
            break;
        default:
            break;
        }
        SetAddress sa = SetAddress.newInstance(id, title, address);
        sa.setTargetFragment(GasPlanDialogFragment.this, REQ_CODE_SET_ADDRESS);
        sa.setCancelable(false);
        sa.show(getFragmentManager(), "set address dialog");
    }

    private void checkHomeAddress()
    {
        if (mHomeAddress != null)
            return;
        showSetAddressDialog(R.id.home_address_text);
    }
    private void checkAddress(int id)
    {
        if (id != R.id.here_address && id != R.id.there_address)
            return;
        if (
            (id == R.id.here_address && mHereAddress != null) ||
            (id == R.id.there_address && mThereAddress != null)
        )
            return;
        showSetAddressDialog(id == R.id.here_address ? R.id.here_address_text : R.id.there_address_text);
    }
    private void checkAdjustPrices()
    {
        if (mAdjustPricesMileage != null)
            return;
        showAdjustPricesDialog();
    }
    private void showAdjustPricesDialog()
    {
        AdjustPrices sa = AdjustPrices.newInstance(R.id.adjust_prices_text, mAdjustPricesMileage, mAdjustPricesByDollars, mAdjustPricesAmount);
        sa.setTargetFragment(GasPlanDialogFragment.this, REQ_CODE_ADJUST_PRICES);
        sa.setCancelable(false);
        sa.show(getFragmentManager(), "adjust prices dialog");
    }

    @Override   // CompoundButton.OnCheckedChangeListener
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
    {
        int id = buttonView.getId();
        if (id == R.id.adjust_prices) {
            mAdjustPricesTextView.setVisibility(isChecked ? View.VISIBLE : View.GONE);
            if (!isChecked)
                return;

            checkAdjustPrices();
            setTextForAdjustPrices();

            return;
        }

        if (!isChecked)
            return;
        if (id == R.id.here_home) {
            mHereHomeRadioButton.setChecked(true);
            mHereAddressRadioButton.setChecked(false);
            mHereAddressTextView.setVisibility(View.GONE);
            checkHomeAddress();
        } else if (id == R.id.here_address) {
            mHereHomeRadioButton.setChecked(false);
            mHereAddressRadioButton.setChecked(true);
            mHereAddressTextView.setVisibility(View.VISIBLE);
            checkAddress(id);
        } else if (id == R.id.there_round_trip) {
            mThereRoundTripRadioButton.setChecked(true);
//          mThereLongTripRadioButton.setChecked(false);
            mThereHomeRadioButton.setChecked(false);
            mThereHomeAddressRadioButton.setChecked(false);
            mThereAddressTextView.setVisibility(View.GONE);
/*
        } else if (id == R.id.there_long_trip) {
            mThereRoundTripRadioButton.setChecked(false);
//          mThereLongTripRadioButton.setChecked(true);
            mThereHomeRadioButton.setChecked(false);
            mThereHomeAddressRadioButton.setChecked(false);
            mThereAddressTextView.setVisibility(View.GONE);
 */
        } else if (id == R.id.there_home) {
            mThereRoundTripRadioButton.setChecked(false);
//          mThereLongTripRadioButton.setChecked(false);
            mThereHomeRadioButton.setChecked(true);
            mThereHomeAddressRadioButton.setChecked(false);
            mThereAddressTextView.setVisibility(View.GONE);
            checkHomeAddress();
        } else if (id == R.id.there_address) {
            mThereRoundTripRadioButton.setChecked(false);
//          mThereLongTripRadioButton.setChecked(false);
            mThereHomeRadioButton.setChecked(false);
            mThereHomeAddressRadioButton.setChecked(true);
            mThereAddressTextView.setVisibility(View.VISIBLE);
            checkAddress(id);
        } else {
            // not implemented yet
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode == REQ_CODE_SET_ADDRESS) {
            CharSequence address = data.getCharSequenceExtra(SetAddress.ADDRESS);
            if (resultCode == R.id.home_address_text) {
                mHomeAddressTextView.setText(address);
                mHomeAddress = address;
            } else if (resultCode == R.id.here_address_text) {
                mHereAddressTextView.setText(address);
                mHereAddress = address;
            } else if (resultCode == R.id.there_address_text) {
                mThereAddressTextView.setText(address);
                mThereAddress = address;
            } else {
                // not implemented yet
            }
        } else if (requestCode == REQ_CODE_ADJUST_PRICES) {
            if (resultCode == R.id.adjust_prices_text) {
                CharSequence cs;
                cs = data.getCharSequenceExtra(AdjustPrices.MILEAGE);
                if (cs != null) {
                    String s = cs.toString();
                    mAdjustPricesMileage = s.isEmpty() ? null : new Float(s);
                } else
                    mAdjustPricesMileage = null;
                mAdjustPricesByDollars = data.getBooleanExtra(AdjustPrices.BY_DOLLARS, true);
                cs = data.getCharSequenceExtra(AdjustPrices.AMOUNT);
                if (cs != null) {
                    String s = cs.toString();
                    mAdjustPricesAmount = s.isEmpty() ? null : new Float(s);
                } else
                    mAdjustPricesAmount = null;
                setTextForAdjustPrices();
            } else {
                // not implemented yet
            }
        }
    }

    private void setTextForAdjustPrices()
    {
        String text = null;
        if (!mAdjustPricesCheckBox.isChecked()) {
            text = "no adjust prices";
        } else {
            if (mAdjustPricesMileage == null) {
                text = "no adjust prices/no mileage";
            } else {
                if (mAdjustPricesAmount == null)
                    text = "show distances only";
                else {
                    text = String.format(
                        "mileage: %.2f miles/gallon %s %.2f %s"
                        ,
                        mAdjustPricesMileage,
                        mAdjustPricesByDollars ? "with" : "for",
                        mAdjustPricesAmount,
                        mAdjustPricesByDollars ? "dollars" : "gallons"
                    );
                }
            }
        }
        mAdjustPricesTextView.setText(text);
    }


    static class SetAddress extends DialogFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener {
        private static final String ID = "id";
        private static final String TITLE = "title";
        private static final String ADDRESS = "address";

        int mId;
        String mTitle;
        CharSequence mAddress;
        public static SetAddress newInstance(int id, String title, CharSequence address)
        {
            SetAddress f = new SetAddress();
            Bundle args = new Bundle();
            args.putInt(ID, id);
            args.putString(TITLE, title);
            args.putCharSequence(ADDRESS, address);
            f.setArguments(args);
            return f;
        }
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            Bundle args = getArguments();
            mId = args.getInt(ID);
            mTitle = args.getString(TITLE);
            mAddress = args.getCharSequence(ADDRESS);
        }

        EditText mAddressEditText;
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            LayoutInflater inflater = getActivity().getLayoutInflater();
            View lv = inflater.inflate(R.layout.gas_plan_set_address, null);
            CheckBox getLocationCheckBox = (CheckBox) lv.findViewById(R.id.get_location);
            getLocationCheckBox.setOnCheckedChangeListener(this);
            mAddressEditText = (EditText) lv.findViewById(R.id.address);
            mAddressEditText.setText(mAddress);
            Button cancelButton = (Button) lv.findViewById(R.id.cancel);
            cancelButton.setOnClickListener(this);
            Button updateButton = (Button) lv.findViewById(R.id.update);
            updateButton.setOnClickListener(this);
            return new AlertDialog.Builder(getActivity())
                .setTitle(mTitle)
                .setView(lv)
                .create();
        }

        private boolean areAllSet()
        {
            if (mAddressEditText.getText().toString().isEmpty()) {
                GasPlanDialogFragment.showWarning((Context) getActivity(), mTitle + ": not set");
                return false;
            }
            return true;
        }
        @Override   // View.OnClickListener
        public void onClick(View v)
        {
            int id = v.getId();
            if (id == R.id.cancel) {
                SetAddress.this.getDialog().dismiss();
                return;
            }
            if (id != R.id.update) {
                // not implemented yet
                return;
            }
            // id = R.id.update
            if (!areAllSet())
                return;
            // deliver result
            getTargetFragment().onActivityResult(
                getTargetRequestCode(), // REQ_CODE_SET_ADDRESS
                mId,    // resultCode
                new Intent()
                    .putExtra(ADDRESS, mAddressEditText.getText())
            );
            SetAddress.this.getDialog().dismiss();
        }
        @Override   // CompoundButton.OnCheckedChangeListener
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
        {
            int id = buttonView.getId();
            if (id != R.id.get_location) {
                // not implements yet
                return;
            }
            // id = R.id.get_location
            if (!isChecked) {
                mAddressEditText.setText("");
                return;
            }
/** disable google services
            if (LoadActivity.mGoogleApiClient == null) {
                mAddressEditText.setText("GoogleApiClient is null");
                return;
            }
 */
            int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable((Context) getActivity());
            if (ConnectionResult.SUCCESS != resultCode) {
                mAddressEditText.setText("" + resultCode + ": google location service not available");
                return;
            }
/** disable google services
            Location currentLocation = LoadActivity.mLocationClient.getLastLocation();
            mAddressEditText.setText(String.format(
                "%.8f,%.8f",
                currentLocation.getLatitude(),
                currentLocation.getLongitude()
            ));
 */
            //(new SetAddress.GetAddressTask((Context) getActivity())).execute(currentLocation);
        }

        /**
         * An AsyncTask that calls getFromLocation() in the background.
         * The class uses the following generic types:
         * Location - A {@link android.location.Location} object containing the current location,
         *            passed as the input parameter to doInBackground()
         * Void     - indicates that progress units are not used by this subclass
         * String   - An address passed to onPostExecute()
         */
        protected class GetAddressTask extends AsyncTask<Location, Void, String> {

            // Store the context passed to the AsyncTask when the system instantiates it.
            Context localContext;

            // Constructor called by the system to instantiate the task
            public GetAddressTask(Context context) {

                // Required by the semantics of AsyncTask
                super();

                // Set a Context for the background task
                localContext = context;
            }

            /**
             * Get a geocoding service instance, pass latitude and longitude to it, format the returned
             * address, and return the address to the UI thread.
             */
            @Override
            protected String doInBackground(Location... params) {
                /*
                 * Get a new geocoding service instance, set for localized addresses. This example uses
                 * android.location.Geocoder, but other geocoders that conform to address standards
                 * can also be used.
                 */
                Geocoder geocoder = new Geocoder(localContext, Locale.getDefault());

                // Get the current location from the input parameter list
                Location location = params[0];

                // Create a list to contain the result address
                List<Address> addresses = null;

                // Try to get an address for the current location. Catch IO or network problems.
                try {

                    /*
                     * Call the synchronous getFromLocation() method with the latitude and
                     * longitude of the current location. Return at most 1 address.
                     */
                    addresses = geocoder.getFromLocation(location.getLatitude(),
                        location.getLongitude(), 1
                    );

                    // Catch network or other I/O problems.
                    } catch (IOException exception1) {
                        return "IOException: getFromLocation";
                    } catch (IllegalArgumentException exception2) {
                        return "IllegalArgumentException: getFromLocation";
                    }
                    // If the reverse geocode returned an address
                    if (addresses != null && addresses.size() > 0) {
                        Address address = addresses.get(0);
                        details(address);
                        int index = address.getMaxAddressLineIndex();
                        return String.format(
                            "(%d) |%s| |%s|"
                            ,
                            index,
                            index > 0 ? address.getAddressLine(0) : "null",
                            index > 1 ? address.getAddressLine(1) : "null"
                        );
                    } else {
                        return "no address found";
                    }
            }

            /**
             * A method that's called once doInBackground() completes. Set the text of the
             * UI element that displays the address. This method runs on the UI thread.
             */
            @Override
            protected void onPostExecute(String address) {

                // Turn off the progress bar
//              mActivityIndicator.setVisibility(View.GONE);

                // Set the address in the UI
//              mAddress.setText(address);
                mAddressEditText.setText(address);
            }
            private void details(Address addr)
            {
                if (addr == null)
                    Log.i(TAG, "addr is null");

                Log.i(TAG, "getAdminArea() returns " + addr.getAdminArea());
                Log.i(TAG, "getCountryCode() returns " + addr.getCountryCode());
                Log.i(TAG, "getCountryName() returns " + addr.getCountryName());
                Log.i(TAG, "getFeatureName() returns " + addr.getFeatureName());
                Log.i(TAG, "getLocality() returns " + addr.getLocality());
                for (int i = 0; i < addr.getMaxAddressLineIndex(); ++i)
                    Log.i(TAG, "getAddressLine(" + i + ") returns " + addr.getAddressLine(i));
                Log.i(TAG, "getPhone() returns " + addr.getPhone());
                Log.i(TAG, "getPostalCode() returns " + addr.getPostalCode());
                Log.i(TAG, "getPremises() returns " + addr.getPremises());
                Log.i(TAG, "getSubAdminArea() returns " + addr.getSubAdminArea());
                Log.i(TAG, "getSubLocality() returns " + addr.getSubLocality());
                Log.i(TAG, "getSubThoroughfare() returns " + addr.getSubThoroughfare());
                Log.i(TAG, "getThoroughfare() returns " + addr.getThoroughfare());
                Log.i(TAG, "(" + addr.getLatitude() + ", " + addr.getLongitude() + ")");
            }
        }
    }

    static class AdjustPrices extends DialogFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener, TextWatcher {
        private static final String ID = "id";
        private static final String MILEAGE = "mileage";
        private static final String BY_DOLLARS = "by dollars";
        private static final String AMOUNT = "amount";

        EditText mMileageEditText;
        EditText mDollarsEditText;
        EditText mGallonsEditText;
        RadioButton mDollarsRadioButton;
        RadioButton mGallonsRadioButton;

        int mId;
        Float mMileage;
        boolean mByDollars;
        Float mAmount;
        public static AdjustPrices newInstance(int id, Float mileage, boolean byDollars, Float amount)
        {
            AdjustPrices f = new AdjustPrices();
            Bundle args = new Bundle();
            args.putInt(ID, id);
            args.putFloat(MILEAGE, mileage == null ? -1.f : mileage);
            args.putBoolean(BY_DOLLARS, byDollars);
            args.putFloat(AMOUNT, amount == null ? -1.f : amount);
            f.setArguments(args);
            return f;
        }
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            Bundle args = getArguments();
            mId = args.getInt(ID);
            float f;
            f = args.getFloat(MILEAGE, -1.f);
            mMileage = f < 0.f ? null : new Float(f);
            f = args.getFloat(AMOUNT, -1.f);
            mAmount = f < 0.f ? null : new Float(f);
            mByDollars = args.getBoolean(BY_DOLLARS);
        }
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            LayoutInflater inflater = getActivity().getLayoutInflater();
            View lv = inflater.inflate(R.layout.gas_plan_adjust_prices, null);
            mMileageEditText = (EditText) lv.findViewById(R.id.mileage_text);
            mMileageEditText.setText(mMileage == null ? "" : mMileage.toString());
            mDollarsRadioButton = (RadioButton) lv.findViewById(R.id.dollars);
            mDollarsRadioButton.setChecked(mByDollars);
            mDollarsEditText = (EditText) lv.findViewById(R.id.dollars_text);
            mDollarsEditText.setText(mByDollars ? (mAmount == null ? "" : mAmount.toString()) : "");
            mDollarsEditText.setEnabled(mByDollars);
            mGallonsRadioButton = (RadioButton) lv.findViewById(R.id.gallons);
            mGallonsRadioButton.setChecked(!mByDollars);
            mGallonsEditText = (EditText) lv.findViewById(R.id.gallons_text);
            mGallonsEditText.setText(mByDollars ? "" : (mAmount == null ? "" : mAmount.toString()));
            mGallonsEditText.setEnabled(!mByDollars);
            mDollarsRadioButton.setOnCheckedChangeListener(this);
            mGallonsRadioButton.setOnCheckedChangeListener(this);
            mMileageEditText.addTextChangedListener(this);
            mDollarsEditText.addTextChangedListener(this);
            mGallonsEditText.addTextChangedListener(this);
            checkMileage();

            Button cancelButton = (Button) lv.findViewById(R.id.cancel);
            cancelButton.setOnClickListener(this);
            Button updateButton = (Button) lv.findViewById(R.id.update);
            updateButton.setOnClickListener(this);
            return new AlertDialog.Builder(getActivity())
                .setTitle("Adjust Prices")
                .setView(lv)
                .create();
        }
        @Override   // CompoundButton.OnCheckedChangeListener
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
        {
            int id = buttonView.getId();
            if (!isChecked)
                return;
            if (id == R.id.dollars) {
                mDollarsRadioButton.setChecked(true);
                mDollarsEditText.setEnabled(true);
                mGallonsRadioButton.setChecked(false);
                mGallonsEditText.setEnabled(false);
            } else if (id == R.id.gallons) {
                mDollarsRadioButton.setChecked(false);
                mDollarsEditText.setEnabled(false);
                mGallonsRadioButton.setChecked(true);
                mGallonsEditText.setEnabled(true);
            } else {
                // not implemented yet
            }
        }

        private boolean isNumber(Editable e)
        {
            if (e == null)
                return false;
            String s = e.toString();
            if (s.isEmpty())
                return false;
            boolean r = false;
            try {
                Float.parseFloat(s);
                r = true;
            } catch (NumberFormatException ex) {}
            return r;
        }
        private void alertNotANumber(String s)
        {
            GasPlanDialogFragment.showWarning((Context) getActivity(), "|" + s + "|: not a number");
        }
        private void checkMileage()
        {
            Editable e = mMileageEditText.getText();
            boolean enable = isNumber(e);
            if (enable) {
                if (e.toString().isEmpty())
                    enable = false;
            }
            if (mByDollars) {
                mDollarsRadioButton.setEnabled(enable);
                mDollarsEditText.setEnabled(enable);
            } else {
                mGallonsRadioButton.setEnabled(enable);
                mGallonsEditText.setEnabled(enable);
            }
        }
        // TextWatcher
        @Override
        public void afterTextChanged(Editable e)
        {
            checkMileage();
            if (isNumber(e))
                return;
            alertNotANumber(e.toString());
        }
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after)
        {
        }
        @Override
        public void onTextChanged(CharSequence s, int start, int count, int after)
        {
        }

        @Override   // View.OnClickListener
        public void onClick(View v)
        {
            int id = v.getId();
            if (id == R.id.cancel) {
                AdjustPrices.this.getDialog().dismiss();
                return;
            }

            if (id != R.id.update) {
                // not implemented yet
                return;
            }

            // id = R.id.update
            if (!areAllSet())
                return;
            // deliver result
            boolean byDollars = mDollarsRadioButton.isChecked();
            getTargetFragment().onActivityResult(
                getTargetRequestCode(), // REQ_CODE_SET_ADDRESS
                mId,
                new Intent()
                    .putExtra(MILEAGE, mMileageEditText.getText())
                    .putExtra(BY_DOLLARS, byDollars)
                    .putExtra(AMOUNT, byDollars ? mDollarsEditText.getText() : mGallonsEditText.getText())
            );
            AdjustPrices.this.getDialog().dismiss();
        }
        private boolean areAllSet()
        {
            Editable e;
            e = mMileageEditText.getText();
            if (!isNumber(e)) {
                alertNotANumber(e.toString());
                return false;
            }
            boolean byDollars = mDollarsRadioButton.isChecked();
            e = byDollars ? mDollarsEditText.getText() : mGallonsEditText.getText();
            if (!isNumber(e)) {
                alertNotANumber(e.toString());
                return false;
            }
            return true;
        }
    }
}
