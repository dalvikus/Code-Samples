package com.dn2soft.dick;

import com.dn2soft.dick.dictionary.Cambridge;
import com.dn2soft.dick.DickW;
import com.dn2soft.dick.DickWrapper;
import com.dn2soft.dick.utility.GetOpt;

import java.util.List;

public class Dick {
    static final String optStr = "Wkafxd:";
    static final String optStrForExpert = "wsNvthHTRDcC";
    static void Usage(boolean showExpertOption)
    {
        String  s = "\n";
        s += "Usage:\n";
        s += "\t[[-" + optStr + (showExpertOption ? optStrForExpert : "") + "] word]*\n";
        s += "\n";
        s += "\t-W: run in gui\n";
        s += "\t-k: uk(Britain English); default American English\n";
        s += "\t-a: show all words/phrases/idioms related word; disabled in gui mode\n";
        s += "\t-f: skip database lookup\n";
        s += "\t-d: database pathname\n";
        if (!showExpertOption) {
            s += "\t-x: show hidden options for expert\n";
        } else {
            s += "\n";
            s += "Expert Only; works only along with -x option(first time only)\n";
            s += "\t-w: hover your mouse cursor over any word in the gui box\n";
            s += "\t    to keep looking up them\n";
            s += "\t-s: pronunciation\n";
            s += "\t-N: do not show related links; not used in gui mode\n";
            s += "\t-v: verbose; for debug\n";
            s += "\t-t: trace; too verbose\n";
            s += "\t-h: show href0(real URL); disabled in gui mode\n";
            s += "\t-H: show href0(real URL) only; -h implied; disabled in gui mode\n";
            s += "\t-T: no ANSI; implied with -W (gui mode)\n";
            s += "\t-R: act like robot\n";
            s += "\t-D: dump data in database\n";
            s += "\t-c: create database given in -d option\n";
            s += "\t-C: -c plus create tables\n";
        }
        System.out.println(s);
    }

    private static void createAndShowGUI(Cambridge.Flag flag)
    {
        //Create and set up the window.
        javax.swing.JFrame  frame = new javax.swing.JFrame("DickW");
        frame.setDefaultCloseOperation(javax.swing.JFrame.EXIT_ON_CLOSE);

        //Create and set up the content pane.
        javax.swing.JComponent  newContentPane = new DickW(flag);
        //newContentPane.setName("newContentPane");
        newContentPane.setOpaque(true);
        frame.setContentPane(newContentPane);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }

    public static void
    main(String[] args)
    throws
        InterruptedException,
        GetOpt.GetOptStrException, GetOpt.GetOptParseException
    {
        GetOpt.Option   option = GetOpt.getopt(args, optStr + optStrForExpert);
        List<String>    wordStrArgs = option.getArgs();
        boolean robot = option.hasKey("-R");
        boolean dump = option.hasKey("-D");
        boolean createDatabase = option.hasKey("-c");
        boolean createTables = option.hasKey("-C");
        String dbpath = null;
        if (option.hasKey("-d")) {
            dbpath =  option.getValue("-d");
        }
        if (createDatabase || createTables) {
            if (dbpath != null) {
                //System.out.printf("|%s|\n", dbpath);
                DickWrapper.createDatabase(dbpath, createTables);
//              System.exit(0);
            } else {
                System.err.println("ERR: database pathname not given: use -d dbpath");
                Usage(option.hasKey("-x"));
                System.exit(-1);
            }
        }
        if (dump) {
            DickWrapper.dump(dbpath);
            System.exit(0);
        }
        boolean gui = !robot && option.hasKey("-W");
        if (!gui && wordStrArgs.isEmpty()) {
            if (!createDatabase && !createTables) {
                Usage(option.hasKey("-x"));
            }
            return;
        }

        do {
            // lookup
            final Cambridge.Flag    flag    = new Cambridge.Flag();
            flag.uk             = option.hasKey("-k");
            flag.showAll        = option.hasKey("-a");
            flag.force          = option.hasKey("-f");
            flag.showLink       = !option.hasKey("-N");
            flag.playPron       = option.hasKey("-s");
            flag.ansi           = !option.hasKey("-T");
            flag.verbose        = option.hasKey("-v");
            flag.trace          = option.hasKey("-t");
            flag.showHref0      = option.hasKey("-h");
            flag.showHref0Only  = option.hasKey("-H");
            flag.followWord     = false;
            flag.resultOnly     = false;    // just print
            flag.dbpath         = dbpath;

            if (gui) {
                flag.showAll        = false;
                flag.showLink       = true;
                flag.ansi           = false;
                flag.showHref0      = false;
                flag.showHref0Only  = false;
                flag.followWord     = option.hasKey("-w");
                flag.resultOnly     = true; // just print
                javax.swing.SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        createAndShowGUI(flag);
                    }
                });
                break;
            }
            if (robot) {
                flag.playPron = false;
                flag.showAll = true;
                flag.resultOnly = true;
                DickWrapper.robot(wordStrArgs.get(0), flag);
            } else
                DickWrapper.lookup(wordStrArgs.get(0), flag);

            // rescan
            wordStrArgs.remove(0);
            option = GetOpt.getopt(wordStrArgs.toArray(new String[0]), optStr + optStrForExpert);
            wordStrArgs = option.getArgs();
        } while (!wordStrArgs.isEmpty());
    }
}
