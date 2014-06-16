package com.dn2soft.dick.dictionary;

import com.dn2soft.dick.utility.MultiLine;
import com.dn2soft.dick.utility.Joint;
import com.dn2soft.dick.utility.AnsiText;
import com.dn2soft.dick.utility.AnsiText.ColorAttr;
import com.dn2soft.dick.utility.Json;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.FileSystems;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;

import java.net.URLEncoder;
import java.net.URL;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;

import java.util.Map;
import java.util.List;
import java.util.ArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.TextNode;
import org.jsoup.select.Elements;

public class Cambridge {
    private static void
    printf(String format, Object... args)
    {
        System.out.printf(format, args);
    }
    private static void
    println(Object o)
    {
        System.out.printf("%s%n", o);
    }

    public static class Flag {
        public boolean  uk          = false;
        public boolean  showAll     = false;
        public boolean  force       = false;
        public boolean  playPron    = false;
        public boolean  showLink    = true;
        public boolean  ansi        = true;
        public boolean  verbose     = false;
        public boolean  trace       = false;
        public boolean  showHref0   = false;
        public boolean  showHref0Only   = false;// if set, showHref0 implied
        public boolean  followWord  = false;// gui mode only
        public boolean  resultOnly  = false;
        public String   dbpath      = null;
    }

    static ColorAttr    COLOR_ATTR_INFO_HEAD = new ColorAttr("yellow", "bold", false);
    static ColorAttr    COLOR_ATTR_INFO = new ColorAttr("yellow", "none", false);
    static ColorAttr    COLOR_ATTR_TRACE_HEAD = new ColorAttr("cyan", "bold", false);
    static ColorAttr    COLOR_ATTR_TRACE = new ColorAttr("cyan", "none", false);
    public static void printInfo(String s)
    {
        println(AnsiText.colorText("INFO:", COLOR_ATTR_INFO_HEAD) + "\t" + AnsiText.colorText(s, COLOR_ATTR_INFO));
    }
    static void printTrace(String s)
    {
        println(AnsiText.colorText("TRACE:", COLOR_ATTR_TRACE_HEAD) + "\t" + AnsiText.colorText(s, COLOR_ATTR_TRACE));
    }

    static int  WIDTH = 72;
    static ColorAttr    COLOR_ATTR_WORD = new ColorAttr("cyan", "bold", false);
    static ColorAttr    COLOR_ATTR_IPA = new ColorAttr("blue", "bold", false);
    static ColorAttr    COLOR_ATTR_POS = new ColorAttr("yellow", "bold", false);
    static ColorAttr    COLOR_ATTR_GW = new ColorAttr("magenta", "bold", false);
    static ColorAttr    COLOR_ATTR_DEF = new ColorAttr("green", "bold", false);
    static ColorAttr    COLOR_ATTR_EXAMPLE_DOT = new ColorAttr("cyan", "bold", false);
    static ColorAttr    COLOR_ATTR_EXAMPLE = new ColorAttr("cyan", "none", false);
    static ColorAttr    COLOR_ATTR_MORE = new ColorAttr("yellow", "none", false);
    public static ColorAttr    COLOR_ATTR_LINK = new ColorAttr("blue", "none", false);
    static ColorAttr    COLOR_ATTR_GLUE = COLOR_ATTR_EXAMPLE_DOT;

    static final String BLOCK_GLUE =
    /*
        "\n" +
        AnsiText.colorText("---- block glue ----", COLOR_ATTR_GLUE) +
     */
        "\n";
    static final String PAGE_MORE_BLOCK_GLUE =
        "\n" +
    /*
        AnsiText.colorText("---- page-more glue ----", COLOR_ATTR_GLUE) +
     */
        "\n";
    static final String MORE_STR_MORE_GLUE =
    /*
        "\n"
        + AnsiText.colorText("---- more-str-more glue ----", COLOR_ATTR_GLUE) +
     */
        "\n";
    static final String PAGE_GLUE =
        "\n" +
    /*
        AnsiText.colorText("==== page glue ====", COLOR_ATTR_GLUE) +
     */
        "\n";
    public static class Block {
        public static class Head {
            public String   word;
            public String   gw;
            public List<String>   pos = new ArrayList<String>();
            public String   uk_mp3, uk_ogg, us_mp3, us_ogg;
            public List<String> ipa = new ArrayList<String>();

    /**
     * JSON
     *  {
     *      "word": "...",
     *      "gw":   "...",
     *      "pos":  ["...", ...],
     *      "pron": ["uk_mp3", "uk_ogg", "us_mp3", "us_ogg"],
     *      "ipa":  ["...", ...],
     *  }
     */
            String  getJSONStr()
            {
                List<String>    str_a = new ArrayList<String>();

                str_a.add(String.format("'word': '%s'", Json.escapeString(word)));
                str_a.add(String.format("'gw': '%s'", Json.escapeString(gw)));
                List<String>    s_a = new ArrayList<String>();
                //s_a.clear();
                for (String s: pos) {
                    s_a.add(String.format("'%s'", Json.escapeString(s)));
                }
                str_a.add(String.format("'pos': [%s]", Joint.join(s_a, ",")));
                s_a.clear();
                s_a.add(String.format("'%s'", Json.escapeString(uk_mp3)));
                s_a.add(String.format("'%s'", Json.escapeString(uk_ogg)));
                s_a.add(String.format("'%s'", Json.escapeString(us_mp3)));
                s_a.add(String.format("'%s'", Json.escapeString(us_ogg)));
                str_a.add(String.format("'pron': [%s]", Joint.join(s_a, ",")));
                s_a.clear();
                for (String s: ipa) {
                    s_a.add(String.format("'%s'", Json.escapeString(s)));
                }
                str_a.add(String.format("'ipa': [%s]", Joint.join(s_a, ",")));
                //return String.format("{\n    %s\n}", Joint.join(str_a, ",\n    "));
                return String.format("{%s}", Joint.join(str_a, ","));
            }
            static String   POS_GLUE = AnsiText.colorText(
            /*
                "-- IG --"
             */
                ","
                , COLOR_ATTR_GLUE
            );
            static String   IPA_GLUE = AnsiText.colorText(
            /*
                "-- IG --"
             */
                ","
                , COLOR_ATTR_GLUE
            );
            static String   HEAD_GLUE =
            /*
                AnsiText.colorText("-- HG --", COLOR_ATTR_GLUE);
             */
                " ";
            public String  getAudioURL()
            {
                if (us_ogg != null && !us_ogg.equals(""))
                    return us_ogg;
                if (us_mp3 != null && !us_mp3.equals(""))
                    return us_mp3;
                if (uk_ogg != null && !uk_ogg.equals(""))
                    return uk_ogg;
                if (uk_mp3 != null && !uk_mp3.equals(""))
                    return uk_ogg;
                return null;
            }
            String  getHeadStr(boolean ansi)
            {
                List<String>    str_list = new ArrayList<String>();
                str_list.add(AnsiText.colorText(word, COLOR_ATTR_WORD, ansi));
                if (!ipa.isEmpty()) {
                    List<String>    s_a = new ArrayList<String>();
                    for (String s: ipa) {
                        s_a.add(AnsiText.colorText("/" + s + "/", COLOR_ATTR_IPA, ansi));
                    }
                    str_list.add(Joint.join(s_a, IPA_GLUE));
                }
/*
                //"à la carte"
                if (pos != null) {
                    str_list.add(AnsiText.colorText("(" + pos + ")", COLOR_ATTR_POS, ansi));
                }
 */
                if (!pos.isEmpty()) {
                    List<String>    s_a = new ArrayList<String>();
                    for (String s: pos) {
                        s_a.add(AnsiText.colorText(s, COLOR_ATTR_POS, ansi));
                    }
                    String  s = AnsiText.colorText("(", COLOR_ATTR_POS, ansi);
                    s += Joint.join(s_a, POS_GLUE);
                    s += AnsiText.colorText(")", COLOR_ATTR_POS, ansi);
                    //str_list.add(Joint.join(s_a, POS_GLUE));
                    str_list.add(s);
                }
                if (gw != null && !gw.isEmpty()) {
                    str_list.add(AnsiText.colorText("[" + gw + "]", COLOR_ATTR_GW, ansi));
                }
                //println(getJSONStr());
                return Joint.join(str_list, HEAD_GLUE);
            }
            void    print() {print(true);}
            void    print(boolean ansi)
            {
                String  str = getHeadStr(ansi);
                if (!str.isEmpty())
                    println(str);
            }
        }
        public static class Body {
            public String   def;
            public List<String> example = new ArrayList<String>();

    /**
     * JSON
     *  {
     *      "def":      "...",
     *      "example":  ["...", ...]
     *  }
     */
            String  getJSONStr()
            {
                List<String>    str_a = new ArrayList<String>();

                str_a.add(String.format("'def': '%s'", Json.escapeString(def)));
                List<String>    s_a = new ArrayList<String>();
                for (String e: example) {
                    s_a.add(String.format("'%s'", Json.escapeString(e)));
                }
                str_a.add(String.format("'example': [%s]", Joint.join(s_a, ",")));
                //return String.format("{\n    %s\n}", Joint.join(str_a, ",\n    "));
                return String.format("{%s}", Joint.join(str_a, ","));
            }

            static final String DEF_MULTILINE_GLUE =
            /*
                "\n" +
                AnsiText.colorText("---- multi-line glue ----", COLOR_ATTR_GLUE) +
             */
                "\n";
            static final String EXAMP_MULTILINE_GLUE =
            /*
                "\n" +
                AnsiText.colorText("---- multi-line glue ----", COLOR_ATTR_GLUE) +
             */
                "\n      ";
            static final String EXAMP_GLUE =
            /*
                "\n" +
                AnsiText.colorText("---- example glue ----", COLOR_ATTR_GLUE) +
             */
                "\n";
            static final String DEF_EXAMP_GLUE =
            /*
                "\n" +
                AnsiText.colorText("---- def-examp glue ----", COLOR_ATTR_GLUE) +
             */
                "\n";
            String  getBodyStr(boolean ansi)
            {
                List<String>    str_list = new ArrayList<String>();
                if (def != null && !def.isEmpty()) {
                    List<String>    s_a = MultiLine.split(def, WIDTH);
                    List<String>    cs_a = new ArrayList<String>();
                    for (String s: s_a)
                        cs_a.add(AnsiText.colorText(s, COLOR_ATTR_DEF, ansi));
                    str_list.add(Joint.join(cs_a, DEF_MULTILINE_GLUE));
                }
                List<String>    example_list = new ArrayList<String>();
                for (String e: example) {
                    List<String>    s_a = MultiLine.split(e, WIDTH - 6);
                    List<String>    cs_a = new ArrayList<String>();
                    for (String s: s_a)
                        cs_a.add(AnsiText.colorText(s, COLOR_ATTR_EXAMPLE, ansi));
                    example_list.add("    " + AnsiText.colorText("·", COLOR_ATTR_EXAMPLE_DOT, ansi) + " " + Joint.join(cs_a, EXAMP_MULTILINE_GLUE));
                }
                String  example_str = Joint.join(example_list, EXAMP_GLUE);
                if (!example_str.isEmpty())
                    str_list.add(example_str);
                //println(getJSONStr());
                return Joint.join(str_list, DEF_EXAMP_GLUE);
            }
            void    print() {print(true);}
            void    print(boolean ansi)
            {
                String  str = getBodyStr(ansi);
                if (!str.isEmpty())
                    println(str);
            }
        }

        static final String BODY_GLUE =
        /*
            "\n" +
            AnsiText.colorText("---- body glue ----", COLOR_ATTR_GLUE) +
         */
            "\n";
        static final String HEAD_BODY_GLUE =
        /*
            "\n" +
            AnsiText.colorText("---- head body glue ----", COLOR_ATTR_GLUE) +
         */
            "\n";
        public Head head = new Head();
        public List<Body>   bodyList = new ArrayList<Body>();
        public String       jsonStrWordSmart = null;

        public String  getJSONStr()
        {
            List<String>    s_a = new ArrayList<String>();
            for (Body body: bodyList)
                s_a.add(body.getJSONStr());
            //return String.format("{'head': %s, 'body': [%s]}", head.getJSONStr(), Joint.join(s_a, ","));
            return String.format("{'head': %s, 'body': [%s], 'Word Smart': '%s'}", head.getJSONStr(), Joint.join(s_a, ","), jsonStrWordSmart == null ? "null" : Json.escapeString(jsonStrWordSmart));
        }
        String  getBlockStr(boolean ansi)
        {
            List<String>    str_list = new ArrayList<String>();
            String  str = head.getHeadStr(ansi);
            if (!str.isEmpty())
                str_list.add(str);
            List<String>    body_str_list = new ArrayList<String>();
            for (Body body: bodyList) {
                str = body.getBodyStr(ansi);
                if (!str.isEmpty())
                    body_str_list.add(str);
            }
            if (!body_str_list.isEmpty())
                str_list.add(Joint.join(body_str_list, BODY_GLUE));
            //println(getJSONStr());
            return Joint.join(str_list, HEAD_BODY_GLUE);
        }
        void    print() {print(true);}
        void    print(boolean ansi)
        {
            String  str = getBlockStr(ansi);
            if (!str.isEmpty())
                println(str);
        }
    }
    public static class More {
        public static class Info {
            public String   href;   //
            int arl;                //
            String  arl0;           //
            public String   word;   //
            public String   pos;    //
            public String   gw;     //

            /**
             * JSON
             * {
             *  "word": "...",
             *  "pos":  "...",
             *  "gw":   "...",
             *  "arl":  #,
             *  "href": "..."
             * }
             */
            String getJSONStr()
            {
                List<String>    str_a = new ArrayList<String>();

                str_a.add(String.format("'word': '%s'", Json.escapeString(word)));
                str_a.add(String.format("'pos': '%s'", Json.escapeString(pos)));
                str_a.add(String.format("'gw': '%s'", Json.escapeString(gw)));
                str_a.add(String.format("'href': '%s'", Json.escapeString(href)));
                str_a.add(String.format("'arl': %d", arl));
                //return String.format("{\n    %s\n}", Joint.join(str_a, ",\n    "));
                return String.format("{%s}", Joint.join(str_a, ","));
            }
        }

        public List<Info>   infoList = new ArrayList<Info>();
        void setInfoList(Json.BaseType json) {
            assert json.type == Json.Type.LIST : "json: Not LIST";
            //println("# of more = " + ((Json.ListType) Tmore).list.size());
            /*
        public List<Info>   infoList = new ArrayList<Info>();
             */
            for (Json.BaseType TinfoList: ((Json.ListType) json).toList()) {
        /*
            public String   href;   //
            int arl;                //
            String  arl0;           //
            public String   word;   //
            public String   pos;    //
            public String   gw;     //
         */
                assert TinfoList.type == Json.Type.MAP : "TinfoList: Not MAP";
                Info   info = new Info();
                infoList.add(info);
                info.href = ((Json.StringType) ((Json.MapType) TinfoList).toMap().get("href")).toString();
                info.arl0 = ((Json.StringType) ((Json.MapType) TinfoList).toMap().get("arl")).toString();
                info.arl = new Integer(info.arl0).intValue();
                info.word = ((Json.StringType) ((Json.MapType) TinfoList).toMap().get("word")).toString();
                info.pos = ((Json.StringType) ((Json.MapType) TinfoList).toMap().get("pos")).toString();
                info.gw = ((Json.StringType) ((Json.MapType) TinfoList).toMap().get("gw")).toString();
            }
        }
        void setInfoList(String href, String href0) throws IOException {
            if (href == null)
                return;
            int i = href0.indexOf("?");
            href0 = i == -1 ? href0 : href0.substring(0, i);
            Document    doc = Jsoup.connect(href).get();
/*
    <ul class="result-list">
                                <li><a href="http://dictionary.cambridge.org/dictionary/american-english/word_1" title="Definition of word n LANGUAGE UNIT in American English"><span class='arl1'><span class="base"><span class="hw">word</span></span> <span class="pos">n</span> <span title="Guideword" class="gw">LANGUAGE UNIT</span></span></a></li>
 */
Pattern Pgw1 = Pattern.compile("<span[^<>]*class=\"gw\".*?>(.*?)</span>");
Pattern Pgw2 = Pattern.compile("<b[^<>]*class=\"gw\".*?>(.*?)</b>");
Pattern Ppos = Pattern.compile("<span[^<>]*class=\"pos\">(.*?)</span>");
String  gw;
String  pos;
            Element ul = doc.select("ul.result-list").first();
            for (Element li: ul.select("li")) {
gw = null;
pos = null;
                Element a = li.select("a").first();

                String  ref = a.attr("href");
                assert !ref.toString().isEmpty() : AnsiText.fatalStr("no \"href\" attribute");
                if (ref.equals(href0))
                    continue;

                Info    info = new Info();
                infoList.add(info);
                info.href = ref;
                Element arl = a.select("span").first();
                info.arl = new Integer(arl.attr("class").substring(3)).intValue();
                String  s = arl.toString();
                info.arl0 = s;
Matcher mgw1 = Pgw1.matcher(s);
if (mgw1.find()) {
//  System.out.println("\t|" + mgw1.group() + "|, |" + mgw1.group(1) + "|");
    gw = mgw1.group(1);
}
                //s = s.replaceFirst("<span[^<>]*class=\"gw\".*?>(.*?)</span>", AnsiText.colorText("[$1]", COLOR_ATTR_GW, ansi));
                s = s.replaceFirst("<span[^<>]*class=\"gw\".*?>(.*?)</span>", "");
                // http://dictionary.cambridge.org/dictionary/british/sake_1
Matcher mgw2 = Pgw2.matcher(s);
if (mgw2.find()) {
//  System.out.println("\t|" + mgw2.group() + "|, |" + mgw2.group(1) + "|");
    gw = mgw2.group(1);
}
                //s = s.replaceFirst("<b[^<>]*class=\"gw\".*?>(.*?)</b>", AnsiText.colorText("[$1]", COLOR_ATTR_GW, ansi));
                s = s.replaceFirst("<b[^<>]*class=\"gw\".*?>(.*?)</b>", "");
//              s = s.replaceFirst("<([a-zA-Z]+)[^<>]*class=\"gw\".*?>(.*?)</$1>", AnsiText.colorText("[$2]", COLOR_ATTR_GW, ansi));
Matcher mpos = Ppos.matcher(s);
if (mpos.find()) {
//  System.out.println("\t|" + mpos.group() + "|, |" + mpos.group(1) + "|");
    pos = mpos.group(1);
}
                //s = s.replaceFirst("<span[^<>]*class=\"pos\">(.*?)</span>", AnsiText.colorText("($1)", COLOR_ATTR_POS, ansi));
                s = s.replaceFirst("<span[^<>]*class=\"pos\">(.*?)</span>", "");
                s = s.replaceAll("<[^<>]*>", "").trim();
info.word = s;
info.pos = pos;
info.gw = gw;
                // nested ansi
                assert !s.isEmpty() : AnsiText.fatalStr("no \"info\"");
            }
        }

        static final String MORE_GLUE =
        /*
            "\n" +
            AnsiText.colorText("---- more glue ----", COLOR_ATTR_GLUE) +
         */
            "\n";
        static final String INFO_HREF_GLUE =
        /*
            "\n" +
            AnsiText.colorText("---- info-href glue ----", COLOR_ATTR_GLUE) +
         */
            "\n";
        public String  getJSONStr()
        {
            List<String>    str_a = new ArrayList<String>();
            for (Info info: infoList)
                str_a.add(info.getJSONStr());
            return String.format("[%s]", Joint.join(str_a, ","));
        }
        String  getMoreStr(boolean ansi)
        {
            List<String>    strList = new ArrayList<String>();
            for (Info info: infoList) {
                String  str = AnsiText.colorText(info.word, COLOR_ATTR_WORD, ansi);
                if (info.pos != null && !info.pos.isEmpty())
                    str += " (" + AnsiText.colorText(info.pos, COLOR_ATTR_POS, ansi) + ")";
                if (info.gw != null && !info.gw.isEmpty())
                    str += " [" + AnsiText.colorText(info.gw, COLOR_ATTR_GW, ansi) + "]";
                strList.add(
                    str + INFO_HREF_GLUE +
                    "    " + AnsiText.colorText(info.href, COLOR_ATTR_LINK, ansi)
                );
            }
            return Joint.join(strList, MORE_GLUE);
        }
        void    print() {print(true);}
        void    print(boolean ansi)
        {
            String  str = getMoreStr(ansi);
            if (!str.isEmpty())
                println(str);
        }
    }

    /**
     * Return a "real" URL for given wordStr.
     *
     * @wordStr "word" string or "http://dictionary.cambridge.org/..."
     * @flag    control/info flag
     */
    public static String getHref0(String wordStr, Flag flag)
    throws IOException
    {
        if (flag.verbose)
            printInfo("getHref0#wordStr\t|" + wordStr + "|");
        if (wordStr.startsWith("http://dictionary.cambridge.org/")) {
            return wordStr;
        }
        String  urlStr = null;
        urlStr = "http://dictionary.cambridge.org/search/";
        urlStr += flag.uk ? "british" : "american-english";
        urlStr += "/direct/?q=" + URLEncoder.encode(wordStr, "UTF-8");
        if (flag.verbose)
            printInfo("getHref0#wordStr:\t|" + urlStr + "|");

        HttpURLConnection   conn = null;
        conn = (HttpURLConnection) (new URL(urlStr).openConnection());
        //conn.setInstanceFollowRedirects(false);
        // not enough for "real" location due to multiple redirection
        conn.setRequestMethod("HEAD");
        // without this, got EOFException on HEAD call in android (emulator only)
        //  http://stackoverflow.com/questions/17638398/androids-httpurlconnection-throws-eofexception-on-head-requests
        //  https://code.google.com/p/android/issues/detail?id=24672
        conn.setRequestProperty("Accept-Encoding", "");
        //conn.setRequestProperty("Accept-Encoding", "gzip");
    ////    conn.setRequestMethod("GET");
    ////    conn.setRequestProperty("Accept-Encoding", "gzip");
        // just setting "Accept-Encoding" makes it work as "GET" specified
        int rc = -1;
        conn.connect(); // not enough for "real" fetch
        rc = conn.getResponseCode();
        // some action needed to get page
        if (flag.verbose)
            printInfo("response code = " + rc);
        // or...
        //println(conn.getContentType());
        //println(conn.getHeaderField("Content-Encoding"));
        //println(conn.getHeaderField("Content-Length"));
        if (flag.trace) {
            printTrace("|||header info|||");
            Map<String, List<String>>   map = conn.getHeaderFields();
            for (Map.Entry<String, List<String>> entry: map.entrySet()) {
                println("\t|" + entry.getKey() + "|: |" + entry.getValue() + "|");
            }
        }
        if (rc != 200)
            return null;
        String  s = conn.getURL().toString();
        if (flag.verbose) {
            printInfo("href0-:\t|" + urlStr + "|"); // old
            printInfo("href0:\t|" + s + "|");       // new
        }
        return s;
    }
    public static String
    get_href0(String url, Flag flag)
    throws IOException
    {
//  {
    //  boolean uk      = flag.uk;
    //  boolean showAll     = flag.showAll;
    //  boolean playPron    = flag.playPron;
    //  boolean showLink    = flag.showLink;
    //  boolean ansi        = flag.ansi;
        boolean verbose     = flag.verbose;
        boolean trace       = flag.trace;
    //  boolean showHref0   = flag.showHref0;
    //  boolean showHref0Only   = flag.showHref0Only;
    //  boolean resultOnly  = flag.resultOnly;
//  }
        if (verbose)
            printInfo("#href0\t|" + url + "|");
        HttpURLConnection   conn = (HttpURLConnection) (new URL(url).openConnection());
        //conn.setInstanceFollowRedirects(false);
        // not enough for "real" location due to multiple redirection

        conn.setRequestMethod("HEAD");
    ////    conn.setRequestMethod("GET");
    ////    conn.setRequestProperty("Accept-Encoding", "gzip");
        // just setting "Accept-Encoding" makes it work as "GET" specified
        conn.connect(); // not enough for "real" fetch
        // some action needed to get page
        int rc = conn.getResponseCode();
        if (verbose)
            printInfo("response code = " + rc);
        // or...
        //println(conn.getContentType());
        //println(conn.getHeaderField("Content-Encoding"));
        //println(conn.getHeaderField("Content-Length"));
        if (trace) {
            printTrace("|||header info|||");
            Map<String, List<String>>   map = conn.getHeaderFields();
            for (Map.Entry<String, List<String>> entry: map.entrySet()) {
                println("\t|" + entry.getKey() + "|: |" + entry.getValue() + "|");
            }
        }
        String  s = conn.getURL().toString();
        if (verbose) {
            printInfo("href0-:\t|" + url + "|");
            printInfo("href0:\t|" + s + "|");
        }
        return s;
    }

    public static String getAudioURL(Result result)
    {
        String  wordStr = result.wordStr;
        Result.Page page0 = result.doc.get(0);
        boolean isURL = wordStr.startsWith("http://");
        String  src;
        if (isURL)
            return page0.content.get(0).head.getAudioURL();
        for (Block r: page0.content) {
            if (wordStr.toUpperCase().equals(r.head.word.toUpperCase()))
                return r.head.getAudioURL();
        }
        // use first word
        return page0.content.get(0).head.getAudioURL();
    }
    public static String play(
        String  audioURL
    )
    throws
        IOException, InterruptedException
    {
        boolean downloadOnly = false;
        return play(audioURL, downloadOnly);
    }
    public static String play(
        String  audioURL,
        boolean downloadOnly
    )
    throws
        IOException, InterruptedException
    {
        if (audioURL == null)
            return null;
        Map<String, String> env = System.getenv();
        String  dir = env.get("HOME") + "/" + "vocabulary_mp3/";
        assert new File(dir).isDirectory() : AnsiText.fatalStr(dir + ": Not a directory");
        String  pathname = dir + audioURL.substring(audioURL.lastIndexOf('/'));
        //ProcessBuilder pb = new ProcessBuilder("play", audioURL);
        Process p;
        if (!downloadOnly && new File(pathname).isFile()) {
            ProcessBuilder pb = new ProcessBuilder("play", pathname);
            p = pb.start();
        } else {
            String  execStr;
            if (downloadOnly)
                execStr = String.format("wget -c -O \"%s\" \"%s\"", pathname, audioURL);
            else
                execStr = "wget -c -O- \"" + audioURL + "\" | tee " + pathname + " | play -";
            String[] cmdArray = {"bash", "-c", execStr};
            p = Runtime.getRuntime().exec(cmdArray);
        }
        p.waitFor();
        if (p.exitValue() != 0) {
            InputStream is = p.getErrorStream();
            int c;
            StringBuilder sb = new StringBuilder();
            while ((c = is.read()) != -1) {
                sb.append((char) c);
            }
            String s = sb.toString();
            if (!s.isEmpty())
                println(s);
        }
        return pathname;
    }

    static String   setAssertStr(
        String  str,
        String  href0,
        Element e
    )
    {
        return AnsiText.colorText(str, AnsiText.COLOR_ATTR_FATAL_HEAD) + ": " + AnsiText.colorText(href0, COLOR_ATTR_LINK) + "\n" + e.toString() + "\n";
    }
    // div.di-head, span.runon-head, span.phrase-head
    static void setHead(
        Element     headElement,
        String[]    tags,
        Flag        flag,
        Block.Head  head        /* OUT */
    )
    {
//  {
    //  boolean uk      = flag.uk;
    //  boolean showAll     = flag.showAll;
    //  boolean playPron    = flag.playPron;
    //  boolean showLink    = flag.showLink;
    //  boolean ansi        = flag.ansi;
        boolean verbose     = flag.verbose;
        boolean trace       = flag.trace;
    //  boolean showHref0   = flag.showHref0;
    //  boolean showHref0Only   = flag.showHref0Only;
    //  boolean resultOnly  = flag.resultOnly;
//  }
        if (verbose) {
            printInfo("#setHead");
            for (String tag: tags)
                printInfo("\t|" + tag + "|");
        }
                // div.di-head   span.runon-head   span.phrase-head
                // div.di-title  span.runon-title  span.phrase-title
                //   h1.hw         span.w            span.phrase
                // span.di-info  span.runon-info   span.phrase-info???
                //   a.sound*      ......            ??????
                //   span.pron
                //     span.ipa
                //   span.posgram
                //     span.pos
                //   "gw"
        Element di_title = headElement.select(tags[0]).first();
        if (trace) {
            printTrace("|||" + tags[0] + "|||");
            println(di_title);
        }
        Element hw = di_title.select(tags[1]).first();
        if (trace) {
            printTrace("|||" + tags[1] + "|||");
            println(hw);
        }
        String  s = hw.textNodes().get(0).text();
        if (verbose)
            printInfo("w-:\t|" + s + "|");
        int ii = s.indexOf("“");
        if (ii++ >= 0) {
            int fi = s.indexOf("”", ii);
            s = s.substring(ii, fi);
        } else
            s = s.trim();
        if (verbose)
            printInfo("w:\t|" + s + "|");
        head.word = s;
        Element info = headElement.select(tags[2]).first();
        if (trace) {
            printTrace("|||" + tags[2] + "|||");
            println(info);
        }
        if (info == null) { // could be null; "abide by"
            if (verbose)
                printInfo("info is null");
            return;
        }
        List<TextNode>  tna = info.textNodes();
        String  gw = "";
        for (TextNode tn: tna) {
            gw += tn;
            if (trace)
                printTrace("|||" + tn + "|||");
        }
        if (verbose)
            printInfo("gw:\t|" + gw.trim() + "|");
        gw = gw.replaceAll("&nbsp;", "");
        gw = gw.trim();
        head.gw = gw.isEmpty() ? null : gw;
        Elements    POSes = info.select("span.pos");
        if (!POSes.isEmpty()) { // could be none
            // http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word
            //assert POSes.size() == 1 : AnsiText.fatalStr("# of POS = " + POSes.size());
            //  "à la carte" (adj,adv)
            for (Element pos: POSes) {
                if (verbose)
                    printInfo("pos:\t|" + pos.text() + "|");
                head.pos.add(pos.text());
            }
        }
        Elements    aa = info.select("a");
        boolean audio = false;
        for (Element a: aa) {
            String  c = a.attr("class");
            if (c.equals("sound audio_play_button pron-uk")) {
                if (verbose) {
                    printInfo("uk:mp3:\t|" + a.attr("data-src-mp3") + "|");
                    printInfo("uk:ogg:\t|" + a.attr("data-src-ogg") + "|");
                }
                head.uk_mp3 = a.attr("data-src-mp3");
                head.uk_ogg = a.attr("data-src-ogg");
                audio = true;
            } else if (c.equals("sound audio_play_button pron-us")) {
                if (verbose) {
                    printInfo("us:mp3:\t|" + a.attr("data-src-mp3") + "|");
                    printInfo("us:ogg:\t|" + a.attr("data-src-ogg") + "|");
                }
                head.us_mp3 = a.attr("data-src-mp3");
                head.us_ogg = a.attr("data-src-ogg");
                audio = true;
            }
        }
        if (!audio && verbose) {
            // http://dictionary.cambridge.org/dictionary/american-english/code-word
            printInfo("no audio");
        }
        Elements    IPAs = info.select("span.ipa");
        //assert IPAs.size() == 1 : AnsiText.fatalStr("# of IPA = " + IPAs.size());
        // http://dictionary.cambridge.org/dictionary/american-english/word-processing
        for (Element ipa: IPAs) {
            if (verbose)
                printInfo("ipa:\t|" + ipa.text() + "|");
            head.ipa.add(ipa.text());
        }
    }

    // div.sense-body, div.idiom-body, span.runon-body, span.phrase-body
    static void setBody(
        Element         bodyElement,
        Flag            flag,
        List<Block.Body>    bodyList    /* OUT */
    ) {
//  {
    //  boolean uk      = flag.uk;
    //  boolean showAll     = flag.showAll;
    //  boolean playPron    = flag.playPron;
    //  boolean showLink    = flag.showLink;
    //  boolean ansi        = flag.ansi;
        boolean verbose     = flag.verbose;
        boolean trace       = flag.trace;
    //  boolean showHref0   = flag.showHref0;
    //  boolean showHref0Only   = flag.showHref0Only;
    //  boolean resultOnly  = flag.resultOnly;
//  }
        if (verbose)
            printInfo("#setBody");
        // consider child only; due to "span.def-block" in "runon" block
        for (Element e: bodyElement.children()) {
            if (!e.tagName().equals("span") || !e.attr("class").toString().equals("def-block"))
                continue;
            
            Block.Body  body = new Block.Body();
            bodyList.add(body);
            if (trace) {
                printTrace("|||def-block|||");
                println(e);
            }
            Element def_head = e.select("span.def-head").first();
            if (trace) {
                printTrace("|||def-head|||");
                println(def_head);
            }
            Elements defs = def_head.select("span.def");
            if (!defs.isEmpty()) {  // could be null; sycophant
                assert defs.size() == 1 : AnsiText.fatalStr("# of DEF = " + defs.size());
                Element def = defs.first();
                if (verbose)
                    printInfo("def:\t|" + def.text() + "|");
                body.def = def.text();
            }
            Element def_body = e.select("span.def-body").first();
            if (trace) {
                printTrace("|||def-body|||");
                println(def_body);
            }
            if (def_body != null) { // could be null; "duck"
/*
<span class="def-body"><span class="examp"><span class="eg">He was the charismatic leader his people had hoped for.</span></span></span>

<span class="def-body"> 
 <div class="trans" lang="en" dict="english-spanish">
  <span class="SEP"> </span>
  <span class="SEP nota">⇨ </span>charisma
 </div>
 <!-- End of DIV trans--></span>
 */
                Elements    examples = def_body.select("span.eg");
                for (Element examp: examples) {
                    if (verbose)
                        printInfo(".:\t|" + examp.text() + "|");
                    body.example.add(examp.text());
                }
                // carisma => chrisma
                Elements    trans = def_body.select("div.trans");
                for (Element t: trans) {
                    if (verbose)
                        printInfo(".:\t|" + t.text() + "|");
                    body.example.add(t.text());
                }
            }
        }
    }

/*
http://dictionary.cambridge.org/dictionary/american-english/describe?q=describe
http://dictionary.cambridge.org/dictionary/american-english/duck_1?q=duck
http://dictionary.cambridge.org/dictionary/british/sycophantic?q=sycophant
http://dictionary.cambridge.org/dictionary/british/sake_1?q=for+the+sake+of
http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone


http://dictionary.cambridge.org/dictionary/american-english/describe?q=describe
div.di
  div.di-head   // xxx-head block for "title", "sound for pronunciation", "ipa", "pos", "gw"
    div.di-title
      h1.hw
    span.di-info    //* could be null
      a.sound*
      span.pron
        span.ipa
      span.posgram
        span.pos    //* could be null
      "gw"      //* text; could be null
  div.di-body
    div.sense-block
      div.sense-body
        span.def-block  // def-block for "def" and "examp"
          span.def-head
            span.def
          span.def-body
            span.examp
              span.eg
http://dictionary.cambridge.org/dictionary/american-english/duck_1?q=duck
http://dictionary.cambridge.org/dictionary/british/sycophantic?q=sycophant
    // runon block
div.di
  div.di-head
  ......
  div.di-body
    div.sense-block
      div.sense-body
        span.def-block
        span.runon* // kind of "div.di" block with "di-head", "di-body"
          span.runon-head   // exactly same as "di-head";
                // 1. "runon-" instead of "di-"
        span.runon-title
                  span.w    // 2. instead of "h1.w"
                span.runon-info
          span.runon-body   // kind of "div.sense-body" with "def-block"
            span.def-block
            ......
http://dictionary.cambridge.org/dictionary/british/sake_1?q=for+the+sake+of
    // no def-block; but phrase-block
div.di
  div.di-head
  ......
  div.di-body
    div.sense-block
      div.sense-body
        span.phrase-block   // instead of "span.def-block"
          span.phrase-head  // exactly same as "di-head";
                // 1. "phrase-" instead of "di-"
            span.phrase-title
              span.phrase   // 2. instead of "h1.w"
          span.phrase-body  // instead of "div.sense-body"
            span.def-block  // exactly same
            ......
http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone
    // no sense-block; but idiom-block
div.di
  div.di-head
  ......
  div.di-body
    div.idiom-block // instead of "div.sense-block"
      div.idiom-body    // instead of "div.sense-body"
        span.def-block  // exactly same
        ......





div.di
  div.di-head
    div.di-title
      h1.hw
    span.di-info    //* could be null
      a.sound*
      span.pron
        span.ipa
      span.posgram
        span.pos    //* could be null
      "gw"      //* text; could be null
  div.di-body
    div.sense-block/div.idiom-block
  1.div.sense-block
      div.sense-body
        span.def-block/span.runon/span.phrase-block
      i.  span.def-head
             span.def
          span.def-body
            span.examp
              span.eg
     ii.span.runon* // kind of "div.di" block with "di-head", "di-body"
          span.runon-head   // exactly same as "di-head";
                // 1. "runon-" instead of "di-"
        span.runon-title
                  span.w    // 2. instead of "h1.w"
                span.runon-info
          span.runon-body   // kind of "div.sense-body" with "def-block"
            span.def-block
            ......
    iii.span.phrase-block   // instead of "span.def-block"
          span.phrase-head  // exactly same as "di-head";
                // 1. "phrase-" instead of "di-"
            span.phrase-title
              span.phrase   // 2. instead of "h1.w"
          span.phrase-body  // instead of "div.sense-body"
            span.def-block  // exactly same
            ......
  2.div.idiom-block
      div.idiom-body
        span.def-block
        ......


di = select("div.di")           // only 1
di_head = di.select("div.di-head")
    call setHead    // div.di-head   span.runon-head   span.phrase-head
                        // div.di-title  span.runon-title  span.phrase-title
                        //   h1.hw         span.w            span.phrase
                        // span.di-info  span.runon-info   span.phrase-info???
                        //   a.sound*      ......            ??????
                        //   span.pron
                        //     span.ipa
                        //   span.posgram
                        //     span.pos
                        //   "gw"
di_body = di.select("div.di-body")  // only 1
div_sense_block = di_body.select("div.sense-block")
if (div_sense_block is null)    // 2.
  div_idiom_block = di_bocy.select("div.idiom-block")       // only 1 if any
  if (div_idiom_block is not null)
    div_idiom_body = div_idiom_block.select("div.idiom_body")   // only 1
    span_def_block = div_idiom_body.select("span.def-block")    // only 1
      call setDefBlock
else                // 1.
  // i.
  div_sense_body = div_sense_block.select("div.sense-body") // only 1
  span_def_block = div_sense_body.select("span.def-block")  // only 1
    call setDefBlock
  // ii.
  runon = div_sense_body.select("runon")
  while (runon is not null)
    span_runon_head = runon.select("span.runon-head")
    call setHeadBlock   // "runon-head"
                        //   "runon-title"
                        //     "span.w"
                        //   "runon-info"
    span_runon_body = runon.select("span.runon-body")
    if (span_runon_body is not null)
      span_def_block = span_runon_body.select("span.def-block")
      call setDefBlock
  // iii.
  span_phrase_block = div_sense_body.select("span.phrase-block")
  while (span_phrase_block is not null)
    span_phrase_head = span_phrase_block.select("span.phrase-head")
    call setHeadBlock   // "phrase-head"
                        //   "phrase-title"
                        //     "span.phrase"
                        //   "phrase-info"
    span_phrase_body = span_phrase_block.select("span.phrase-body")
    if (span_phrase_body is not null)
      span_def_block = span_phrase_body.select("span.def-block")
      call setDefBlock
 */
    public static String
    lookup0(
        Flag        flag,
        Result.Page page    /* OUT */
    )
    throws IOException
    {
//  {
    //  boolean uk      = flag.uk;
    //  boolean showAll     = flag.showAll;
    //  boolean playPron    = flag.playPron;
        boolean showLink    = flag.showLink;
    //  boolean ansi        = flag.ansi;
        boolean verbose     = flag.verbose;
        boolean trace       = flag.trace;
    //  boolean showHref0   = flag.showHref0;
    //  boolean showHref0Only   = flag.showHref0Only;
    //  boolean resultOnly  = flag.resultOnly;
//  }
    // 'http://dictionary.cambridge.org/search/british/direct/?q=%s'
    // 'http://dictionary.cambridge.org/search/american-english/direct/?q=%s'
        String  href0 = page.href;

        Document    doc = Jsoup.connect(href0).get();
////        File        input = new File("x.html");
////        Document    doc = Jsoup.parse(input, "UTF-8");
    //  println(doc);

        Elements    di_a = doc.select("div.di");
        assert !di_a.isEmpty() : setAssertStr("No div.di", href0, doc);
        assert di_a.size() == 1 : setAssertStr("Two or more div.di", href0, doc);
        Element di = di_a.first();
        if (trace) {
            printTrace("|||div.di|||");
            println(di);
        }

        Block   block = new Block();
        page.content.add(block);
        // head
        Elements    di_head_a = di.select("div.di-head");
        assert !di_head_a.isEmpty() : setAssertStr("No div.di-head", href0, di);
        assert di_head_a.size() == 1 : setAssertStr("Two or more div.di-head", href0, di);
        Element di_head = di_head_a.first();
        if (trace) {
            printTrace("||||div.di-head|||");
            println(di_head);
        }
        String[]    tags = {"div.di-title", "h1.hw", "span.di-info"};
        setHead(di_head, tags, flag, block.head);

        // body
        Elements    di_body_a = di.select("div.di-body");
        assert !di_body_a.isEmpty() : setAssertStr("No div.di-body", href0, di);
        assert di_body_a.size() == 1 : setAssertStr("Two or more div.di-body", href0, di);
        Element di_body = di_body_a.first();
        if (trace) {
            printTrace("||||div.di-body|||");
            println(di_body);
        }
        Elements    sense_block_a = di_body.select("div.sense-block");
        if (sense_block_a.isEmpty()) {
            if (verbose) {
                printInfo("No sense-block");
                printInfo("Try idiom-block");
            }
            // http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone
            Elements    idiom_block_a = di_body.select("div.idiom-block");
            assert !idiom_block_a.isEmpty() : setAssertStr("No div.sense-block and div.idiom-block", href0, di_body);
            assert idiom_block_a.size() == 1 : setAssertStr("Two or more div.idiom-block", href0, di_body);
            Element     idiom_block = idiom_block_a.first();
            if (trace) {
                printInfo("|||div.idiom_block|||");
                println(idiom_block);
            }
            Elements    idiom_body_a = idiom_block.select("span.idiom-body");
            assert !idiom_body_a.isEmpty() : setAssertStr("No span.idiom-body", href0, idiom_block);
            assert idiom_body_a.size() == 1 : setAssertStr("Two or more span.idiom-body", href0, idiom_block);
            Element body = idiom_body_a.first();
            if (trace) {
                printInfo("|||span.idiom-body|||");
                println(body);
            }
            setBody(body, flag, block.bodyList);
        } else {
            //assert sense_block_a.size() == 1 : setAssertStr("Two or more div.sense-block", href0, di_body);
            // two div.sense-block: indemnify
            //Element sense_block = sense_block_a.first();
        for (Element sense_block: sense_block_a) {
            if (trace) {
                printTrace("|||div.sense-block|||");
                println(sense_block);
            }
            Elements    sense_body_a = sense_block.select("span.sense-body");
            assert !sense_body_a.isEmpty() : setAssertStr("No span.sense-body", href0, sense_block);
            assert sense_body_a.size() == 1 : setAssertStr("Two or more div.sense-body", href0, sense_block);
            Element body = sense_body_a.first();
            if (trace) {
                printInfo("|||span.sense-body|||");
                println(body);
            }
            setBody(body, flag, block.bodyList);
        }
        }

        // another block(head, body)
        //Elements    runon_a = body.select("span.runon");
        // find "span.runon" not in "span.sense-body" but in "div.di-body"
        //  indemnify
        Elements    runon_a = di_body.select("span.runon");
        for (Element runon: runon_a) {  // sycophantic
            Block   runonBlock = new Block();
            page.content.add(runonBlock);
            if (trace) {
                printInfo("|||span.runon|||");
                println(runon);
            }

            // head
            Elements    runon_head_a = runon.select("span.runon-head");
            assert !runon_head_a.isEmpty() : setAssertStr("No span.runon-head", href0, runon);
            assert runon_head_a.size() == 1 : setAssertStr("Two or more span.runon-head", href0, runon);
            Element runon_head = runon_head_a.first();
            if (trace) {
                printInfo("|||span.runon-head|||");
                println(runon_head);
            }
            String[] runon_tag3 = {"span.runon-title", "span.w", "span.runon-info"};
            setHead(runon_head, runon_tag3, flag, runonBlock.head);

            // body
            Elements    runon_body_a = runon.select("span.runon-body");
            if (runon_body_a.isEmpty()) // sycophancy
                continue;
            assert runon_body_a.size() == 1 : setAssertStr("Two or more span.runon-body",href0, runon);
            Element runon_body = runon_body_a.first();
            if (trace) {
                printInfo("|||span.runon-body|||");
                println(runon_body);
            }
            setBody(runon_body, flag, runonBlock.bodyList);
        }

        // another block(head, body)
        Elements    phrase_block_a = di_body.select("span.phrase-block");
        for (Element phrase_block: phrase_block_a) {
            // http://dictionary.cambridge.org/dictionary/british/sake_1?q=for+the+sake+of
            Block   phraseBlock = new Block();
            page.content.add(phraseBlock);
            if (trace) {
                printInfo("|||span.phrase-block|||");
                println(phrase_block);
            }

            // head
            Elements    phrase_head_a = phrase_block.select("span.phrase-head");
            assert !phrase_head_a.isEmpty() : setAssertStr("No span.phrase-head", href0, phrase_block);
            assert phrase_head_a.size() == 1 : setAssertStr("Two or more span.phrase-head", href0, phrase_block);
            Element phrase_head = phrase_head_a.first();
            if (trace) {
                printInfo("|||span.phrase-head|||");
                println(phrase_head);
            }
            String[] phrase_tag3 = {"span.phrase-title", "span.phrase", "span.phrase-info"};
            setHead(phrase_head, phrase_tag3, flag, phraseBlock.head);

            // body
            Elements    phrase_body_a = phrase_block.select("span.phrase-body");
            assert !phrase_body_a.isEmpty() : setAssertStr("No span.phrase-body", href0, phrase_block);
            assert phrase_body_a.size() == 1 : setAssertStr("Two or more span.phrase-body", href0, phrase_block);
            Element phrase_body = phrase_body_a.first();
            if (trace) {
                printInfo("|||span.phrase-body|||");
                println(phrase_body);
            }
            setBody(phrase_body, flag, phraseBlock.bodyList);
        }

        if (!showLink)
            return null;
        //<p class="moreResultLink">
        //  <a class="cdo-link"
        Elements    more_aa = doc.select("p.moreResultLink");
    //  println("more...");
        String  all_href = null;
        for (Element more_a: more_aa) {
    //      println("more");
            Elements    aa = more_a.select("a.cdo-link");
            for (Element ea: aa) {
                String  s = ea.attr("href");
    //          println(ea.attr("href"));
    //          println(ea.attr("title"));
                if (ea.attr("title").startsWith("All results")) {
                    if (all_href == null)
                        all_href = s;
                    else {
                        assert all_href.equals(s) : AnsiText.fatalStr(all_href + " != " + s);
                    }
                }
            }
        }
        if (verbose)
            printInfo("URL for all: |" + all_href + "|");
        if (all_href != null)
            page.more.setInfoList(all_href, href0);
        return all_href;
    }

    public static List<String> spellCheck(
        String  spellcheckURL
    )
    throws
        IOException
    {
        int statusCode = 200;
        String  result = "";
        {
            BufferedReader rd;
            HttpURLConnection   conn = (HttpURLConnection) (new URL(spellcheckURL).openConnection());
            statusCode = conn.getResponseCode();
            InputStream is = statusCode == 200 ? conn.getInputStream() : conn.getErrorStream();
            rd = new BufferedReader(new InputStreamReader(is));
            String  line;
            while ((line = rd.readLine()) != null) {
                result += line;
            }
            rd.close();
        }
        //println(spellcheckURL);
        //Document    doc = Jsoup.connect(spellcheckURL).get();
        List<String>    sws = new ArrayList<String>();
        Document    doc = Jsoup.parse(result);
        Element scc = doc.select("div#cdo-spellcheck-container").first();
        if (statusCode != 200) {
/*
            Element ul = scc.select("ul").first();
            for (Element li: ul.select("li")) {
                Element a = li.select("a").first();
                println("    " + a.attr("title"));
            }
 */
        } else {
            Element sc = scc.select("ul#cdo-spellcheck").first();
            for (Element e: sc.select("a.cdo-link")) {
                sws.add(e.text().trim());
            }
        }
        return sws;
    }

    static abstract class ResultBase {
        public String       wordStr = null;
        public List<String> ErrorMessageList = null;
        public String       WarnMessage = null;
        public List<String> tryWordList = new ArrayList<String>();
    }
    public static class Result extends ResultBase {
        public static class Page {
            public String       href;
            public List<Block>  content = new ArrayList<Block>();
            public More         more = new More();

            public String getJSONStr() {
                return String.format("{'href': '%s', 'content': %s, 'more': %s}", Json.escapeString(href), contentJSONStr(), more.getJSONStr());
            }
            public String contentJSONStr() {
                List<String>    a = new ArrayList<String>();
                for (Block block: content)
                    a.add(block.getJSONStr());
                return String.format("[%s]", Joint.join(a, ","));
            }
        }
        public List<Page>   doc = new ArrayList<Page>();

        public String getStr(boolean all, boolean ansi)
        {
            List<String>    allList = new ArrayList<String>();
            for (Page page: doc) {
                List<String>    a = new ArrayList<String>();
                for (Block block: page.content) {
                    String  str = block.getBlockStr(ansi);
                    if (!str.isEmpty())
                        // if showAll not set, last is for more if any
                        a.add(str);
                }
                if (!a.isEmpty())
                    allList.add(Joint.join(a, BLOCK_GLUE));
                if (!all)
                    break;
            }
            Page    page0 = doc.get(0);
            if (!all && page0.more.infoList.size() > 0) {
                allList.add(
                    AnsiText.colorText("more (to get all, use \"-a\" option)", COLOR_ATTR_MORE, ansi) +
                    MORE_STR_MORE_GLUE +
                    page0.more.getMoreStr(ansi)
                );
            }
            return Joint.join(allList, all ? PAGE_GLUE : PAGE_MORE_BLOCK_GLUE);
        }
        public String getJSONStr()
        {
            List<String>    page_a = new ArrayList<String>();
            for (Page page: doc) {
                page_a.add(page.getJSONStr());
            }
            return String.format("[%s]", Joint.join(page_a, ","));
        }
/*
        public void setDoc2(Json.BaseType json)
        {
            setDoc2(json, false);
        }
        public void setDoc2(Json.BaseType json, boolean verbose)
        {
            Object oDoc = Json.BaseType.setObject(json);
////    public static class Page {
////        String              href;
////        public List<Block>  content = new ArrayList<Block>();
////    }
////    public List<Page>   doc = new ArrayList<Page>();
////    public More         more = new More();
            // T0: Result.doc
            assert oDoc instanceof List : "oDoc: Not List";
            List<Object> oPageList = (List<Object>) oDoc;
            if (verbose)
                println("doc: # of pages = " + oPageList.size());
            for (Object oPage: oPageList) {
                Result.Page p = new Result.Page();
                doc.add(p);
                assert oPage instanceof Map : "oPage: Not Map";
                Map<String, Object> mPage = (Map<String, Object>) oPage;
                Object oHref = mPage.get("href");
                assert oHref instanceof String : "oHref: Not String";
                p.href = (String) oHref;
                Object oMore = mPage.get("more");
                Object oContent = mPage.get("content");
                if (oMore instanceof String) {
                    if (verbose)
                        println("more: |" + (String) oMore + "|");
                    assert ((String) oMore).equals("null") : "oMore: String: Not a \"null\": |" + ((String) oMore) + "|";
                } else {
                    assert oMore instanceof List : "oMore: Not List";
                    List<Object> oInfoList = (List<Object>) oMore;
                    if (verbose)
                        println("more: # of info = " + oInfoList.size());
                    for (Object oInfo: oInfoList) {
////        public String   href;   //
////        int arl;                //
////        String  arl0;           //
////        public String   word;   //
////        public String   pos;    //
////        public String   gw;     //
                        assert oInfo instanceof Map : "oInfo: Not Map";
                        More.Info   info = new More.Info();
                        p.more.infoList.add(info);
                        Map<String, Object> mInfo = (Map<String, Object>) oInfo;
                        Object oStr;
                        oStr = mInfo.get("href");
                        assert oStr instanceof String : "oMore.href: Not String";
                        if (verbose)
                            println("href: |" + (String) oStr + "|");
                        info.href = (String) oStr;
                        oStr = mInfo.get("arl");
                        assert oStr instanceof String : "oMore.arl: Not String";
                        if (verbose)
                            println("arl: |" + (String) oStr + "|");
                        info.arl0 = (String) oStr;
                        info.arl = new Integer(info.arl0).intValue();
                        oStr = mInfo.get("word");
                        assert oStr instanceof String : "oMore.word: Not String";
                        if (verbose)
                            println("word: |" + (String) oStr + "|");
                        info.word = (String) oStr;
                        oStr = mInfo.get("pos");
                        assert oStr instanceof String : "oMore.pos: Not String";
                        if (verbose)
                            println("pos: |" + (String) oStr + "|");
                        info.pos = (String) oStr;
                        oStr = mInfo.get("gw");
                        assert oStr instanceof String : "oMore.gw: Not String";
                        if (verbose)
                            println("gw: |" + (String) oStr + "|");
                        info.gw = (String) oStr;
                    }
                }
                if (oContent instanceof String) {
                    if (verbose)
                        println("content: |" + (String) oContent + "|");
                    assert ((String) oContent).equals("null") : "oContent: String: Not a \"null\": |" + (String) oContent + "|";
                } else {
                    assert oContent instanceof List : "oContent: Not List";
                    List<Object> oBlockList = (List<Object>) oContent;
                    if (verbose)
                        println("content: # of blocks = " + oBlockList.size());
                    for (Object oBlock: oBlockList) {
                        assert oBlock instanceof Map : "oBlock: Not Map";
                        Block   block = new Block();
                        p.content.add(block);
                        Map<String, Object> mBlock = (Map<String, Object>) oBlock;
////    public Head head = new Head();
////    public List<Body>   bodyList = new ArrayList<Body>();
                        Object oHead = mBlock.get("head");
                        assert oHead instanceof Map : "oHead: Not Map";
                        Map<String, Object> mHead = (Map<String, Object>) oHead;
////        public String   word;
////        public String   gw;
////        public List<String>   pos = new ArrayList<String>();
////        public String   uk_mp3, uk_ogg, us_mp3, us_ogg;
////        public List<String> ipa = new ArrayList<String>();
                        Object oWord = mHead.get("word");
                        assert oWord instanceof String : "oWord: Not String";
                        if (verbose)
                            println("word: |" + (String) oWord + "|");
                        block.head.word = (String) oWord;
                        Object oGw = mHead.get("gw");
                        assert oGw instanceof String : "oGw: Not String";
                        if (verbose)
                            println("gw: |" + (String) oGw + "|");
                        block.head.gw = (String) oGw;
                        Object oPos = mHead.get("pos");
                        assert oPos instanceof List: "oPos: Not List";
                        for (Object o: (List<Object>) oPos) {
                            assert o instanceof String : "oPos.o: Not String";
                            if (verbose)
                                println("pos: |" + (String) o + "|");
                            block.head.pos.add((String) o);
                        }
                        Object oPron = mHead.get("pron");
                        assert oPron instanceof List: "oPron: Not List";
                        List<Object> oPronList = (List<Object>) oPron;
                        assert oPronList.size() == 4 : "oPron: # of elements <> 4 (" + oPronList.size() + ")";
                        Object oUKmp3 = oPronList.get(0);
                        assert oUKmp3 instanceof String : "oPron[0]: Not String";
                        if (verbose)
                            println("uk_mp3: |" + (String) oUKmp3 + "|");
                        block.head.uk_mp3 = (String) oUKmp3;
                        Object oUKogg = oPronList.get(1);
                        assert oUKogg instanceof String : "oPron[1]: Not String";
                        if (verbose)
                            println("uk_ogg: |" + (String) oUKogg + "|");
                        block.head.uk_ogg = (String) oUKogg;
                        Object oUSmp3 = oPronList.get(2);
                        assert oUSmp3 instanceof String : "oPron[2]: Not String";
                        if (verbose)
                            println("us_mp3: |" + (String) oUSmp3 + "|");
                        block.head.us_mp3 = (String) oUSmp3;
                        Object oUSogg = oPronList.get(3);
                        assert oUSogg instanceof String : "oPron[3]: Not String";
                        if (verbose)
                            println("us_ogg: |" + (String) oUSogg + "|");
                        block.head.us_ogg = (String) oUSogg;
                        Object oIpa = mHead.get("ipa");
                        assert oIpa instanceof List: "oIpa: Not List";
                        for (Object o: (List<Object>) oIpa) {
                            assert o instanceof String : "oIpa.o: Not String";
                            if (verbose)
                                println("ipa: |" + (String) o + "|");
                            block.head.ipa.add((String) o);
                        }

                        Object oBodies = mBlock.get("body");
                        assert oBodies instanceof List : "oBodies: Not List";
                        List<Object> oBodyList = (List<Object>) oBodies;
                        if (verbose)
                            println("block: # of bodies = " + oBodyList.size());
                        for (Object oBody: oBodyList) {
                            assert oBody instanceof Map : "oBody: Not Map";
                            Block.Body  body = new Block.Body();
                            block.bodyList.add(body);
                            Map<String, Object> mBody = (Map<String, Object>) oBody;
////        public String   def;
////        public List<String> example = new ArrayList<String>();
                            Object oDef = mBody.get("def");
                            assert oDef instanceof String : "oDef: Not String";
                            if (verbose)
                                println("def: |" + (String) oDef + "|");
                            body.def = (String) oDef;
                            Object oExampleList = mBody.get("example");
                            assert oExampleList instanceof List : "oExampleList: Not List";
                            for (Object oExample: (List<Object>) oExampleList) {
                                assert oExample instanceof String : "oExample: Not String";
                                if (verbose)
                                    println("example: |" + (String) oExample + "|");
                                body.example.add((String) oExample);
                            }
                        }
                    }
                }
            }
        }
 */
        public void setDoc(Json.BaseType json, boolean showAll)
        {
            /*
        public static class Page {
            String              href;
            public List<Block>  content = new ArrayList<Block>();
        }
        public List<Page>   doc = new ArrayList<Page>();
        public More         more = new More();
             */
            // T0: Result.doc
            assert json.type == Json.Type.LIST : "json: Not LIST";
            //println("# of doc = " + ((Json.ListType) Tdoc).list.size());
            for (Json.BaseType TPage: ((Json.ListType) json).toList()) {
                Result.Page p = new Result.Page();
                doc.add(p);
                //TPage.print(0, ">");
                // TPage: Page
                assert TPage.type == Json.Type.MAP : "TPage: Not MAP";
                p.href = ((Json.StringType) ((Json.MapType) TPage).toMap().get("href")).toString();
                Json.BaseType  y = ((Json.MapType) TPage).toMap().get("more");
                if (y.type == Json.Type.STRING) {
                    String s = ((Json.StringType) y).toString();
                    assert s.equals("null") : AnsiText.fatalStr("Not a \"null\": " + AnsiText.colorText(s, COLOR_ATTR_POS));
                } else {
                    p.more.setInfoList(y);
                }
                Json.BaseType  x = ((Json.MapType) TPage).toMap().get("content");
                if (x.type == Json.Type.STRING) {
                    String s = ((Json.StringType) x).toString();
                    assert s.equals("null") : AnsiText.fatalStr("Not a \"null\": " + AnsiText.colorText(s, COLOR_ATTR_POS));
                    continue;
                }
                for (Json.BaseType TBlock: ((Json.ListType) x).toList()) {
                    // TBlock: Block
                    assert TBlock.type == Json.Type.MAP : "TBlock: Not MAP";
                    Block   block = new Block();
                    /*
        public Head head = new Head();
        public List<Body>   bodyList = new ArrayList<Body>();
                     */
                    p.content.add(block);
                    Json.BaseType  THead = ((Json.MapType) TBlock).toMap().get("head");
                    /*
            public String   word;
            public String   gw;
            public List<String>   pos = new ArrayList<String>();
            public String   uk_mp3, uk_ogg, us_mp3, us_ogg;
            public List<String> ipa = new ArrayList<String>();
                     */
                    //THead.print();
                    assert THead.type == Json.Type.MAP : "THead: Not MAP";
                    block.head.word = ((Json.StringType) ((Json.MapType) THead).toMap().get("word")).toString();
                    block.head.gw = ((Json.StringType) ((Json.MapType) THead).toMap().get("gw")).toString();
                    for (Json.BaseType Tpos: ((Json.ListType) ((Json.MapType) THead).toMap().get("pos")).toList()) {
                        block.head.pos.add(((Json.StringType) Tpos).toString());
                    }
                    List<Json.BaseType> a = ((Json.ListType) ((Json.MapType) THead).toMap().get("pron")).toList();
                    block.head.uk_mp3 = ((Json.StringType) a.get(0)).toString();
                    block.head.uk_ogg = ((Json.StringType) a.get(1)).toString();
                    block.head.us_mp3 = ((Json.StringType) a.get(2)).toString();
                    block.head.us_ogg = ((Json.StringType) a.get(3)).toString();
                    for (Json.BaseType Tipa: ((Json.ListType) ((Json.MapType) THead).toMap().get("ipa")).toList()) {
                        block.head.ipa.add(((Json.StringType) Tipa).toString());
                    }
                    Json.BaseType  TbodyList = ((Json.MapType) TBlock).toMap().get("body");
                    assert TbodyList.type == Json.Type.LIST : "TbodyList: Not LIST";
                    //println("# of body = " + ((Json.ListType) TbodyList).list.size());
                    for (Json.BaseType Tbody: ((Json.ListType) TbodyList).toList()) {
                        assert Tbody.type == Json.Type.MAP : "Tbody: Not MAP";
                        //Tbody.print();
                        Block.Body  body = new Block.Body();
                        block.bodyList.add(body);
                        /*
            public String   def;
            public List<String> example = new ArrayList<String>();
                         */
                        body.def = ((Json.StringType) ((Json.MapType) Tbody).toMap().get("def")).toString();
                        for (Json.BaseType Texample: ((Json.ListType) ((Json.MapType) Tbody).toMap().get("example")).toList()) {
                            body.example.add(((Json.StringType) Texample).toString());
                        }
                    }
                    //println(block.getBlockStr(ansi));
                    Json.BaseType  jsWordSmart = ((Json.MapType) TBlock).toMap().get("Word Smart");
                    assert jsWordSmart.type == Json.Type.STRING : "jsWordSmart: Not STRING";
                    block.jsonStrWordSmart = ((Json.StringType) jsWordSmart).toString();
                    if (block.jsonStrWordSmart.equals("null"))
                        block.jsonStrWordSmart = null;
                }
                if (!showAll)
                    break;
            }
        }
    }
    /**
     * Return Result class (flag.resultOnly is true) or print the string for Result (flag.resultOnly is false).
     *
     * @wordStr tells if "word" is given or not; <code>null</code> means url is given instead of word
     * @href0   "real" url to be accessed and for information to be gathered from
     * @flag    control/info flag
     */
    public static Result
    getResultFromWeb(
        String  href0,
        Flag    flag
    )
    throws
        IOException
    {
//  {
//      boolean uk              = flag.uk;
        boolean showAll         = flag.showAll;
//      boolean force           = flag.force;
//      boolean playPron        = flag.playPron;
//      boolean showLink        = flag.showLink;
        boolean ansi            = flag.ansi;
        boolean verbose         = flag.verbose;
        boolean trace           = flag.trace;
//      boolean showHref0       = flag.showHref0;
//      boolean showHref0Only   = flag.showHref0Only;
        boolean resultOnly      = flag.resultOnly;
//  }
        Result  result = new Result();
        int nPage = 0;   // # of "new" page via web access
        // 1. set page0
        // 2. based on all_href, set more
        // 3. if showAll is set, set all pages
        Result.Page page0 = new Result.Page();
        page0.href = href0;
        result.doc.add(page0); ++nPage;
        System.err.printf(".");
        String all_href = lookup0(flag, page0);
        if (showAll) {
            for (More.Info info: page0.more.infoList) {
                Result.Page page = new Result.Page();
                page.href = info.href;
                result.doc.add(page); ++nPage;
                System.err.printf(".");
                lookup0(flag, page);
            }
        }
        // clear dots
        if (nPage >= 0) {
            System.err.printf("\r");
            for (int i = 0; i < nPage; ++i)
                System.err.printf(" ");
            System.err.printf("\r");
        }
        return result;
    }
    public static List<String> getStackTrace(Throwable e)
    {
        List<String>    a = new ArrayList<String>();
        a.add(e.toString());
        for (StackTraceElement ee: e.getStackTrace())
            a.add("    " + ee.toString());
        return a;
//      return e.toString() + "\n\t" + Joint.join(a, "\n\t");
    }
}
