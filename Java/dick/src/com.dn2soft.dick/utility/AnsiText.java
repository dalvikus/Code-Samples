package com.dn2soft.dick.utility;

import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
/**
 *
 */
public class AnsiText {
/*
$ dircolors -p
......
# Below are the color init strings for the basic file types. A color init
# string consists of one or more of the following numeric codes:
# Attribute codes:
# 00=none 01=bold 04=underscore 05=blink 07=reverse 08=concealed
# Text color codes:
# 30=black 31=red 32=green 33=yellow 34=blue 35=magenta 36=cyan 37=white
# Background color codes:
# 40=black 41=red 42=green 43=yellow 44=blue 45=magenta 46=cyan 47=white
......
 */

    public static class ColorAttr {
        String  color;
        String  attr;
        boolean background;
        public ColorAttr(String c, String a, boolean b) {
            color = c;
            attr = a;
            background = b;
        }
    };

    private static final Map<String, Integer>   COLOR_TABLE = new HashMap<String, Integer>() {{
        put("black", 30);
        put("red", 31);
        put("green", 32);
        put("yellow", 33);
        put("blue", 34);
        put("magenta", 35);
        put("cyan", 36);
        put("white", 37);
    }};

    private static final Map<String, Integer>   ATTR_TABLE = new HashMap<String, Integer>() {{
        put("none", 0);     // 0b0000
        put("bold", 1);     // 0b0001
        put("underscore", 4);   // 0b0100
        put("blink", 5);    // 0b0101
        put("reverse", 7);  // 0b0111
        put("concealed", 8);    // 0b1000
    }};

    public static String    colorText(String str, ColorAttr colorAttr)
    {
        return colorText(str, colorAttr, true);
    }
    public static String    colorText(String str, ColorAttr colorAttr, boolean ansi)
    {
        if (!ansi)
            return str;

        int iColor = COLOR_TABLE.get(colorAttr.color).intValue();
        if (colorAttr.background)
            iColor += 10;
        int iAttr = ATTR_TABLE.get(colorAttr.attr).intValue();
        return "[" + iAttr + ";" + iColor + "m" + str + "[0m";
    }

    public static void  testColor()
    {
        List<String>    colors = new ArrayList<String>() {{
            add("black");
            add("red");
            add("green");
            add("yellow");
            add("blue");
            add("magenta");
            add("cyan");
            add("white");
        }};
        List<String>    attrs = new ArrayList<String>() {{
            add("   none   ");
            add("   bold   ");
            add("underscore");
            add("   blink  ");
            add("  reverse ");
            add(" concealed");
        }};
        for (String color: colors) {
            for (int i = 0; i < 2; ++i) {
                boolean b = i == 0;
                for (String attr: attrs) {
                    System.out.format(" %s", AnsiText.colorText(attr, new AnsiText.ColorAttr(color, attr.trim(), b)));
                }
                System.out.println();
            }
        }
    }

    public static final ColorAttr   COLOR_ATTR_FATAL_HEAD = new ColorAttr("red", "none", true);
    public static final ColorAttr   COLOR_ATTR_FATAL = new ColorAttr("red", "none", false);
    public static final ColorAttr   COLOR_ATTR_WARN_HEAD = new ColorAttr("yellow", "none", true);
    public static final ColorAttr   COLOR_ATTR_WARN = new ColorAttr("yellow", "none", false);
    public static String    fatalStr(String s)
    {
        return fatalStr(s, true);
    }
    public static String    fatalStr(String s, boolean ansi)
    {
        return colorText("FATAL:", COLOR_ATTR_FATAL_HEAD, ansi) + "\t" + AnsiText.colorText(s, COLOR_ATTR_FATAL, ansi);
    }
    public static String    fatalStr0(String s)
    {
        return AnsiText.colorText(s, COLOR_ATTR_FATAL);
    }
    public static void  printFatal(String s) {
        System.err.println(fatalStr(s));
    }
    public static String    warnStr(String s)
    {
        return colorText("WARN:", COLOR_ATTR_WARN_HEAD) + "\t" + AnsiText.colorText(s, COLOR_ATTR_WARN);
    }
    public static String    warnStr0(String s)
    {
        return AnsiText.colorText(s, COLOR_ATTR_WARN);
    }
    public static void  printWarn(String s) {
        System.err.println(warnStr(s));
    }
}
