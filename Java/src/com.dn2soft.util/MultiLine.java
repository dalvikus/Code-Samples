package com.dn2soft.util;

import java.util.List;
import java.util.ArrayList;

public class MultiLine {
    static public List<String>  split(String long_line, int width) {
        assert width > 0 : "width(" + width + ") should be positive";
        List<String>    str_list = new ArrayList<String>();
        String[]    s_a = long_line.split("\\s+");
        int len = 0;
        String  str = "";
        for (String s: s_a) {
            int n = s.length();
            if (len > 0 && (len + 1 + n) > width) {
                str_list.add(str);
                str = s;
                len = n;
            } else {
                str += (len > 0 ? " " : "") + s;
                len += (len > 0 ? 1 : 0) + n;
            }
        }
        if (len > 0)
            str_list.add(str);
        return str_list;
    }
}
