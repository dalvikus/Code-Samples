package com.dn2soft.util;

import java.util.List;

public class Joint {
    static public String join(List<String> str_list, String glue) {
        String  s = "";
        int k = 0;
        for (String str: str_list)
            s += (k++ == 0 ? "" : glue) + str;
        return s;
    }
}
