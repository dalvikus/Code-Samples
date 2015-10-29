package com.dn2soft.maps;
import com.dn2soft.util.JsonAbs;

import java.util.List;


/**
 * https://developers.google.com/maps/documentation/directions/
 */
public class MapDirection extends JsonAbs {
    public List<Route> routes;
    public String status;

    @Override
    public String toString()
    {
        Float miles = getMiles();
        return miles == null ? null : miles.toString();
    }

    public Float getMiles()
    {
        if (routes.size() == 0)
            return null;
        Route route = routes.get(0);
        if (route.legs.size() == 0)
            return null;
        String text = route.legs.get(0).distance.text;
        // "17.3 mi", "1,234 mi"
        return Float.parseFloat(text.substring(0, text.indexOf(" ")).replaceAll(",", ""));
    }
    public String getStartAddress()
    {
        if (routes.size() == 0)
            return null;
        Route route = routes.get(0);
        if (route.legs.size() == 0)
            return null;
        return route.legs.get(0).start_address;
    }
    public String getEndAddress()
    {
        if (routes.size() == 0)
            return null;
        Route route = routes.get(0);
        if (route.legs.size() == 0)
            return null;
        return route.legs.get(0).end_address;
    }
    public String getStartLocation()
    {
        if (routes.size() == 0)
            return null;
        Route route = routes.get(0);
        if (route.legs.size() == 0)
            return null;
        return route.legs.get(0).start_location.toString();
    }
    public String getEndLocation()
    {
        if (routes.size() == 0)
            return null;
        Route route = routes.get(0);
        if (route.legs.size() == 0)
            return null;
        return route.legs.get(0).end_location.toString();
    }


    public static class Route extends JsonAbs {
        public Bounds bounds;
        public String copyrights;
        public List<Leg> legs;
        public Polyline overview_polyline;
        public String summary;
        public String[] warnings;
        public Integer[] waypoint_order;
    }
    public static class Step extends JsonAbs {
        public Pair distance;
        public Pair duration;
        public Location end_location;
        public String html_instructions;
        public Location start_location;
        public Polyline polyline;
        public String travel_mode;
    }
    public static class Leg extends JsonAbs {
        public Pair distance;
        public Pair duration;
        public String end_address;
        public Location end_location;
        public String start_address;
        public Location start_location;
        public List<Step> steps;
        public List<String> via_waypoint;
    }
    public static class Polyline extends JsonAbs {
        public String points;
    }
    public static class Location extends JsonAbs {
        public float lat;
        public float lng;

        @Override
        public String toString()
        {
            return "" + lat + "," + lng;
            //return "lattitude: " + lat + ", longitude: " + lng;
        }
    }
    public static class Bounds extends JsonAbs {
        public Location northeast;
        public Location southwest;
    }
    public static class Pair extends JsonAbs {
        public String text;
        public int value;

        @Override
        public String toString()
        {
            return "text: |" + text + "|, value: <" + value + ">";
        }
    }


    public static void main(String[] args)
    {
        System.out.println("see DrivingDistance class for an example");
    }
}
