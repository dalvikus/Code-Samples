package com.dn2soft.util;

/**
 * class JsonX extends JsonAbs {
 *  ....
 * }
 *
 * JsonX jsonX = (JsonX) new JsonX().set(jsonStr, JsonX.class);
 * String jsonStr = jsonX.set(JsonX.class);
 */
public class JsonAbs implements JsonableEx {
    public String set(Class cls)
    {
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(cls, this);
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return jsonStr;
    }
    public Object set(String jsonStr, Class cls)
    {
        if (jsonStr == null)
            return null;
        Object obj = null;
        try {
            obj = JsonWalk.setWalkClassJson(
                cls,
                Json.setJson(jsonStr)
            );
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        return obj;
    }
}
