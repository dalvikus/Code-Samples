package com.dn2soft.dicka;

import com.dn2soft.util.Jsonable;
import com.dn2soft.util.Json;
import com.dn2soft.util.JsonWalk;

import java.util.List;
import java.util.ArrayList;

import android.util.Log;

public class StringArrayArrayList implements Jsonable {
    public List<List<String>> mA;
    public StringArrayArrayList() {
        mA = new ArrayList<List<String>>();
    }
    public StringArrayArrayList(ArrayList<String[]> saal) {
        mA = new ArrayList<List<String>>(saal.size());
        for (String[] sa: saal) {
            List<String> a = new ArrayList<String>(sa.length);
            mA.add(a);
            for (String s: sa)
                a.add(s);
        }
    }
    public StringArrayArrayList(List<List<String>> saal) {
        mA = saal;
    }

    public static StringArrayArrayList set(String jsonStr)
    {
        if (jsonStr == null)
            return null;
        StringArrayArrayList saal = null;
        try {
            saal = (StringArrayArrayList) JsonWalk.setWalkClassJson(
                StringArrayArrayList.class,
                Json.setJson(jsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return saal;
    }

    public static String getJsonStr(StringArrayArrayList saal)
    {
        if (saal == null)
            return null;
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(StringArrayArrayList.class, saal);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
}
