package com.dn2soft.maps;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLEncoder;

import javax.net.ssl.HttpsURLConnection;

import java.util.zip.GZIPInputStream;


/**
 * get driving information from google apis
 */
public class DrivingDistance {
    public static final String HOME = "32.967094,-117.09529";
    public static MapDirection getMapDirection(String fromAddrStr, String toAddrStr)
    {
        String  urlStr = null;
        try {
            urlStr = String.format("https://maps.googleapis.com/maps/api/directions/json?origin=%s&destination=%s", URLEncoder.encode(fromAddrStr, "UTF-8"), URLEncoder.encode(toAddrStr, "UTF-8"));
        }
        catch (UnsupportedEncodingException e) {e.printStackTrace();}
        ////System.out.println(urlStr);
        if (urlStr == null)
            return null;

        String jsonStr = DrivingDistance.getJsonStr(urlStr);
        if (jsonStr == null)
            return null;
        return (MapDirection) new MapDirection().set(jsonStr, MapDirection.class);
    }
    private static String getJsonStr(String urlStr)
    {
        HttpsURLConnection   conn = null;
        try {
            conn = (HttpsURLConnection) (new URL(urlStr).openConnection());
        }
        catch (MalformedURLException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}
        //conn.setInstanceFollowRedirects(false);
        // not enough for "real" location due to multiple redirection
        try {
            conn.setRequestMethod("GET");
        }
        catch (ProtocolException e) {e.printStackTrace();}
        // without this, got EOFException on HEAD call in android (emulator only)
        //  http://stackoverflow.com/questions/17638398/androids-httpurlconnection-throws-eofexception-on-head-requests
        //  https://code.google.com/p/android/issues/detail?id=24672
        //conn.setRequestProperty("Accept-Encoding", "");
        try {
            conn.setRequestProperty("Accept-Encoding", "gzip,deflate,sdch");
            conn.setRequestProperty("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:32.0) Gecko/20100101 Firefox/32.0");
        }
        catch (IllegalStateException e) {e.printStackTrace();}
        //conn.setRequestProperty("Accept-Encoding", "gzip");
    ////    conn.setRequestMethod("GET");
    ////    conn.setRequestProperty("Accept-Encoding", "gzip");
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
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            conn.disconnect();
        }
        return jsonStr;
    }


    public static void main(String[] args)
    {
        if (args.length == 0) {
            System.out.println("There Here");
            System.out.println("    Here could be \"\": (" + DrivingDistance.HOME + ").");
            return;
        }
        String toAddr = args.length > 1 ? args[0] : "5793 Alton Pkwy, Irvine, CA 92618-4057";
        String fromAddr = args.length > 2 ? (args[1].isEmpty() || args[1].equals("HOME") ? DrivingDistance.HOME : args[1]) : DrivingDistance.HOME;
        MapDirection dir = DrivingDistance.getMapDirection(fromAddr, toAddr);
        System.out.println("Status: " + dir.status);
        System.out.println("  From: " + dir.getStartAddress());
        System.out.println("    To: " + dir.getEndAddress());
        System.out.println(" Miles: " + dir);
    }
}
