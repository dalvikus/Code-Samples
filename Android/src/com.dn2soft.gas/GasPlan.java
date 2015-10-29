package com.dn2soft.gas;
import com.dn2soft.gas.GasStation.StationInfo;

import java.util.List;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Collections;

public class GasPlan {
    public interface OnItemAddListener {
        public void itemAdded(GasStationTrip gst);
    }
    OnItemAddListener mListener = null;
    public void setOnItemAddListener(OnItemAddListener listener)
    {
        mListener = listener;
    }

    public static StationInfo[] sia = new StationInfo[] { // ID, Comment
        // near 92620
        new StationInfo(17855, "Mobil"),
        new StationInfo(26902, "Mobil"),

        new StationInfo(97025, "Cheveron"),
        new StationInfo(22660, "Cheveron"),
        new StationInfo(22661, "Cheveron"),
        new StationInfo(26901, "Cheveron"),

        new StationInfo(24082, "Shell"),
        new StationInfo(24083, "Shell"),
        new StationInfo(26905, "Shell"),

        new StationInfo(35015, "ARCO"),
        new StationInfo(35015, "ARCO"),
        new StationInfo(36464, "ARCO"),
        new StationInfo(26900, "ARCO"),

        new StationInfo(34329, "76"),

        new StationInfo(26903, "Mc Fadden Gas"),
        new StationInfo(29558, "Grand Gas & Mini Mart"),
        new StationInfo(89256, "OC Gas")
/*
        // near 92129
        new StationInfo(30791, "OAAI: cash = credit card"),         // 11.5
        new StationInfo(29845, "Han's Auto"),                       // 11.0
        new StationInfo(18186, "C Stop"),                           // 11.0
        new StationInfo(18187, "Vons"),                             // 10.1
        new StationInfo(12302, "Varso"),                            // 9.4
        new StationInfo(32501, "Rancho Bernado ARCO"),              // 4.5
        new StationInfo(19669, "We Got It: cash = credit card"),    // 2.4
        new StationInfo(10044, "Sabre Springs"),                    // 3.3
        new StationInfo(26742, "near 99 cents"),                    // 5.4
        new StationInfo(12396, "Poway ARCO"),                       // 5.8
        new StationInfo( 5284, "Ranoma ARCO"),                      // 18.4
        new StationInfo(12303, "Balboa ARCO"),                      // 13.9
        new StationInfo(32500, "near Target, Poway"),               // 3.4
        new StationInfo(27790, "shell in Carmel Mountain"),         // 1.7
        new StationInfo(28407, "another shell in Carmel Mountain")  // 2.7
 */
    };
    final GasStationTrip.TripPoints mTripPoints;
    final GasStationTrip.Car mCar;
    public List<GasStationTrip> gstal;
    public GasPlan(GasStationTrip.TripPoints tpts, GasStationTrip.Car car) {
        mTripPoints = tpts;
        mCar = car;
        gstal = new ArrayList<GasStationTrip>();
    }
    public void set()
    {
        set(false);
    }
    public void set(boolean verbose)
    {
        set(GasPlan.sia, verbose);
    }
    public void set(StationInfo[] sia, boolean verbose)
    {
        if (sia == null)
            return;
        gstal = new ArrayList<GasStationTrip>(sia.length);
        for (int i = 0; i < sia.length; ++i) {
            if (verbose)
                System.out.print(".");
            GasStation gs = GasStation.newInstance(sia[i]);
            GasStationTrip gst = new GasStationTrip(mTripPoints, gs, mCar);
            gst.set();  // set distances and prices
            if (mListener != null)
                mListener.itemAdded(gst);
            gstal.add(gst);
        }
        if (!verbose)
            return;
        System.out.print("\r");
        for (int i = 0; i < sia.length; ++i)
            System.out.print(" ");
        System.out.print("\r");
    }
    public void sort(final boolean byCash)
    {
        Collections.sort(
            gstal,
            new Comparator<GasStationTrip>() {
                @Override
                public int compare(GasStationTrip gstA, GasStationTrip gstB) {
                    if (mCar == null || mCar.mileage == null)
                        return GasPlan.compareFloat(gstA.mGasStation.getCashRegularPrice(), gstB.mGasStation.getCashRegularPrice());
                    if (mCar.dollars != null) {
                        if (byCash)
                            return GasPlan.compareFloat(gstA.adjustedRegularCashPriceByDollars, gstB.adjustedRegularCashPriceByDollars);
                        return GasPlan.compareFloat(gstA.adjustedRegularCreditCardPriceByDollars, gstB.adjustedRegularCreditCardPriceByDollars);
                    }
                    if (byCash)
                        return GasPlan.compareFloat(gstA.adjustedRegularCashPriceByGallons, gstB.adjustedRegularCashPriceByGallons);
                    return GasPlan.compareFloat(gstA.adjustedRegularCreditCardPriceByGallons, gstB.adjustedRegularCreditCardPriceByGallons);
                }
            }
        );
    }
    private static int compareFloat(Float a, Float b)
    {
        return a == b ? 0 : (a < b ? -1 : 1);
    }


    public static void main(String[] args)
    {
        if (args.length == 0) {
            System.out.println("Here There mileage dollars gallons byCash");
            System.out.println("    byCash: true/false");
            System.out.println("    There could be \"\".");
            return;
        }
        GasStationTrip.TripPoints tpts = new GasStationTrip.TripPoints(
            null,
            args.length > 0 ? (args[0].isEmpty() ? GasStationTrip.UCSD : (args[0].toUpperCase().equals("HOME") ? GasStationTrip.HOME_LOC : args[0])) : GasStationTrip.UCSD,
            args.length > 1 ? (args[1].isEmpty() ? null : args[1]) : null
        );
        GasStationTrip.Car car = new GasStationTrip.Car(
            args.length > 2 ? Float.parseFloat(args[2]) : null,
            args.length > 3 ? (args[3].isEmpty() ? null : Float.parseFloat(args[3])) : null,
            args.length > 4 ? Float.parseFloat(args[4]) : null
        );
        GasPlan gas = new GasPlan(tpts, car);
        StationInfo[] sia = new StationInfo[] { // ID, Comment
            new StationInfo(30791, "OAAI: cash = credit card"),         // 11.5
            new StationInfo(29845, "Han's Auto"),                       // 11.0
            new StationInfo(18187, "Vons"),                             // 10.1
            new StationInfo(12302, "Varso"),                            // 9.4
            new StationInfo(19669, "We Got It: cash = credit card"),    // 2.4
            new StationInfo(32500, "near Target, Poway"),               // 3.4
            new StationInfo(27790, "shell in Carmel Mountain"),         // 1.7
            new StationInfo(28407, "another shell in Carmel Mountain")  // 2.7
        };
        gas.set(sia, true); // verbosely
        boolean byCash = args.length > 5 ? (args[5].isEmpty() ? false : Boolean.parseBoolean(args[5])) : false;
        gas.sort(byCash);

        for (GasStationTrip gst: gas.gstal)
            gst.print(true);
    }
}
