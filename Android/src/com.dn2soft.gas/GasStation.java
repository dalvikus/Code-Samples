package com.dn2soft.gas;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.GZIPInputStream;


public class GasStation {
    public static class StationInfo {
        int id;
        String comment;
        public StationInfo(int i, String c) {
            id = i;
            comment = c;
        }
    }
    StationInfo info;
    public GasStation(StationInfo si) {
        info = si;
    }
    public static GasStation newInstance(StationInfo info)
    {
        if (info == null)
            return null;
        String urlStr = "http://www.gasbuddy.com/ajaxpro/GasBuddy_ASPX.GasTempMap,GasBuddy_ASPX.ashx";
        String dataStr = String.format("{\"id\":\"%d\",\"iFeedID\":0}", info.id);
        return GasStation.parse(info, GasStation.fetchByPOST(urlStr, dataStr));
    }

    public String stationIdStr;
    public String addressLine1;
    public String city;
    public String state;
    public String postalCodeStr;
    public Float lattitude;
    public Float longitude;
    public Float postedCashRegularPrice;
    public String postedCashRegularDateStr;

    public Float cashRegularPrice;
    public String cashRegularDateStr;
    public Float creditCardRegularPrice;
    public String creditCardRegularDateStr;

    public String getAddress()
    {
        if (stationIdStr == null)
            return null;
        return addressLine1 + ", " + city + ", " + state + " " + postalCodeStr;
    }
    public String getLocation()
    {
        if (lattitude == null || longitude == null)
            return null;
        return lattitude.toString() + "," + longitude.toString();
    }
    public String getStation()
    {
        return String.format("%s [#%d]%s", stationIdStr, info.id, info.comment == null ? "" : (": |" + info.comment + "|"));
    }
    public float getCashRegularPrice()
    {
        return cashRegularPrice > 0 ? cashRegularPrice : postedCashRegularPrice;
    }
    public float getCreditCardRegularPrice()
    {
        if (creditCardRegularPrice != null && creditCardRegularPrice > 0.f)
            return creditCardRegularPrice;
        if (cashRegularPrice > 0.f)
            return cashRegularPrice;
        return postedCashRegularPrice;
    }
    public void print()
    {
        if (stationIdStr == null) {
            System.out.println("No station with id = " + info.id);
            return;
        }
        System.out.println(getStation());
        System.out.println(getAddress() + " (" + getLocation() + ")");
        System.out.println("    postedCashRegularPrice: " + postedCashRegularPrice);
        System.out.println("    cashRegularPrice: " + cashRegularPrice);
        System.out.println("    creditCardRegularPrice: " + creditCardRegularPrice);
    }

    private static String fetchByPOST(String urlStr, String dataStr)
    {
        //HttpsURLConnection   conn = null;
        HttpURLConnection   conn = null;
        try {
            conn = (HttpURLConnection) (new URL(urlStr).openConnection());
        }
        catch (MalformedURLException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}
        //conn.setInstanceFollowRedirects(false);
        // not enough for "real" location due to multiple redirection
        try {
            //conn.setRequestMethod("GET");
            conn.setRequestMethod("POST");
        }
        catch (ProtocolException e) {e.printStackTrace();}
        // without this, got EOFException on HEAD call in android (emulator only)
        //  http://stackoverflow.com/questions/17638398/androids-httpurlconnection-throws-eofexception-on-head-requests
        //  https://code.google.com/p/android/issues/detail?id=24672
        //conn.setRequestProperty("Accept-Encoding", "");
        try {
            conn.setRequestProperty("Accept-Encoding", "gzip,deflate,sdch");
            conn.setRequestProperty("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:32.0) Gecko/20100101 Firefox/32.0");
            conn.setDoOutput(true);
            byte[] bytes = null;
            String CHARSET = "UTF-8";
            try {
                bytes = dataStr.getBytes(CHARSET);
            }
            catch (UnsupportedEncodingException e) {e.printStackTrace();}
            //conn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
            conn.setRequestProperty("Content-Type", "text/plain; charset=" + CHARSET);
            conn.setRequestProperty("Content-Length", String.valueOf(bytes.length));
            conn.setRequestProperty("X-AjaxPro-Method", "gusd");
            try {
                conn.getOutputStream().write(bytes);
                conn.getOutputStream().flush();
            }
            catch (IOException e) {e.printStackTrace();}
        }
        catch (IllegalStateException e) {e.printStackTrace();}
        // just setting "Accept-Encoding" makes it work as "GET" specified
        try {
            conn.connect(); // not enough for "real" fetch
        }
        catch (IOException e) {e.printStackTrace();}
        int rc = -1;
        try {
            rc = conn.getResponseCode();
        }
        catch (IOException e) {e.printStackTrace();}
        // some action needed to get page
        //System.out.println("rc = " + rc);
        // or...
        //println(conn.getContentType());
        //println(conn.getHeaderField("Content-Encoding"));
        //println(conn.getHeaderField("Content-Length"));
        if (rc != 200)
            return null;
        String contentType = conn.getHeaderField("Content-Type");
        //System.out.println(conn.getHeaderField("Content-Type"));
        // application/json; charset=UTF-8
        String CHARSET = "CHARSET";
        String charsetName = null;
        if (contentType != null) {
            int index = contentType.toUpperCase().indexOf(CHARSET);
            if (index >= 0) {
                charsetName = contentType.substring(index + CHARSET.length() + 1);
                //System.out.println("|" + charsetName + "|");
            }
        }
        String jsonStr = null;
        try {
            GZIPInputStream gzis = new GZIPInputStream(new BufferedInputStream(conn.getInputStream()));
            StringBuilder sb = new StringBuilder();
            final int BUF_LEN = 8192;
            byte[] buffer = new byte[BUF_LEN];
            int byteRead;
            while ((byteRead = gzis.read(buffer, 0, BUF_LEN)) >= 0) {
                sb.append(new String(buffer, 0, byteRead, charsetName));
            }
            gzis.close();
            jsonStr = sb.toString();
        }
        catch (IOException e) {e.printStackTrace();}
        finally {
            conn.disconnect();
        }
        return jsonStr;
    }

    private static GasStation parse(StationInfo info, String contentStr)
    {
        if (contentStr == null)
            return null;

        GasStation gs = new GasStation(info);

        // new Date(2012,10,3,18,37,12,957)
        // to make a comma separator in each bracket
        String x = contentStr.replaceAll("new Date\\((\\d+),(\\d+),(\\d+),(\\d+).*?\\)", "$1-$2-$3 $4:00");
        //System.out.print(x);
        Pattern p0 = Pattern.compile("new GBAjaxPro.Web.DataTable\\((.*?)\\)");
        Pattern p1 = Pattern.compile("\\[([^\\[]*?)\\]");
        Matcher m0 = p0.matcher(x);
/*
        while (m.find()) {
            Matcher m1 = p1.matcher(m.group(1));
            while (m1.find()) {
                System.out.println(m1.group(1));
                String[] sa = m1.group(1).split(",");
                for (String s: sa) {
                    System.out.println("|" + s + "|");
                }
            }
        }
 */
        if (m0.find()) {
            Matcher m1 = p1.matcher(m0.group(1));
            while (m1.find()) {
                //System.out.println(m1.group(1));
                String[] sa = m1.group(1).split(",");
                for (String s: sa) {
                    //System.out.println("|" + s + "|");
                }
                if (sa.length > 2) {
/*
System.out.println("[0]: |" + sa[0].substring(1, sa[0].length() - 1)  + "|");     // ["station_nm","System.String"],
System.out.println("[1]: |" + sa[1].substring(1, sa[1].length() - 1) + "|");     // ["station_alias","System.String"],
System.out.println("[2]: |" + sa[2].substring(1, sa[2].length() - 1) + "|");     // ["address","System.String"],
System.out.println("[3]: |" + sa[3].substring(1, sa[3].length() - 1) + "|");     // ["city","System.String"],
System.out.println("[4]: |" + sa[4].substring(1, sa[4].length() - 1) + "|");     // ["state","System.String"],
System.out.println("[5]: |" + sa[5].substring(1, sa[5].length() - 1) + "|");     // ["postal_code","System.String"],
System.out.println("[29]: |" + sa[29] + "|");   // ["reg_price","System.Decimal"],
System.out.println("[30]: |" + sa[30] + "|");   // ["reg_tme","System.DateTime"],
 */
                    String s;
                    s = sa[0];
                    gs.stationIdStr = s.substring(1, s.length() - 1);
                    s = sa[1];
                    if (s.length() > 2)
                        gs.stationIdStr += " (" + s.substring(1, s.length() - 1) + ")";
                    s = sa[2]; gs.addressLine1 = s.substring(1, s.length() - 1);
                    s = sa[3]; gs.city = s.substring(1, s.length() - 1);
                    s = sa[4]; gs.state = s.substring(1, s.length() - 1);
                    s = sa[5]; gs.postalCodeStr = sa[5].substring(1, sa[5].length() - 1);
                    gs.postedCashRegularPrice = new Float(sa[29]);
                    gs.postedCashRegularDateStr = sa[30];
                    gs.lattitude = new Float(sa[52]);
                    gs.longitude = new Float(sa[53]);
                }
            }
        }
        m0.find();  // skip second part
/*
        {
            Matcher m1 = p1.matcher(m0.group(1));
            while (m1.find()) {
                System.out.println(m1.group(1));
                String[] sa = m1.group(1).split(",");
                for (String s: sa) {
                    System.out.println("|" + s + "|");
                }
            }
        }
 */
        if (m0.find()) {
            Matcher m1 = p1.matcher(m0.group(1));
            int index = 0;
            while (m1.find()) {
                //System.out.println(m1.group(1));
                String[] sa = m1.group(1).split(",");
                for (String s: sa) {
                    //System.out.println("|" + s + "|");
                }
                if (sa.length > 2) {
                    if (index == 0) {           // Regular Cash
/*
System.out.println("Regular Cash");
System.out.println("[0]: |" + sa[0] + "|");     // ["price","System.Decimal"],
System.out.println("[2]: |" + sa[2] + "|");     // ["tme","System.DateTime"],
 */
                        gs.cashRegularPrice = new Float(sa[0]);
                        gs.cashRegularDateStr = sa[2];
                    } else if (index == 4) {    // Regular Credit Card
/*
System.out.println("Regular Credit Card");
System.out.println("[0]: |" + sa[0] + "|");     // ["price","System.Decimal"],
System.out.println("[2]: |" + sa[2] + "|");     // ["tme","System.DateTime"],
 */
                        gs.creditCardRegularPrice = new Float(sa[0]);
                        gs.creditCardRegularDateStr = sa[2];
                    }

                    ++index;
                }
            }
        }
        return gs;
    }


    public static void main(String[] args)
    {
        if (args.length == 0) {
            System.out.println("ID Comment");
            System.out.println("    For example, ID = 19669; We Got It!");
            System.out.println("    Comment could be \"\".");
            return;
        }
        GasStation gs = GasStation.newInstance(new StationInfo(Integer.parseInt(args[0]), args.length > 1 ? (args[1].isEmpty() ? null : args[1]) : null));
        gs.print();
    }
}
