package com.dn2soft.gas.app;
import com.dn2soft.gas.GasStation;
import com.dn2soft.gas.GasStationTrip;
import com.dn2soft.gas.GasPlan;
import com.dn2soft.dicka.R;

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

import android.content.Intent;
import android.os.AsyncTask;
import java.util.List;


public class GasPlanMenuDialogFragment extends DialogFragment {
    private static final String TAG = "GasPlanMenuDialogFragment";
    private static final int REQ_CODE_GAS_PLAN = 0;

    public static GasPlanMenuDialogFragment newInstance()
    {
        GasPlanMenuDialogFragment fragment = new GasPlanMenuDialogFragment();
        return fragment;
    }


    ListView mListView;
    Button mCancelBtn;
    Button mGasPlanBtn;
    ArrayAdapterGas mAdapter;

    private static class ArrayAdapterGas extends ArrayAdapter<GasStationTrip> {
        private final LayoutInflater mInflater;

        boolean mByCash;
        public ArrayAdapterGas(Context context) {
            super(context, android.R.layout.simple_list_item_1);
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mByCash = true;
        }
        public void setByCash(boolean byCash)
        {
            mByCash = byCash;
        }

        public void addData(GasStationTrip gst)
        {
            if (gst != null)
                add(gst);
        }
        public void setData(List<GasStationTrip> entries)
        {
            clear();
            if (entries != null)
                addAll(entries);
        }

        static class ViewHolder {
            TextView cashPriceTextView;
            TextView creditCardPriceTextView;
            TextView adjustedPriceTextView;
            TextView stationTextView;
            TextView addressTextView;
        }
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            ViewHolder holder;
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.gas_item, null);
                holder = new ViewHolder();
                holder.cashPriceTextView = (TextView) convertView.findViewById(R.id.cash_price);
                holder.creditCardPriceTextView = (TextView) convertView.findViewById(R.id.credit_card_price);
                holder.adjustedPriceTextView = (TextView) convertView.findViewById(R.id.adjusted_price);
                holder.stationTextView = (TextView) convertView.findViewById(R.id.station);
                holder.addressTextView = (TextView) convertView.findViewById(R.id.address);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }
            GasStationTrip gst = getItem(position);
            GasStation gs = gst.gasStation();
            holder.cashPriceTextView.setText(String.valueOf(gs.getCashRegularPrice()));
            holder.creditCardPriceTextView.setText(String.valueOf(gs.getCreditCardRegularPrice()));
            Float adjustedPrice = gst.getAdjustedPrice(mByCash);
            holder.adjustedPriceTextView.setText(adjustedPrice == null ? "N/A" : String.format("%.2f", adjustedPrice));
            holder.stationTextView.setText(gs.getStation());
            holder.addressTextView.setText(gs.getAddress());
            return convertView;
        }
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
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View gasPlanLayoutView = inflater.inflate(R.layout.gas_list, null);

        mListView = (ListView) gasPlanLayoutView.findViewById(android.R.id.list);
        mListView.setVisibility(View.VISIBLE);

        mListView.setOnItemClickListener(
            new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> lv, View v, int position, long id)
                {
                }
            }
        );
        TextView emptyText = (TextView) gasPlanLayoutView.findViewById(android.R.id.empty);
        mListView.setEmptyView(emptyText);
        mAdapter = new ArrayAdapterGas((Context) getActivity());
        mListView.setAdapter(mAdapter);
        setData();

        mCancelBtn = (Button) gasPlanLayoutView.findViewById(R.id.cancel);
        mGasPlanBtn = (Button) gasPlanLayoutView.findViewById(R.id.gas_plan);
        mCancelBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    GasPlanMenuDialogFragment.this.getDialog().cancel();
                }
            }
        );
        mGasPlanBtn.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    DialogFragment gp = GasPlanDialogFragment.newInstance(
                        mHomeCharSeq,
                        mHereCharSeq,
                        mThereCharSeq,
                        mMileage,
                        mByDollars,
                        mAmount
                    );
                    gp.setTargetFragment(GasPlanMenuDialogFragment.this, REQ_CODE_GAS_PLAN);
                    gp.setCancelable(false);
                    gp.show(getFragmentManager(), "gas plan");
                }
            }
        );

        return new AlertDialog.Builder(getActivity())
            .setCancelable(false)
            .setView(gasPlanLayoutView)
            .create();
    }
    CharSequence mHomeCharSeq;
    CharSequence mHereCharSeq;
    CharSequence mThereCharSeq;
    Float mMileage;
    boolean mByDollars = true;
    Float mAmount;
    int mSortBy = GasPlanDialogFragment.SORT_BY_CASH;
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode != REQ_CODE_GAS_PLAN) {
            // not implemented yet
            return;
        }

        // REQ_CODE_GAS_PLAN
        mSortBy = resultCode;
        mHomeCharSeq = data.getCharSequenceExtra(GasPlanDialogFragment.HOME);
Log.i(TAG, "Home: |" + mHomeCharSeq + "|");
        mHereCharSeq = data.getCharSequenceExtra(GasPlanDialogFragment.HERE);
Log.i(TAG, "Here: |" + mHereCharSeq + "|");
        mThereCharSeq = data.getCharSequenceExtra(GasPlanDialogFragment.THERE);
Log.i(TAG, "There: |" + mThereCharSeq + "|");
        float mileage = data.getFloatExtra(GasPlanDialogFragment.MILEAGE, -1.f);
Log.i(TAG, "mileage = " + mileage);
        mMileage = mileage < 0.f ? null : new Float(mileage);
        mByDollars = data.getBooleanExtra(GasPlanDialogFragment.BY_DOLLARS, true);
Log.i(TAG, "byDollars? " + mByDollars);
        float amount = data.getFloatExtra(GasPlanDialogFragment.AMOUNT, -1.f);
        mAmount = amount < 0.f ? null : new Float(amount);
Log.i(TAG, "amount = " + amount);

        setData();
    }

    private static class GasPlanWrapper extends AsyncTask<Void, GasStationTrip, GasPlan> implements GasPlan.OnItemAddListener {
        final ArrayAdapterGas mAdapter;
        final GasStationTrip.TripPoints mTripPoints;
        final GasStationTrip.Car mCar;
        final boolean mByCash;
        GasPlanWrapper(
            ArrayAdapterGas adapter,
            GasStationTrip.TripPoints tpts,
            GasStationTrip.Car car,
            boolean byCash
        ) {
            mAdapter = adapter;
            mTripPoints = tpts;
            mCar = car;
            mByCash = byCash;
        }

        @Override
        protected void onProgressUpdate(GasStationTrip... gsta)
        {
            if (gsta != null && gsta.length > 0 && gsta[0] != null) {
                gsta[0].print(true);
                mAdapter.addData(gsta[0]);
            }
        }

        @Override
        protected GasPlan doInBackground(Void...none)
        {
            GasPlan gp = new GasPlan(mTripPoints, mCar);
            gp.setOnItemAddListener(this);
            gp.set();
            return gp;
        }

        @Override
        protected void onPostExecute(GasPlan gp)
        {
            gp.sort(mByCash);
            mAdapter.setByCash(mByCash);
            mAdapter.setData(gp.gstal);
        }

        @Override   // GasPlan.OnItemClickListener
        public void itemAdded(GasStationTrip gst)
        {
            publishProgress(gst);
        }
    }
    private void setData()
    {
        mAdapter.clear();
        GasStationTrip.TripPoints tpts = new GasStationTrip.TripPoints(
            mHomeCharSeq == null ? null : mHomeCharSeq.toString(),
            mHereCharSeq == null ? null : mHereCharSeq.toString(),
            mThereCharSeq == null ? null : mThereCharSeq.toString()
        );
        GasStationTrip.Car car = new GasStationTrip.Car(
            mMileage,
            mByDollars ? mAmount : null,
            mByDollars ? null : mAmount
        );
        boolean byCash = true;
        new GasPlanWrapper(mAdapter, tpts, car, byCash).execute();
    }
}
