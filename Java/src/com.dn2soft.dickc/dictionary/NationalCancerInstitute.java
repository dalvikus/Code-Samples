package com.dn2soft.dickc.dictionary;

import com.dn2soft.util.Json;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.nodes.TextNode;
import org.jsoup.select.Elements;

import java.io.File;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;

import java.net.HttpURLConnection;
import java.net.URL;
import java.io.InputStreamReader;
import java.io.BufferedReader;

public class NationalCancerInstitute {
    final static String BASE_URL_MP3 = "http://www.cancer.gov/PublishedContent/Media/CDR/media/";

    private static void genetics(String urlStr, boolean verbose)
    throws java.io.IOException
    {
        Document    doc = Jsoup.connect(urlStr).get();
        List<String[]> ra = NationalCancerInstitute.genetics0(doc);//, true);
        if (verbose)
            NationalCancerInstitute.print(ra);
    }
    private static void genetics(File file, boolean verbose)
    throws java.io.IOException
    {
        Document doc = Jsoup.parse(file, "UTF-8");
        List<String[]> ra = NationalCancerInstitute.genetics0(doc);//, true);
        if (verbose)
            NationalCancerInstitute.print(ra);
    }

    private static void cancer(String urlStr, boolean verbose)
    throws java.io.IOException
    {
        //Document    doc = Jsoup.connect(href).get();
        // for big data(||), jsoup does not return a complete document
        String all = NationalCancerInstitute.getHtml(urlStr);//, true);
        Document doc = Jsoup.parse(all);
        List<String[]> ra = NationalCancerInstitute.cancer0(doc);//, true);
        if (verbose)
            NationalCancerInstitute.print(ra);
    }
    private static void cancer(File file, boolean verbose)
    throws java.io.IOException
    {
        Document doc = Jsoup.parse(file, "UTF-8");
        List<String[]> ra = NationalCancerInstitute.cancer0(doc);//, true);
        if (verbose)
            NationalCancerInstitute.print(ra);
    }

    /**
     * for big data
     *
     * jsoup returns partial document
     */
    private static String getHtml(String urlStr)
    throws java.io.IOException
    {
        return getHtml(urlStr, false);
    }
    private static String getHtml(String urlStr, boolean trace)
    throws java.io.IOException
    {
        if (trace) {
            HttpURLConnection   conn = null;
            conn = (HttpURLConnection) (new URL(urlStr).openConnection());
            conn.setRequestMethod("HEAD");
            conn.setRequestProperty("Accept-Encoding", "");
            int rc = -1;
            conn.connect(); // not enough for "real" fetch
            rc = conn.getResponseCode();
            System.out.println(rc);
            Map<String, List<String>>   map = conn.getHeaderFields();
            for (Map.Entry<String, List<String>> entry: map.entrySet()) {
                System.out.println("\t|" + entry.getKey() + "|: |" + entry.getValue() + "|");
            }
        }

        URL myUrl = new URL(urlStr);
        BufferedReader in = new BufferedReader(new InputStreamReader(myUrl.openStream()));
        StringBuilder sb = new StringBuilder();
        int c;
        while ((c = in.read()) != -1)
            sb.append((char) c);
        in.close();
        return sb.toString();
    }

    private static void print(List<String[]> a)
    {
        for (String[] sa: a) {
            String word = sa[0];
            String mp3 = sa[1];
            String mtd = sa[2];
            String def = sa[3];
            System.out.println("|" + word + "|");
            System.out.println("\t|" + mp3 + "|");
            System.out.println("\t|" + mtd + "|");
            System.out.println("\t|" + def + "|");
            System.out.printf("JSON|'%s': ['%s', '%s', '%s']\n", Json.escapeString(word), Json.escapeString(mp3), Json.escapeString(mtd), Json.escapeString(def));
        }
    }


    private static List<String[]> genetics0(Document doc)
    {
        return genetics0(doc, false);
    }
    private static List<String[]> genetics0(Document doc, boolean trace)
    {
        List<String[]>  ra = new ArrayList<String[]>(); // to be returned

/*
<div class="searchResults">
    <span class="page-title">
        <br>
        <span class="page-title" id="lblNumResults">172</span> 
        <span class="page-title" id="lblResultsFor">results found for:</span> 
        <span class="page-title" id="Span1">%</span>
...
 */
        int numResult = new Integer(doc.select("span.page-title#lblNumResults").first().text());
        Elements ul_a = doc.select("ul.no-bullets");
        int n = 0;
        final String NBSP = new String(new byte[] {(byte) 0xc2, (byte) 0xa0});
        if (ul_a.size() > 0) {
            Elements li_a = ul_a.first().select("li");
            for (Element li: li_a) {
/*
                <li style="list-style-type: none;">
                <a href="http://www.cancer.gov/geneticsdictionary?cdrid=751596" onclick="NCIAnalytics.GeneticsDictionaryResults(this,&#39;1&#39;);">
                admixture mapping</a>&nbsp;
                <span class="CDR_audiofile"><a href="http://www.cancer.gov/PublishedContent/Media/CDR/media/752683.mp3" id="audioLink752683" class="CDR_audiofile" shape="rect"><img src="./NCI Dictionary of Genetics Terms - National Cancer Institute_files/audio-icon.gif" alt="listen" border="0" height="16" width="16"></a></span>&nbsp;&nbsp;<span class="mtd_pronounce">(ad-MIX-cher MA-ping)</span>
                <br>
                A method used to identify genetic variants associated with traits and/or diseases in ethnic groups whose genomes resulted from a recent mixture of two or more geographically distinct ancestral populations.
                
                </li>
 */
/*
                <li style="list-style-type: none;">
                <a href="http://www.cancer.gov/geneticsdictionary?cdrid=664428" onclick="NCIAnalytics.GeneticsDictionaryResults(this,&#39;6&#39;);">
                ART</a>&nbsp;
                
                <br>
                A term used to describe collectively a number of noncoital methods of conception that are used to treat infertility with donor or nondonor eggs and sperm including in vitro fertilization (IVF), gamete intrafallopian transfer (GIFT), and zygote intrafallopian transfer (ZIFT). Also called assisted reproductive technology.
                
                </li>
 */
                String word = li.child(0).text();
                String mp3 = "";
                Elements mp3_a = li.select("span.CDR_audiofile");
                if (mp3_a.size() > 0) {
                    mp3 = mp3_a.first().child(0).attr("href");
                    int idx = mp3.lastIndexOf("/");
                    assert idx != -1 : "FATAL: \".../...mp3\" <> |" + mp3 + "|";
                    mp3 = mp3.substring(idx + 1);
                }
                String mtd = "";
                Elements mtd_a = li.select("span.mtd_pronounce");
                if (mtd_a.size() > 0) {
                    mtd = mtd_a.first().text();
                    assert mtd.startsWith("(") && mtd.endsWith(")") : "FATAL: \"(...)\" <> |" + mtd + "|";
                    mtd = mtd.substring(1, mtd.length() - 1);
                }
                String def = li.ownText().replaceAll("\u00a0", "").trim();  // contains unicode nob-break space (\u00a0)(\xc2\xa0)

                if (trace) {
                    System.out.println("|" + word + "|");
                    System.out.println("\t|" + mp3 + "|");
                    System.out.println("\t|" + mtd + "|");
                    System.out.println("\t|" + def + "|");
                    System.out.printf("JSON|'%s': ['%s', '%s', '%s']\n", Json.escapeString(word), Json.escapeString(mp3), Json.escapeString(mtd), Json.escapeString(def));
                }
                String[] sa = new String[] {word, mp3, mtd, def};
                ra.add(sa);

                ++n;
            }
        }
        if (trace)
            System.out.println("# of result = " + numResult + " (" + n + ")");
        assert numResult == ra.size() : numResult + " <> " + ra.size();

        return ra;
    }

    private static List<String[]> cancer0(Document doc)
    {
        return cancer0(doc, false);
    }
    private static List<String[]> cancer0(Document doc, boolean trace)
    {
        List<String[]> ra = new ArrayList<String[]>();  // to be returned

/*
        <div id="ctl21_numResDiv" class="dictionary-search-results-header">
			
            <img src="./NCI Dictionary of Cancer Terms - National Cancer Institute_files/spacer.gif" width="10" height="19" alt="" border="0"><br>
            <span class="page-title">
                <span id="ctl21_lblNumResults" class="page-title">7637</span>
                <span id="ctl21_lblResultsFor" class="page-title">results found for:</span>
            </span>&nbsp;&nbsp; &nbsp;&nbsp;
...
 */
        int numResult = new Integer(doc.select("span.page-title#ctl21_lblNumResults").first().text());
/*
      <div id="cgvBody" class="cgvBody-defaultTemplateContentContainer">
	<div class="contentid-16385 slot-item first-SI">
		
	</div><div class="contentid-22950 slot-item last-SI">
	</div><div class="contentid-22950 slot-item last-SI">
 */
        Element div = doc.select("div.cgvBody-defaultTemplateContentContainer#cgvBody").first().child(1);
        assert div.nodeName().equals("div") && div.attr("class").equals("contentid-22950 slot-item last-SI") : "Not a |<div class=\"contentid-22950 slot-item last-SI\">|: |" + div + "|";
/*
        <div id="ctl21_numResDiv" class="dictionary-search-results-header">
 */
        int numChildren = div.children().size();
        List<Node> nodeChildren = div.childNodes();
        int numNodes = div.children().size();
        if (trace) {
            System.out.println("INFO: # of result: " + numResult);
            System.out.println("INFO: # of nodes: " + nodeChildren.size());
            System.out.println("INFO: # of elements: " + numChildren);
        }
/*
        for (Node node: nodeChildren) {
            System.out.println(node.nodeName());
        }
 */
/*
#text
script
#text
script
#text
#comment
#text
div
#text
div
#text
#comment
#text
div         <-      <div id="ctl21_numResDiv" class="dictionary-search-results-header">
#text       <-o     with pronunciation
a
a
#text
a
#text
table
#text
br
#text       <-o
a
a
#text
a
#text
table
#text
br
#text       <-o
a
a
#text
a
#text
table
#text
br
#text       <-o
a
a
#text
a
#text
table
#text
br
#text       <-o
a
a
#text
a
#text
table
#text
br
#text       <-x     without pronunciation
a
a
#text
table
#text
br
#text       <-x
a
a
#text
table
#text
br
#text
a
a
#text
a
#text
table
#text
br
#text
a
a
#text
table
#text
br
......
 */
        int iChild = 0;
        int iNode = 0;
        boolean found = false;
        for (; iChild < numChildren; ++iChild, ++iNode) {
            while (nodeChildren.get(iNode++).nodeName().startsWith("#")) {}     // #text, #comment
            Element node = div.child(iChild);
            if (trace)
                System.out.println("INFO: " + node.nodeName());
            if (node.nodeName().equals("div") && node.attr("id").equals("ctl21_numResDiv") && node.attr("class").equals("dictionary-search-results-header")) {
                ++iChild;   // proceed to next "real" child
                found = true;
                break;
            }
        }
        assert found : "Not found: |<div id=\"ctl21_numResDiv\" class=\"dictionary-search-results-header\">|";
        if (trace)
            System.out.println("INFO: iNode = " + iNode + ", " + nodeChildren.get(iNode).nodeName()); //iNode = 14;
/*
 * with pronunciation
 *
                <a name="1-(2’-deoxy-2’-fluoro-ß-D-arabinofuranosyl) uracil"></a><a href="http://www.cancer.gov/dictionary?CdrID=642135" onclick="NCIAnalytics.TermsDictionaryResults(this,&#39;1&#39;);">
                    1-(2’-deoxy-2’-fluoro-ß-D-arabinofuranosyl) uracil</a> &nbsp;&nbsp;
                <a href="http://www.cancer.gov/PublishedContent/Media/CDR/media/723011.mp3" id="audioLink723011" class="CDR_audiofile" shape="rect"><img src="./NCI Dictionary of Cancer Terms - National Cancer Institute_files/audio-icon.gif" alt="listen" border="0" height="16" width="16"></a>&nbsp;&nbsp;
                (1-(2'-dee-OK-see-2'-FLOOR-oh-BAY-tuh-D-uh-RA-bih-noh-fyoor-A-noh-sil) YOOR-uh-sil)
                <table cellpadding="0" cellspacing="0" border="0" width="100%">
                    <tbody><tr>
                        <td width="100%" align="left">
                            A substance being studied in the treatment of advanced solid tumors and lymphomas. It blocks the growth of cells and may cause cancer cells to die. It is a type of uracil analog. Also called FAU.
                        </td>
                    </tr>
                </tbody></table>
                <br>
    <a name="1-(2’-deoxy-2’-fluoro-ß-D-arabinofuranosyl) uracil"></a>
    <a href="http://www.cancer.gov/dictionary?CdrID=642135" onclick="NCIAnalytics.TermsDictionaryResults(this,&#39;1&#39;);">
                    1-(2’-deoxy-2’-fluoro-ß-D-arabinofuranosyl) uracil</a>
&nbsp;&nbsp;
    <a href="http://www.cancer.gov/PublishedContent/Media/CDR/media/723011.mp3" id="audioLink723011" class="CDR_audiofile" shape="rect"><img src="./NCI Dictionary of Cancer Terms - National Cancer Institute_files/audio-icon.gif" alt="listen" border="0" height="16" width="16"></a>
&nbsp;&nbsp; (1-(2'-dee-OK-see-2'-FLOOR-oh-BAY-tuh-D-uh-RA-bih-noh-fyoor-A-noh-sil) YOOR-uh-sil)
    <table cellpadding="0" cellspacing="0" border="0" width="100%">
                    <tbody><tr>
                        <td width="100%" align="left">
                            A substance being studied in the treatment of advanced solid tumors and lymphomas. It blocks the growth of cells and may cause cancer cells to die. It is a type of uracil analog. Also called FAU.
                        </td>
                    </tr>
                </tbody></table>
    <br>
 */
/*
 * without pronunciation
 *
                <a name="123I-MIBG"></a><a href="http://www.cancer.gov/dictionary?CdrID=736791" onclick="NCIAnalytics.TermsDictionaryResults(this,&#39;6&#39;);">
                    123I-MIBG</a> &nbsp;&nbsp;
                &nbsp;&nbsp;
                
                <table cellpadding="0" cellspacing="0" border="0" width="100%">
                    <tbody><tr>
                        <td width="100%" align="left">
                            A drug containing a form of radioactive iodine called I 123 that is used to detect certain types of tumors, including pheochromocytomas and neuroblastomas. Radiation from the I 123 may help show where cancer cells are in the body. 123I-MIBG is a type of radioimaging agent and a type of radioconjugate. Also called AdreView, iobenguane I 123,  and iodine I 123 metaiodobenzylguanidine.
                        </td>
                    </tr>
                </tbody></table>
                <br>
 */
/*
 * last two element of div...
 *
<!-- <img src="/images/spacer.gif" width="10" height="19" alt="" border="0"><br>-->
<a href="http://www.cancer.gov/dictionary?expand=%#top" class="backtotop-link"><img src="./NCI Dictionary of Cancer Terms - National Cancer Institute_files/backtotop_red.gif" alt="Back to Top" border="0">Back to Top</a>



<!-- Footer -->
<div id="footerzone" align="center">
    
</div>
 */
/*
 * with pronunciation
 *
#text
a
a
#text
a
#text
table
#text
br
 */
/*
 * without pronunciation
 *
#text
a
a
#text
table
#text
br
 */
/*
 * last nodes
 *
INFO: Node: |#text|
INFO: Node: |#comment|
INFO: Node: |#text|
INFO: Node: |a|
INFO: Node: |#text|
INFO: Node: |#text|
INFO: Node: |#comment|
INFO: Node: |#text|
INFO: Node: |div|
INFO: Node: |#text|
 */
        int n = 0;
        for (; iChild < numChildren; ++iNode, ++iChild, ++n) {
            Node node;
            Element e;
            if (numChildren - iChild < 4) {
                if (trace) {
                    for (; iNode < nodeChildren.size(); ++iNode) {
                        node = nodeChildren.get(iNode);
                        System.out.println("INFO: Node: |" + node.nodeName() + "|");
                    }
                    for (; iChild < numChildren; ++iChild) {
                        e = div.child(iChild);
                        System.out.println("INFO: Element: |" + e.nodeName() + "|");
                    }
                }
                break;
            }
            // by node
            node = nodeChildren.get(iNode++);
            assert node.nodeName().equals("#text") : "Not a \"#text\": |" + node.nodeName() + "|";
            node = nodeChildren.get(iNode++);
            assert node.nodeName().equals("a") : "Not a \"a\": |" + node.nodeName() + "|";
            node = nodeChildren.get(iNode++);
            assert node.nodeName().equals("a") : "Not a \"a\": |" + node.nodeName() + "|";
            node = nodeChildren.get(iNode++);
            assert node.nodeName().equals("#text") : "Not a \"#text\": |" + node.nodeName() + "|";
            node = nodeChildren.get(iNode);
            String  mtd = "";
            if (node.nodeName().equals("a")) {
                ++iNode;
                node = nodeChildren.get(iNode++);
                assert node.nodeName().equals("#text") : "Not a \"#text\": |" + node.nodeName() + "|";
                mtd = node.toString().replaceAll("&nbsp;", "").trim();
                if (!mtd.isEmpty()) {
                    assert mtd.startsWith("(") && mtd.endsWith(")") : "FATAL: \"(...)\" <> |" + mtd + "|";
                    mtd = mtd.substring(1, mtd.length() - 1);
                }
                node = nodeChildren.get(iNode);
            }
            assert node.nodeName().equals("table") : "Not a \"table\": |" + node.nodeName() + "|";
            node = nodeChildren.get(++iNode);
            assert node.nodeName().equals("#text") : "Not a \"#text\": |" + node.nodeName() + "|";
            node = nodeChildren.get(++iNode);
            assert node.nodeName().equals("br") : "Not a \"br\": |" + node.nodeName() + "|";
            // by element
            e = div.child(iChild++);
            assert e.nodeName().equals("a") : "Not a <a>: |" + e + "|";
            String word = e.attr("name");
            e = div.child(iChild++);
            assert e.nodeName().equals("a") : "Not a <a>: |" + e + "|";
            String word2 = e.text();
            assert word.equals(word2) : "|" + word + "| <> |" + word2 + "|";
            String mp3 = "";
            e = div.child(iChild);
            if (e.attr("class").equals("CDR_audiofile")) {
                assert e.nodeName().equals("a") : "Not a <a>: |" + e + "|";
                mp3 = e.attr("href");
                int idx = mp3.lastIndexOf("/");
                assert idx != -1 : "FATAL: \".../...mp3\" <> |" + mp3 + "|";
                mp3 = mp3.substring(idx + 1);
                e = div.child(++iChild);    // proceed to next element: <table>
            }
            String def = e.text();
            assert e.nodeName().equals("table") : "Not a <table>: |" + e + "|";
            String name = div.child(++iChild).nodeName();
            assert name == "br" : "Not <br>: |" + name + "|";

            if (trace) {
                System.out.println("|" + word + "|");
                System.out.println("\t|" + mp3 + "|");
                System.out.println("\t|" + mtd + "|");
                System.out.println("\t|" + def + "|");
                System.out.printf("JSON|'%s': ['%s', '%s', '%s']\n", Json.escapeString(word), Json.escapeString(mp3), Json.escapeString(mtd), Json.escapeString(def));
            }
            String[] sa = new String[] {word, mp3, mtd, def};
            ra.add(sa);
        }
        if (trace)
            System.out.println("INFO: # of result = " + numResult + " (" + n + ")");
        assert numResult == ra.size() : numResult + " <> " + ra.size();

        return ra;
    }



    public static void main(String[] args)
    throws java.io.IOException
    {
        boolean verbose = true;
        //http://www.cancer.gov/geneticsdictionary?expand=%
        //  "%23" "A", ..., "Z" "All" ["%"]
        String X = args.length == 0 ? "%23" : args[0];
        String geneticsUrlStr = "http://www.cancer.gov/geneticsdictionary?expand=" + X;
        //NationalCancerInstitute.genetics(geneticsUrlStr, verbose);
        File geneticsFile = new File("/media/RUBY/download/NCI/NCI Dictionary of Genetics Terms - National Cancer Institute.html");
        //NationalCancerInstitute.genetics(geneticsFile, verbose);

        //http://www.cancer.gov/dictionary?expand=%
        //  "%23" "A", ..., "Z" ["%"]
        String cancerUrlStr = "http://www.cancer.gov/dictionary?expand=" + X;
        NationalCancerInstitute.cancer(cancerUrlStr, verbose);
        File cancerFile = new File("/media/RUBY/download/NCI/NCI Dictionary of Cancer Terms - National Cancer Institute.html");
        //NationalCancerInstitute.cancer(cancerFile, verbose);
    }
}
