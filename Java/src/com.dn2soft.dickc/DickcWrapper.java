package com.dn2soft.dickc;

import com.dn2soft.dickc.dictionary.Cambridge;
import com.dn2soft.util.AnsiText;
//import com.dn2soft.db.Derby;
//import com.dn2soft.db.WordSmartDerby;
import com.dn2soft.db.CambridgeDictionarySQLite;
import com.dn2soft.util.Json;

import java.io.IOException;

public class DickcWrapper {
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
    public static Cambridge.Result
    lookup(
        String  wordStr,
        Cambridge.Flag    flag
    )
    throws ClassNotFoundException, java.sql.SQLException
    {
        if (wordStr == null || wordStr.isEmpty()) {
            if (flag.verbose || flag.trace)
                println(AnsiText.fatalStr("No wordStr"));
            return null;
        }
        if (!wordStr.startsWith("http://"))
            wordStr = wordStr.trim().replaceAll("\\s+", " ");

        boolean isConnected = false;
        //WordSmartDerby db = null;
        CambridgeDictionarySQLite   db = null;
        if (flag.dbpath != null) {
            //db = new WordSmartDerby(flag.dbpath);
            db = new CambridgeDictionarySQLite(flag.dbpath);
            isConnected = db.getConnection() != null;
        } else {
            isConnected = false;
        }
        String  href0 = null;
        try {
            if (isConnected && !flag.force) {
                try {
                    href0 = db.href0(wordStr);
                } catch (java.sql.SQLException e) {
                    //Derby.printSQLException(e);
                    System.err.println(e.getClass().getName() + ": " + e.getMessage());
                    href0 = null;
                }
            }
            if (href0 == null)
                href0 = Cambridge.getHref0(wordStr, flag);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        if (href0 == null) {
            System.err.println(AnsiText.fatalStr(wordStr) + ": no luck");
            return null;
        }
        if (href0.contains("spellcheck")) {
            System.out.println(AnsiText.warnStr0(wordStr) + ": not found");
            System.out.println("Try these:");
            try {
                for (String w: Cambridge.spellCheck(href0)) {
                    System.out.println("\t" + AnsiText.warnStr0(w));
                }
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
            return null;
        }
        // sanitize href0
        int index = href0.indexOf("?"); // index == -1: URL given, index > -1: word given
        href0 = index == -1 ? href0 : href0.substring(0, index);

        if (flag.showHref0Only)
            flag.showHref0 = true;  // forced
        if (flag.showHref0)
            Cambridge.printInfo(href0);
        if (flag.showHref0Only)
            return null;

        // try to get info from database with href0
        String JSONStr = null;
        if (isConnected && !flag.force) {
            try {
                JSONStr = db.select0(href0, flag.showAll);
            } catch (java.sql.SQLException e) {
                //Derby.printSQLException(e);
                System.err.println(e.getClass().getName() + ": " + e.getMessage());
                JSONStr = null;
            }
        }
        Cambridge.Result    result = null;
        int nAddedPage = 0;
        if (JSONStr == null) {
            try {
                result = Cambridge.getResultFromWeb(href0, flag);
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
        } else {
            Json.BaseType  json = Json.setJson(JSONStr);
            result = new Cambridge.Result();
            ////result.setDoc2(json);
            result.setDoc(json, flag.showAll);
            for (Cambridge.Result.Page page: result.doc) {
                if (page.content.size() == 0) { // not filled yet, i.e., no record in "Web" table
                    System.err.printf(".");
                    ++nAddedPage;
                    try {
                        Cambridge.lookup0(flag, page);
                    } catch (IOException e) {
                        e.printStackTrace();
                        return null;
                    }
                }
            }
            if (nAddedPage > 0) {
                System.err.printf("\r");
                for (int i = 0; i < nAddedPage; ++i)
                    System.err.printf(" ");
                System.err.printf("\r");
            }
        }
        result.wordStr = wordStr;
        if (flag.playPron) {
            try {
                Cambridge.play(Cambridge.getAudioURL(result));
            }
            catch (IOException e) {
                e.printStackTrace();
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (flag.downloadAll) {
                Cambridge.downloadAudio(result);
            }
        }
        if (!flag.resultOnly)
            System.out.println(result.getStr(flag.showAll, true));
        if (isConnected && JSONStr == null || nAddedPage > 0) {
            try {
                db.insert(result, flag.force);
            } catch (java.sql.SQLException e) {
                //Derby.printSQLException(e);
                System.err.println(e.getClass().getName() + ": " + e.getMessage());
            }
        }
        return result;
    }

    public static Cambridge.Result
    robot(
        String  wordStr,
        Cambridge.Flag    flag
    )
    throws ClassNotFoundException, java.sql.SQLException
    {
        if (wordStr == null || wordStr.isEmpty()) {
            if (flag.verbose || flag.trace)
                println(AnsiText.fatalStr("No wordStr"));
            return null;
        }

        String  href0 = null;
        try {
            href0 = Cambridge.getHref0(wordStr, flag);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        if (href0 == null) {
            System.err.println(AnsiText.fatalStr(wordStr) + ": no luck");
            return null;
        }
        if (href0.contains("spellcheck")) {
            System.out.println(AnsiText.warnStr0(wordStr) + ": not found");
            System.out.println("Try these:");
            try {
                for (String w: Cambridge.spellCheck(href0)) {
                    System.out.println("\t" + AnsiText.warnStr0(w));
                }
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
            return null;
        }
        // sanitize href0
        int index = href0.indexOf("?"); // index == -1: URL given, index > -1: word given
        href0 = index == -1 ? href0 : href0.substring(0, index);

        if (flag.showHref0Only)
            flag.showHref0 = true;  // forced
        if (flag.showHref0)
            Cambridge.printInfo(href0);
        if (flag.showHref0Only)
            return null;

        boolean isConnected = false;
        //WordSmartDerby db = null;
        CambridgeDictionarySQLite db = null;
        if (flag.dbpath != null) {
            //db = new WordSmartDerby(flag.dbpath);
            db = new CambridgeDictionarySQLite(flag.dbpath);
            isConnected = db.getConnection() != null;
        } else {
            isConnected = false;
        }
        try {
            db.dumpAudio();
        } catch (java.sql.SQLException e) {}
        if (true)
            return null;
        // try to get info from database with href0
        String JSONStr = null;
        if (isConnected && !flag.force) {
            try {
                JSONStr = db.select0(href0, flag.showAll);
            } catch (java.sql.SQLException e) {
                //Derby.printSQLException(e);
                System.err.println(e.getClass().getName() + ": " + e.getMessage());
                JSONStr = null;
            }
        }
        Cambridge.Result    result = null;
        int nAddedPage = 0;
        if (JSONStr == null) {
            try {
                result = Cambridge.getResultFromWeb(href0, flag);
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
        } else {
            Json.BaseType  json = Json.setJson(JSONStr);
            result = new Cambridge.Result();
            ////result.setDoc2(json);
            result.setDoc(json, flag.showAll);
            for (Cambridge.Result.Page page: result.doc) {
                if (page.content.size() == 0) { // not filled yet, i.e., no record in "Web" table
                    System.err.printf(".");
                    ++nAddedPage;
                    try {
                        Cambridge.lookup0(flag, page);
                    } catch (IOException e) {
                        e.printStackTrace();
                        return null;
                    }
                }
            }
            if (nAddedPage >= 0) {
                System.err.printf("\r");
                for (int i = 0; i < nAddedPage; ++i)
                    System.err.printf(" ");
                System.err.printf("\r");
            }
        }
        System.out.println("|" + wordStr + "|");
        for (Cambridge.Result.Page page: result.doc) {
            for (Cambridge.Block block: page.content) {
                String w = block.head.word;
                String s = block.head.getAudioURL();
                try {
                    String p = Cambridge.play(s, true);
                    System.out.printf("<%s>\n\t|%s|\n\t|%s|\n", w, s, p);
                }
                catch (IOException e) {
                    e.printStackTrace();
                }
                catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        result.wordStr = wordStr;
        if (flag.playPron) {
            try {
                Cambridge.play(Cambridge.getAudioURL(result));
            }
            catch (IOException e) {
                e.printStackTrace();
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        if (!flag.resultOnly)
            System.out.println(result.getStr(flag.showAll, true));
        if (isConnected && (JSONStr == null || nAddedPage > 0)) {
            try {
                db.insert(result, flag.force);
            } catch (java.sql.SQLException e) {
                //Derby.printSQLException(e);
                System.err.println(e.getClass().getName() + ": " + e.getMessage());
            }
        }
        return result;
    }
}
