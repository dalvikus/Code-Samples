package com.dn2soft.util;

/**
 * extends JsonAbs, and use it; see JsonAbs clas
 */
interface JsonableEx extends Jsonable {
    String set(Class cls);
    Object set(String s, Class cls);
}
