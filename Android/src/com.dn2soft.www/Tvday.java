package com.dn2soft.www;


import java.util.regex.Pattern;
import java.util.regex.Matcher;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.TextNode;
import org.jsoup.select.Elements;

import java.util.Map;
import java.util.List;
import java.util.ArrayList;

import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLEncoder;

import java.io.BufferedReader;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.UnsupportedEncodingException;


public class Tvday {
    private static final String IDX_MATCH_STR = ".*?showplayer\\((\\d+),2\\).*?";
    private static final Pattern IDX_PATTERN = Pattern.compile(IDX_MATCH_STR);
    private static final String TITLE_MATCH_STR = "(.*?)\\s+\\xa0(\\d+).*";
    private static final Pattern TITLE_PATTERN = Pattern.compile(TITLE_MATCH_STR);
    private static final String DATE_MATCH_STR = ".*?(\\d{4}-\\d{2}-\\d{2})\\s+(\\d{2}:\\d{2}:\\d{2}).*?";
    private static final Pattern DATE_PATTERN = Pattern.compile(DATE_MATCH_STR);
    private static final String MB_MATCH_STR = "MB=(\\d+).*";
    private static final Pattern MB_PATTERN = Pattern.compile(MB_MATCH_STR);
    private static final String MP4_MATCH_STR = ".*?type: \"mp4\", file: \"(.*?)\".*";
    private static final Pattern MP4_PATTERN = Pattern.compile(MP4_MATCH_STR);
    public static String DOWNLOAD_DIRECTORY = ".";

        //  http://216.155.128.99/play.php?key=8090cf0ef5e359ad80aab7e3ef9da49e9dcf392294&ix=81163&ib=2&seq=B2453&seq2=2850338001&iu=1001&ia=
    ////private static final String DOWNLOAD_MATCH_STR = "http://\\d+\\.\\d+\\.\\d+\\.\\d+/play\\.php\\?key=[0-9a-f]{42}&ix=(\\d+)&ib=.*?&seq=.*?&seq2=.*?&iu=(.*?)&ia=";
    private static final String DOWNLOAD_MATCH_STR = "http://\\d+\\.\\d+\\.\\d+\\.\\d+/play\\.php\\?key=[0-9a-f]{42}&ix=(\\d+).*";
    private static final Pattern DOWNLOAD_PATTERN = Pattern.compile(DOWNLOAD_MATCH_STR);
    public static class Entry {
        public int idx;
        public String dateStr;
        public String timeStr;
        public String titleStr;
        public String noStr;
        Entry(int i, String d, String tm, String t, String n) {
            idx = i;
            dateStr = d;
            timeStr = tm;
            titleStr = t;
            noStr = n;
        }
        public void print()
        {
            System.out.println(String.format("%d\t[%s %s] %s%s", idx, dateStr, timeStr, titleStr, noStr == null ? "" : (" [" + noStr + "]")));
        }
    }
    public static void setList(List<Entry> eal, String utf8URLEncodedTitle0, int pageno, boolean recursive, int limit)
    {
        ////String href = String.format("http://korealive.net/list/?cate=&strkey=%s&page=%d", utf8URLEncodedTitle0 == null ? "" : utf8URLEncodedTitle0, pageno);
        String href = String.format("http://liveday.net/list/?cate=&strkey=%s&page=%d", utf8URLEncodedTitle0 == null ? "" : utf8URLEncodedTitle0, pageno);
        System.out.println(href);
        Document doc = null;
        try {
            doc = Jsoup.connect(href).get();
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (doc == null)
            return;
        Element div = doc.select("div#number_box").first().select(".page").first().select(".font_black").first();
        //System.out.println("" + div);
        //System.out.println("" + div.text());
        int npage = -1;
        try {
            npage = Integer.parseInt(div.text().replaceAll(",", ""));
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        if (npage == -1)
            return;
        if (pageno == 1)
            System.out.println("# of pages = " + npage);
        if (npage == 0)
            return;
        for (Element x: doc.select("div#kl-list-box")) {
            //System.out.println("" + x);
            Element y = x.select(".kl-box-content").first();
            Element z = y.select(".kl_list_title").first().select("li").first();
            Matcher m;
            int idx = -1;
            m = IDX_PATTERN.matcher(z.attr("onclick"));
            if (m.matches()) {
                //System.out.println("" + m.group(1));
                try {
                    idx = Integer.parseInt(m.group(1));
                } catch (NumberFormatException e) {}
            }
            String titleStr = z.text();
            String noStr = null;
            Matcher mm = TITLE_PATTERN.matcher(titleStr);
            if (mm.matches()) {
                titleStr = mm.group(1);
                noStr = mm.group(2);
            }
            //System.out.println("|" + titleStr.replaceAll("\u00a0", "...") + "|(" + titleStr.length() + ")");
            Element u = y.select(".kl_list_time").first().select("li").first();
            //System.out.println("" + u.text());
            String dateStr = null;
            String timeStr = null;
            m = DATE_PATTERN.matcher(u.text());
            //System.out.println("" + m);
            if (m.matches()) {
                //System.out.println(m.group(1) + " " + m.group(2));
                dateStr = m.group(1);
                timeStr = m.group(2);
            }
            if (idx != -1 && dateStr != null && titleStr != null)
                eal.add(new Entry(idx, dateStr, timeStr, titleStr, noStr));
        }
        if (!recursive || npage == 1)
            return;
        for (int i = pageno; i < npage; ++i) {
            if (i == limit)
                break;
            setList(eal, utf8URLEncodedTitle0, 1 + i, false, limit);
        }
        return;
    }

    public static String getMBStr(String urlStr, String credentialStr)
    {
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) (new URL(urlStr).openConnection());
        }
        catch (MalformedURLException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}
        try {
            conn.setRequestMethod("POST");
        }
        catch (ProtocolException e) {e.printStackTrace();}
        try {
            conn.setRequestProperty("User-Agent", "curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3");
            conn.setRequestProperty("Accept", "*/*");
            conn.setDoOutput(true);
            byte[] bytes = null;
            String CHARSET = "UTF-8";
            try {
                bytes = credentialStr.getBytes(CHARSET);
            }
            catch (UnsupportedEncodingException e) {e.printStackTrace();}
            conn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
            conn.setRequestProperty("Content-Length", String.valueOf(bytes.length));
            try {
                conn.getOutputStream().write(bytes);
                conn.getOutputStream().flush();
            }
            catch (IOException e) {e.printStackTrace();}
        }
        catch (IllegalStateException e) {e.printStackTrace();}
        try {
            conn.connect(); // not enough for "real" fetch
        }
        catch (IOException e) {e.printStackTrace();}
        int rc = -1;
        try {
            rc = conn.getResponseCode();
        }
        catch (IOException e) {e.printStackTrace();}
        if (rc != 200)
            return null;
        Map<String, List<String>> headers = conn.getHeaderFields();
        for (String cookieStr: headers.get("Set-Cookie")) {
            //System.out.println("Set-Cookie: " + cookieStr);
            Matcher m = MB_PATTERN.matcher(cookieStr);
            if (m.matches()) {
                //System.out.println("MB:= " + m.group(1));
                return m.group(1);
            }
        }
        return null;
    }

    /**
     * Any MB is okay in Cookie though the MB should be gotten through authentification.
     */
    public static String getPlayLinkUrl(int idx)
    {
        if (idx < 0)
            return null;
        ////String urlStr = String.format("http://korealive.net/list/player.php?idx=%d&mode=2", idx);
        String urlStr = String.format("http://liveday.net/list/player.php?idx=%d&mode=2", idx);
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) (new URL(urlStr).openConnection());
        }
        catch (MalformedURLException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}
        try {
            conn.setRequestMethod("GET");
        }
        catch (ProtocolException e) {e.printStackTrace();}
        try {
            conn.setRequestProperty("User-Agent", "curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3");
            conn.setRequestProperty("Accept", "*/*");
////        conn.setRequestProperty("Cookie", "MB=" + MBStr);
            conn.setRequestProperty("Cookie", "MB=1001");
        }
        catch (IllegalStateException e) {e.printStackTrace();}
        try {
            conn.connect(); // not enough for "real" fetch
        }
        catch (IOException e) {e.printStackTrace();}
        int rc = -1;
        try {
            rc = conn.getResponseCode();
        }
        catch (IOException e) {e.printStackTrace();}
        if (rc != 200)
            return null;
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String line;
            while ((line = br.readLine()) != null) {
                //System.out.println("|" + line);
                Matcher m = MP4_PATTERN.matcher(line);
                if (m.matches()) {
                    //System.out.println("" + m.group(1));
                    return m.group(1);
                }
            }
        }
        catch (IOException e) {e.printStackTrace();}
        finally {
            conn.disconnect();
        }
        return null;
    }

    private static int getIdx(String urlStr)
    {
        if (urlStr == null)
            return -1;
        Matcher m = DOWNLOAD_PATTERN.matcher(urlStr);
        if (m.matches())
            return Integer.parseInt(m.group(1));
        return -1;
    }
    public interface DownloadWatcher {
        void setProgress(long current, long total);
        boolean cancelled();
    }
    public static String download(String urlStr, boolean append)
    {
        return download(urlStr, append, null);
    }
    public static String download(int idx, String urlStr, boolean append)
    {
        return download(idx, urlStr, append, null);
    }
    public static String download(String urlStr, boolean append, DownloadWatcher watcher)
    {
        int idx = getIdx(urlStr);
        if (idx == -1)
            return "" + urlStr + ": no idx";
        return download(idx, urlStr, append, watcher);
    }
    public static final String DOWNLOAD_DONE = "Download Done";
    public static final String DOWNLOAD_CANCELLED = "Download Cancelled";
    public static String download(int idx, String urlStr, boolean append, DownloadWatcher watcher)
    {
        if (watcher != null) {
            watcher.setProgress(0, 0);
        }
        long fileSize = 0;
        File outFile = new File(DOWNLOAD_DIRECTORY, "" + idx + ".mp4");
        if (append) {
            fileSize = outFile.length();
            //System.out.println("|" + outFile.getAbsolutePath() + "|: " + fileSize + " bytes");
        }
        boolean partial = append;
        if (append && fileSize == 0)
            partial = false;
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) (new URL(urlStr).openConnection());
        }
        catch (MalformedURLException e) {e.printStackTrace();}
        catch (IOException e) {e.printStackTrace();}
        if (conn == null)
            return "" + urlStr + ": Connection failed";
        try {
            conn.setRequestMethod("GET");
        }
        catch (ProtocolException e) {e.printStackTrace();}
        try {
            conn.setRequestProperty("User-Agent", "curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3");
            conn.setRequestProperty("Accept", "*/*");
            if (append && fileSize > 0)
                conn.setRequestProperty("Range", "bytes=" + fileSize + "-");
        }
        catch (IllegalStateException e) {e.printStackTrace();}
        try {
            conn.connect(); // not enough for "real" fetch
        }
        catch (IOException e) {e.printStackTrace();}
        int rc = -1;
        try {
            rc = conn.getResponseCode();
        }
        catch (IOException e) {e.printStackTrace();}
        if (!(
                (!partial && rc == 200) ||   // OK
                (partial && rc == 206)       // Partial Content
        ))
            return "response code = " + rc;
        //String contentType = conn.getHeaderField("Content-Type");   // "video/mp4"
        String contentLength = conn.getHeaderField("Content-Length");
        long iContentLength = 0;    // partial length
        try {
            iContentLength = Integer.parseInt(contentLength);
        } catch (NumberFormatException e) {}
        long iContentLength0 = fileSize + iContentLength;   // total length
        //String contentRange = conn.getHeaderField("Content-Range");
        Map<String, List<String>> headers = conn.getHeaderFields();
        for (Map.Entry<String, List<String>> entry: headers.entrySet()) {
            System.out.println("\t|" + entry.getKey() + "|: |" + entry.getValue() + "|");
        }
        String returnStr = null;
        try {
            BufferedInputStream bis = new BufferedInputStream(conn.getInputStream());
            BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(outFile, append));
            final int BUF_LEN = 8192;
            byte[] buffer = new byte[BUF_LEN];
            int byteRead;
            long fs = fileSize;
            boolean isCancenlled = false;
            while ((byteRead = bis.read(buffer, 0, BUF_LEN)) >= 0) {
                bos.write(buffer, 0, byteRead);
                fs += byteRead;
                if (watcher != null) {
                    if (watcher.cancelled()) {
                        isCancenlled = true;
                        break;
                    }
                    watcher.setProgress(fs, iContentLength0);
                }
            }
            bis.close();
            bos.close();
            returnStr = isCancenlled ? DOWNLOAD_CANCELLED : DOWNLOAD_DONE;
        } catch (IOException e) {
            e.printStackTrace();
            returnStr = "IOException";
        }
        finally {
            conn.disconnect();
        }
        return returnStr;
    }


    public static void main(String[] args)
    {
        String title0Str = null;
        int idx = -1;
        int limit = -1;
        if (args.length > 0) {
            if (args[0].startsWith("-")) {
                try {
                    limit = Integer.parseInt(args[0]);
                } catch (NumberFormatException e) {}
            } else {
                title0Str = args[0];
                try {
                    idx = Integer.parseInt(title0Str);
                } catch (NumberFormatException e) {}
            }
        }
/*
        System.out.println("idx = " + idx);
        System.out.println("limit = " + limit);
        System.out.println("title0Str: " + title0Str);
 */
        if (idx != -1) {
            String MBStr = null;
////        MBStr = getMBStr("http://korealive.net/my/login.proc.php?mode=login", "userid=99%40g.net&pass=halo");
////        System.out.println("" + MBStr);
            String playUrlStr = getPlayLinkUrl(idx);
            System.out.println("" + playUrlStr);
            return;
        }
        // idx = -1
        // 1) download
        //  http://216.155.128.99/play.php?key=8090cf0ef5e359ad80aab7e3ef9da49e9dcf392294&ix=81163&ib=2&seq=B2453&seq2=2850338001&iu=1001&ia=
        if (title0Str != null && title0Str.startsWith("http://")) {
            download(title0Str, true);
            return;
        }
        // 2) list
        //  title0Str
        List<Entry> eal = new ArrayList<Entry>();
        String utf8URLEncodedTitle0 = null;
        if (title0Str != null) {
            try {
                utf8URLEncodedTitle0 = URLEncoder.encode(title0Str, "UTF-8");
            } catch (UnsupportedEncodingException e) {}
        }
        setList(eal, utf8URLEncodedTitle0, 1, true, -limit);
        for (Entry e: eal)
            e.print();
    }
}
