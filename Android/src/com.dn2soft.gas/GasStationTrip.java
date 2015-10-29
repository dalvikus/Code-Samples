package com.dn2soft.gas;
import com.dn2soft.maps.MapDirection;
import com.dn2soft.maps.DrivingDistance;
import com.dn2soft.gas.GasStation.StationInfo;

public class GasStationTrip {
    public static final String HOME_LOC = "32.967094,-117.09529";   // "latitude,longitude"
    public static final String UCSD = "UC, San Diego";  // google knows the address for this
    public static final String HOME_STR = "HOME";
    public static final String ROUND_TRIP_STR = "ROUND TRIP";


    public static class TripPoints {
        public String homeStr;  // HOME_STR("HOME"), location("latitude, longitude"), address
        public String hereStr;  // same as homeStr
        public String thereStr; // same as homeStr plus ROUND_TRIP_STR
        public TripPoints(String hs, String hrs, String ths) {
            homeStr = hs;
            hereStr = hrs;
            thereStr = ths;
        }
        public boolean isHomeSet() {
            return homeStr != null && !homeStr.toUpperCase().equals(HOME_STR);
        }
    }
    public static class Car {
        public Float mileage;
        // if both are set, dollars is used
        public Float dollars;
        public Float gallons;
        public Car (Float m, Float d, Float g) {
            mileage = m;
            dollars = d;
            gallons = g;
        }
    }


    public Float inDistance;    // from "here" to station
                                    // only if inDistance is not null
    String hereLocation;            // "latitude,longitude" returned by google
    String hereAddress;             // address returned by google
    public Float outDistance;   // from station to "there"
                                    // only if outDistance is not null
    String thereLocation;           // "latitude,longitude" returned by google
    String thereAddress;            // address returned by google

    public String stationLocation;  // "latitude,longitude" returned by google
    public String stationAddress;   // address returned by google
    public Float adjustedRegularCashPriceByDollars;
    public Float adjustedRegularCreditCardPriceByDollars;
    public Float adjustedRegularCashPriceByGallons;
    public Float adjustedRegularCreditCardPriceByGallons;

    TripPoints mTripPoints;
    GasStation mGasStation;
    public GasStation gasStation() {return mGasStation;}
    Car mCar;
    public GasStationTrip(TripPoints tpts, GasStation gs, Car car)
    {
        mTripPoints = tpts;
        mGasStation = gs;
        mCar = car;
        setDistance();
        setAdjustedPrice();
    }
    public Float getAdjustedPrice(boolean byCash)
    {
        if (mCar == null)
            return null;
        if (mCar.dollars != null) {
            return byCash ? adjustedRegularCashPriceByDollars
                          : adjustedRegularCreditCardPriceByDollars;
        }
        if (mCar.gallons != null) {
            return byCash ? adjustedRegularCashPriceByGallons
                          : adjustedRegularCreditCardPriceByGallons;
        }
        return null;
    }
    public void set()
    {
        setDistance();
        setAdjustedPrice();
    }
    private void setDistance()
    {
        if (mTripPoints == null || mGasStation == null)
            return;

        // calculate inDistance
        String here = mTripPoints.hereStr;
        if (here == null || here.toUpperCase().equals(HOME_STR)) {
            here = mTripPoints.isHomeSet() ? mTripPoints.homeStr : HOME_LOC;
        }
        String to = mGasStation.getLocation();//gs.getAddress();
        if (to == null)
            return;
        MapDirection inDir = DrivingDistance.getMapDirection(here, to);
        if (inDir == null)
            return;
        inDistance = inDir.getMiles();
        hereAddress = inDir.getStartAddress();
        hereLocation = inDir.getStartLocation();
        stationLocation = inDir.getEndLocation();
        stationAddress = inDir.getEndAddress();

        // calculate outDistance
        String there = mTripPoints.thereStr;
        if (there == null || there.toUpperCase().equals(HOME_STR)) {
            there = mTripPoints.isHomeSet() ? mTripPoints.homeStr : HOME_LOC;
        }
        if (there.equals(ROUND_TRIP_STR)) {
            outDistance = inDistance;
        } else {
            MapDirection outDir = DrivingDistance.getMapDirection(to, there);
            if (outDir == null) {
                // invalidate inDistance too
                inDistance = null;
                hereLocation = null;
                hereAddress = null;
                return;
            }
            outDistance = outDir.getMiles();
            thereLocation = outDir.getEndLocation();
            thereAddress = outDir.getEndAddress();
        }
    }
    private void setAdjustedPrice()
    {
        if (mGasStation == null)
            return;
        if (mCar == null || mCar.mileage == null || mCar.mileage < 1.f)
            return;
        Float mileage = mCar.mileage;
        if (mCar.dollars == null && mCar.gallons == null)
            return;
        if (inDistance == null || outDistance == null)
            return;

        float cashRegularPrice = mGasStation.getCashRegularPrice();
        boolean isValidCashPrice = cashRegularPrice > 1.f;
        float creditCardRegularPrice = mGasStation.getCreditCardRegularPrice();
        boolean isValidCreditCardPrice = creditCardRegularPrice > 1.f;
        Float dollars = mCar.dollars;
        if (dollars != null) {
            if (isValidCashPrice)
                adjustedRegularCashPriceByDollars = dollars / (dollars / cashRegularPrice - (inDistance + outDistance) / mileage);
            if (isValidCreditCardPrice)
                adjustedRegularCreditCardPriceByDollars = dollars / (dollars / creditCardRegularPrice - (inDistance + outDistance) / mileage);
        }
        Float gallons = mCar.gallons;
        if (gallons != null) {
            if (isValidCashPrice)
                adjustedRegularCashPriceByGallons = (cashRegularPrice * gallons) / (gallons - (inDistance + outDistance) / mileage);
            if (isValidCreditCardPrice)
                adjustedRegularCreditCardPriceByGallons = (creditCardRegularPrice * gallons) / (gallons - (inDistance + outDistance) / mileage);
        }
    }


    public void print()
    {
        print(false);
    }
    public void print(boolean location)
    {
        if (location) {
            if (hereAddress != null)
                System.out.println(String.format("Here: |%s%s|", hereAddress, hereLocation == null ? "" : (" (" + hereLocation + ")")));
            if (thereAddress != null)
                System.out.println(String.format("There: |%s%s|", thereAddress, thereLocation == null ? "" : (" (" + thereLocation + ")")));
        }
        System.out.println("inDistance: " + inDistance);
        System.out.println("outDistance: " + outDistance);
        mGasStation.print();
        if (stationAddress != null)
            System.out.println(String.format("Station: |%s%s|", stationAddress, stationLocation == null ? "" : (" (" + stationLocation + ")")));
        System.out.println("adjustedRegularCashPriceByDollars: " + adjustedRegularCashPriceByDollars);
        System.out.println("adjustedRegularCreditCardPriceByDollars: " + adjustedRegularCreditCardPriceByDollars);
        System.out.println("adjustedRegularCashPriceByGallons: " + adjustedRegularCashPriceByGallons);
        System.out.println("adjustedRegularCreditCardPriceByGallons: " + adjustedRegularCreditCardPriceByGallons);
    }


    public static void main(String[] args)
    {
        if (args.length == 0) {
            System.out.println("ID Comment Here There mileage dollars gallons");
            System.out.println("    For example, ID = 19669; We Got It!");
            System.out.println("    Comment, There could be \"\".");
            return;
        }
        GasStation gs = GasStation.newInstance(new StationInfo(Integer.parseInt(args[0]), args.length > 1 ? (args[1].isEmpty() ? null : args[1]) : null));
        TripPoints tpts = new TripPoints(
            null,
            args.length > 2 ? (args[2].isEmpty() ? GasStationTrip.UCSD : (args[2].toUpperCase().equals("HOME") ? GasStationTrip.HOME_LOC : args[2])) : GasStationTrip.UCSD,
            args.length > 3 ? (args[3].isEmpty() ? null : args[3]) : null
        );
        Car car = new Car(
            args.length > 4 ? Float.parseFloat(args[4]) : null,
            args.length > 5 ? (args[5].isEmpty() ? null : Float.parseFloat(args[5])) : null,
            args.length > 6 ? Float.parseFloat(args[6]) : null
        );
        GasStationTrip gst = new GasStationTrip(tpts, gs, car);
        gst.set();  // set distances and prices
        gst.print(true);
    }
}
