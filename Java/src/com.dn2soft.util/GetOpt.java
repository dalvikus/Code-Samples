package com.dn2soft.util;

import com.dn2soft.util.AnsiText;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

public class GetOpt {
    public static class GetOptStrException extends Exception {
        GetOptStrException(String message) {
            super(message);
        }
    }
    public static class GetOptParseException extends Exception {
        GetOptParseException(String message) {
            super(message);
        }
    }
    public static class Option {
        String      optStr;
        /*
        List<String[]>  opts = new ArrayList<String[]>();
         */
        Map<String, String> optm = new HashMap<String, String>();
        List<String>    args = new ArrayList<String>();
        public List<String> getArgs()
        {
            return args;
        }
        public void print()
        {
            System.out.println("INFO: optStr: \"" + optStr + "\"");
            /*
            System.out.println("INFO: opts:");
            for (String[] s2: opts)
                System.out.println("\t(" + s2[0] + ", " + s2[1] + ")");
             */
            System.out.println("INFO: optm:");
            for (Map.Entry<String, String> entry: optm.entrySet()) {
                System.out.println("\t|" + entry.getKey() + "|: |" + entry.getValue() + "|");
            }
            System.out.println("INFO: args:");
            System.out.println("\t" + args);
        }
        public boolean hasKey(String key)
        {
            return optm.containsKey(key);
            /*
            for (String[] s2: opts)
                if (s2[0].equals(key))
                    return true;
            return false;
             */
        }
        public String getValue(String key)
        {
            return optm.get(key);
            /*
            for (String[] s2: opts)
                if (s2[0].equals(key))
                    return s2[1];
            return null;
             */
        }
    }
    private static Map<String, Boolean> parseOptStr(String optStr, boolean ansi) throws GetOptStrException
    {
        Map<String, Boolean>    optMap = new HashMap<String, Boolean>();
        String[]    sa = optStr.split("");
        assert sa[0].length() == 0 : "sa[0] != \"\"";
        for (int i = 1; i < sa.length; ++i) {
            if (sa[i].equals(":"))
                throw new GetOptStrException(AnsiText.fatalStr("Unhandled \":\"", ansi));
            if (!sa[i].matches("[a-zA-Z]"))
                throw new GetOptStrException(AnsiText.fatalStr(sa[i] + ": Invalid keys: [a-zA-z] only", ansi));
            if (optMap.containsKey(sa[i]))
                throw new GetOptStrException(AnsiText.fatalStr(sa[i] + ": Duplicated option key", ansi));
            if (i + 1 < sa.length && sa[i + 1].equals(":")) {
                optMap.put(sa[i], true);
                ++i;
            } else
                optMap.put(sa[i], false);
        }
        /*
        for (Map.Entry<String, Boolean> entry: optMap.entrySet()) {
            System.out.println("\t|" + entry.getKey() + "|: |" + entry.getValue() + "|");
        }
         */
        if (optMap.entrySet().size() == 0)
            throw new GetOptStrException(AnsiText.fatalStr("\"" + optStr + "\": No option keys", ansi));
        return optMap;
    }
    public static Option    getopt(String[] args, String optStr)
    throws GetOptStrException, GetOptParseException
    {
        return getopt(args, optStr, true);
    }
    public static Option    getopt(String[] args, String optStr, boolean ansi) throws GetOptStrException, GetOptParseException {
        Option  option = new Option();
        option.optStr = optStr;
        Map<String, Boolean>    optMap = parseOptStr(optStr, ansi);

        int index = 0;
        for (; index < args.length; ++index) {
            if (
                args[index].equals("-") ||
                !args[index].startsWith("-")
            )
                break;
            String[]    sa = args[index].split("");
            assert sa[0].length() == 0 : "sa[0] != \"\"";
            // skip "", "-"
            for (int i = 2; i < sa.length; ++i) {
                if (!sa[i].matches("[a-zA-Z]"))
                    throw new GetOptParseException(AnsiText.fatalStr(sa[i] + " in \"" + args[index] + "\": Invalid key: [a-zA-Z] only: optStr:= \"" + optStr + "\"", ansi));
                if (!optMap.containsKey(sa[i]))
                    throw new GetOptParseException(AnsiText.fatalStr(sa[i] + " in \"" + args[index] + "\": Not an option key: optStr:= \"" + optStr + "\"", ansi));
                String[]    str2 = new String[2];
            try {
                str2[0] = "-" + sa[i];
                if (!optMap.get(sa[i])) {
                    str2[1] = "";
                } else {
                    if (i + 1 == sa.length) {
                        if (index + 1 == args.length)
                            throw new GetOptParseException(AnsiText.fatalStr(sa[i] + " in \"" + args[index] + "\": No option value: optStr:= \"" + optStr + "\"", ansi));
                        str2[1] = args[index + 1];
                        ++index;
                        break;
                    } else {
                        str2[1] = sa[i + 1];
                        ++i;
                    }
                }
            } finally {
                /*
                option.opts.add(str2);
                 */
                option.optm.put(str2[0], str2[1]);
            }
            }
        }
        for (; index < args.length; ++index)
            option.args.add(args[index]);
        return option;
    }
}
