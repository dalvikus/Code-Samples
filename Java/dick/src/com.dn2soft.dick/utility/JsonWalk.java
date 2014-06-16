package com.dn2soft.dick.utility;

import com.dn2soft.dick.utility.Joint;
import com.dn2soft.dick.utility.Jsonable;
import com.dn2soft.dick.utility.Json;
import com.dn2soft.dick.utility.AnsiText;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;

import java.lang.reflect.Field;
import java.lang.reflect.Type;
import java.lang.reflect.GenericArrayType;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.TypeVariable;
import java.lang.reflect.Array;

public class JsonWalk {
////////////////////////////////////////////////////////////////////////////////
final static String sycophant = "[{'href': 'http://dictionary.cambridge.org/dictionary/british/sycophantic', 'content': [{'head': {'word': 'sycophantic','gw': '','pos': ['adjective'],'pron': ['http://dictionary.cambridge.org/media/british/uk_pron/u/uks/uksyb/uksybar005.mp3','http://dictionary.cambridge.org/media/british/uk_pron_ogg/u/uks/uksyb/uksybar005.ogg','http://dictionary.cambridge.org/media/british/us_pron/e/eus/eus74/eus74458.mp3','http://dictionary.cambridge.org/media/british/us_pron_ogg/e/eus/eus74/eus74458.ogg'],'ipa': ['ˌsɪk.əˈfæn.tɪk']}, 'bodyList': [{'def': '(of a person or of behaviour) praising people in authority in a way that is not sincere, usually in order to get some advantage from them:','example': ['There was sycophantic laughter from the audience at every one of his terrible jokes.']}], 'jsonStrWordSmart': 'null'},{'head': {'word': 'sycophancy','gw': '','pos': ['noun'],'pron': ['http://dictionary.cambridge.org/media/british/uk_pron/e/epd/epd27/epd27085.mp3','http://dictionary.cambridge.org/media/british/uk_pron_ogg/e/epd/epd27/epd27085.ogg','http://dictionary.cambridge.org/media/british/us_pron/c/cus/cus02/cus02014.mp3','http://dictionary.cambridge.org/media/british/us_pron_ogg/c/cus/cus02/cus02014.ogg'],'ipa': ['ˈsɪk.ə.fən.si']}, 'bodyList': [], 'jsonStrWordSmart': 'null'},{'head': {'word': 'sycophant','gw': '','pos': ['noun'],'pron': ['http://dictionary.cambridge.org/media/british/uk_pron/u/uks/uksyb/uksybar004.mp3','http://dictionary.cambridge.org/media/british/uk_pron_ogg/u/uks/uksyb/uksybar004.ogg','http://dictionary.cambridge.org/media/british/us_pron/e/eus/eus74/eus74457.mp3','http://dictionary.cambridge.org/media/british/us_pron_ogg/e/eus/eus74/eus74457.ogg'],'ipa': ['ˈsɪk.ə.fænt']}, 'bodyList': [{'def': '','example': ['The prime minister is surrounded by sycophants.']}], 'jsonStrWordSmart': 'null'}], 'more': {'infoList': []}}]";
    final static String dick = "[{'href': 'http://dictionary.cambridge.org/dictionary/british/dick_1', 'content': [{'head': {'word': 'dick','gw': '','pos': ['noun'],'pron': ['http://dictionary.cambridge.org/media/british/uk_pron/u/ukd/ukdia/ukdiaph014.mp3','http://dictionary.cambridge.org/media/british/uk_pron_ogg/u/ukd/ukdia/ukdiaph014.ogg','http://dictionary.cambridge.org/media/british/us_pron/d/dic/dick_/dick.mp3','http://dictionary.cambridge.org/media/british/us_pron_ogg/d/dic/dick_/dick.ogg'],'ipa': ['dɪk']}, 'bodyList': [{'def': 'a penis','example': []}], 'jsonStrWordSmart': 'null'}], 'more': {'infoList': [{'word': 'dick','pos': 'noun','gw': '(PERSON)','href': 'http://dictionary.cambridge.org/dictionary/british/dick_2','arl0': 1, 'arl': 1},{'word': 'spotted dick','pos': 'noun','gw': '','href': 'http://dictionary.cambridge.org/dictionary/british/spotted-dick','arl0': 2, 'arl': 2},{'word': 'Tom, Dick, and/or Harry','pos': 'noun','gw': '','href': 'http://dictionary.cambridge.org/dictionary/british/tom-dick-and-or-harry','arl0': 2, 'arl': 2},{'word': 'clever dick/clogs','pos': 'idiom','gw': '','href': 'http://dictionary.cambridge.org/dictionary/british/clever-dick-clogs','arl0': 7, 'arl': 7}]}}]";
    public static class Block implements Jsonable {
        public static class Head implements Jsonable {
            public String   word;
            public String   gw;
            public List<String>   pos = new ArrayList<String>();
            public String[] pron;//  uk_mp3, uk_ogg, us_mp3, us_ogg;
            public List<String> ipa = new ArrayList<String>();
        }
        public static class Body implements Jsonable {
            public String   def;
            public List<String> example = new ArrayList<String>();
        }
        public Head head = new Head();
        public List<Body>   bodyList = new ArrayList<Body>();
        public String       jsonStrWordSmart = null;
    }
    public static class More implements Jsonable {
        public static class Info implements Jsonable {
            public String   href;   //
            int arl;                //
            String  arl0;           //
            public String   word;   //
            public String   pos;    //
            public String   gw;     //
        }

        public List<Info>   infoList = new ArrayList<Info>();
    }
    static abstract class ResultBase {
        public String       wordStr = null;
        public List<String> ErrorMessageList = null;
        public String       WarnMessage = null;
        public List<String> tryWordList = new ArrayList<String>();
    }
    public static class Result extends ResultBase implements Jsonable {
            // boolean, byte, short, int, long, char, float, and double
int[] a = new int[0];
String[] b = new String[2];
boolean boolean0;
byte byte0;
short short0;
int int0;
long long0;
char char0;
float float0;
double double0;
List<String> string0;
Map<String, String> map0;
String[] sa0;
        public static class Page implements Jsonable {
            public String       href;
            public List<Block>  content = new ArrayList<Block>();
            public More         more = new More();
        }
        public List<Page>   doc = new ArrayList<Page>();
        public Map<String, String> x = new HashMap<String, String>();
        public Map<String, String>[] y;//= new HashMap[0];
    }
////////////////////////////////////////////////////////////////////////////////


    final static String STRING = "java.lang.String";
    final static String LIST = "java.util.List";
    final static String MAP = "java.util.Map";

    static boolean TRACE = false;

    private static void
    println(Object o)
    {
        System.out.println(o);
    }
/*
 * NOTE
 *
 * See the last paragraph of a page:
 *  http://docs.oracle.com/javase/tutorial/reflect/member/fieldTypes.html
 *
Field.getGenericType() will consult the Signature Attribute in the class file if it's present. If the attribute isn't available, it falls back on Field.getType() which was not changed by the introduction of generics. The other methods in reflection with name getGenericFoo for some value of Foo are implemented similarly.
 */
    private static boolean isJsonable(Class c)
    {
        for (Class i: c.getInterfaces()) {
            if (i.getName().equals(Jsonable.class.getName()))
                return true;
        }
        Class c0 = c.getSuperclass();
        return c0 == null ? false : isJsonable(c0);
    }

    static String INDENT_PARAMETRIZEDTYPE = JsonWalk.TRACE ? "<>" : "  ";
    static String INDENT_LIST = JsonWalk.TRACE ? "[]" : "  ";
    static String INDENT_MAP = JsonWalk.TRACE ? "{}" : "  ";
    static String INDENT_TYPE = JsonWalk.TRACE ? "TT" : "  ";
    static String INDENT_CLASS = JsonWalk.TRACE ? "CC" : "  ";


////////////////////////////////////////////////////////////////////////////////{
/*
 * it walks all necessary fields given class
 * it is a building block of all others.
 */
////////////////////////////////////////////////////////////////////////////////
    private static String walkParametrizedType(Type type, String indent)
    throws ClassNotFoundException
    {
        final String traceStr0 = "walkParametrizedType";
        final String nextIndent = indent + JsonWalk.INDENT_PARAMETRIZEDTYPE;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + type + "|");
        if (!(type instanceof ParameterizedType)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not ParameterizedType: |" + type + "|");
            throw new IllegalArgumentException(msg);
        }
        ParameterizedType paraType = (ParameterizedType) type;
        String rawClassStr = ((Class) paraType.getRawType()).getName();
        if (rawClassStr.equals(JsonWalk.LIST)) {
            return JsonWalk.walkList(paraType, nextIndent);
        } else if (rawClassStr.equals(JsonWalk.MAP)) {
            return JsonWalk.walkMap(paraType, nextIndent);
        }
        String msg = AnsiText.fatalStr0(traceStr0 + ": rawClass: List or Map only: |" + rawClassStr + "|");
        throw new ClassNotFoundException(msg);
    }
    private static String walkList(ParameterizedType paraType, String indent)
    throws ClassNotFoundException
    {
        final String traceStr0 = "walkList";
        final String nextIndent = indent + JsonWalk.INDENT_LIST;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + paraType + "|");
        Type[] types = paraType.getActualTypeArguments();
        if (types.length != 1) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": # of types (" + types.length + ") <> 1");
            throw new IllegalArgumentException(msg);
        }
        Type type = types[0];
////////
////////......
////////
        String str = walkType(type, nextIndent);
        return '[' + str + ']';
    }
    private static String walkMap(ParameterizedType paraType, String indent)
    throws ClassNotFoundException
    {
        final String traceStr0 = "walkMap";
        final String nextIndent = indent + JsonWalk.INDENT_MAP;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + paraType + "|");
        Type[] types = paraType.getActualTypeArguments();
        if (types.length != 2) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": # of types (" + types.length + ") <> 2");
            throw new IllegalArgumentException(msg);
        }
        Type keyType = types[0];
        if (!(keyType instanceof Class)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": keyClass: Not Class: |" + keyType + "|");
            throw new IllegalArgumentException(msg);
        }
        Class keyClass = (Class) keyType;
        if (!keyClass.getName().equals(JsonWalk.STRING)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": keyClass: " + JsonWalk.STRING + " only: |" + keyClass.getName() + "|");
            throw new ClassNotFoundException(msg);
        }
        Type valType = types[1];
////////
////////......
////////
        String str = walkType(valType, nextIndent);
        return '{' + str + '}';
    }
    private static String walkType(Type type, String indent)
    throws ClassNotFoundException
    {
        final String traceStr0 = "walkType";
        final String nextIndent = indent + JsonWalk.INDENT_TYPE;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + type + "|");
        if (type instanceof Class) {
            return walkClass((Class) type, nextIndent);
        } else if (type instanceof ParameterizedType) {
            return walkParametrizedType((ParameterizedType) type, nextIndent);
        }
        if (type instanceof GenericArrayType) {
            // find "raw" class
            GenericArrayType gaType = (GenericArrayType) type;
            Type t;
            while ((t = gaType.getGenericComponentType()) instanceof GenericArrayType)
                gaType = (GenericArrayType) t;
            Class c = (Class) ((ParameterizedType) t).getRawType();
            String rawClassStr = c.getName();
            if (!rawClassStr.equals(JsonWalk.MAP)) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": GenericArrayType: rawClass: Map only: |" + rawClassStr + "|");
                throw new ClassNotFoundException(msg);
            }
////////
////////......
////////
            String str = JsonWalk.walkType(((GenericArrayType) type).getGenericComponentType(), nextIndent);
            return '[' + str + ']';
        }
        String msg = AnsiText.fatalStr0(traceStr0 + ": Class or ParameterizedType only: |" + type.toString() + "|");
        throw new ClassNotFoundException(msg);
    }
    public static String walkClass(Class cls, String indent)
    throws ClassNotFoundException
    {
        final String traceStr0 = "walkClass";
        final String nextIndent = indent + JsonWalk.INDENT_CLASS;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + cls + "|");
        TypeVariable[] tv = cls.getTypeParameters();
        if (tv.length > 0) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Non-generic class only: |" + cls + "|");
            throw new ClassNotFoundException(msg);
        }

        if (cls.isArray()) {
            Class c = cls.getComponentType();
////////
////////......
////////
            String str = walkClass(c, nextIndent);
            return '[' + str + ']';
        }

        if (cls.isPrimitive()) {
            // boolean, byte, short, int, long, char, float, and double
            String str = cls.getSimpleName();
////////
////////......
////////
            return "'" + str + "'";
        }

        if (cls.getName().equals(JsonWalk.STRING)) {
////////
////////......
////////
            return "'String'";
        }

        if (!JsonWalk.isJsonable(cls)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Jsonable only: |" + cls + "|");
            throw new ClassNotFoundException(msg);
        }

////////
////////......
////////
        // as it is; no herited members, but private members included
        // See the page:
        //  http://docs.oracle.com/javase/tutorial/reflect/class/classMembers.html
        List<String> sa = new ArrayList<String>();
        for (Field field: cls.getDeclaredFields()) {
            String key = field.getName();
            if (JsonWalk.TRACE)
                println(indent + traceStr0 + ": Field: |" + key + "|");
            if (field.isSynthetic()) {
                // ignore: see the end of page:
                //  http://docs.oracle.com/javase/tutorial/reflect/member/fieldModifiers.html
                if (JsonWalk.TRACE)
                    println(indent + traceStr0 + "    isSynthetic");
                continue;
            }
            if (field.isEnumConstant()) {
                // ignore for simplicity; use other type
                if (JsonWalk.TRACE)
                    println(indent + traceStr0 + "    isEnumConstant");
                continue;
            }
            Type fieldType = field.getGenericType();
////////
////////......
////////
            String str = walkType(fieldType, nextIndent);
            sa.add(String.format("'%s': %s", key, str));
        }
        return String.format("{%s}", sa.isEmpty() ? "" : "\n" + indent + "    " + Joint.join(sa, ",\n" + indent + "    ") + "\n" + indent);
    }
////////////////////////////////////////////////////////////////////////////////}


////////////////////////////////////////////////////////////////////////////////{
/*
 * given class, return json string
 */
////////////////////////////////////////////////////////////////////////////////
    private static String walkParametrizedType(Type type, Object object, String indent)
    throws ClassNotFoundException, IllegalAccessException
    {
        final String traceStr0 = "walkParametrizedType";
        final String nextIndent = indent + JsonWalk.INDENT_PARAMETRIZEDTYPE;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + type + "|");
        if (!(type instanceof ParameterizedType)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not ParameterizedType: |" + type + "|");
            throw new IllegalArgumentException(msg);
        }
        ParameterizedType paraType = (ParameterizedType) type;
        String rawClassStr = ((Class) paraType.getRawType()).getName();
        if (rawClassStr.equals(JsonWalk.LIST)) {
            return JsonWalk.walkList(paraType, object, nextIndent);
        } else if (rawClassStr.equals(JsonWalk.MAP)) {
            return JsonWalk.walkMap(paraType, object, nextIndent);
        }
        String msg = AnsiText.fatalStr0(traceStr0 + ": rawClass: List or Map only: |" + rawClassStr + "|");
        throw new ClassNotFoundException(msg);
    }
    private static String walkList(ParameterizedType paraType, Object object, String indent)
    throws ClassNotFoundException, IllegalAccessException
    {
        final String traceStr0 = "walkList";
        final String nextIndent = indent + JsonWalk.INDENT_LIST;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + paraType + "|");
        Type[] types = paraType.getActualTypeArguments();
        if (types.length != 1) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": # of types (" + types.length + ") <> 1");
            throw new IllegalArgumentException(msg);
        }
        Type type = types[0];
////////
////////......
////////
        if (object == null)
            return Json.NULL;
        if (!(object instanceof List)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof List: |" + object + "|");
            throw new IllegalArgumentException(msg);
        }
        List<String> sa = new ArrayList<String>();
        for (Object o: (List<?>) object) {
            String str = walkType(type, o, nextIndent);
            sa.add(str);
        }
        return '[' + (sa.isEmpty() ? "" : Joint.join(sa, ",")) + ']';
    }
    private static String walkMap(ParameterizedType paraType, Object object, String indent)
    throws ClassNotFoundException, IllegalAccessException
    {
        final String traceStr0 = "walkMap";
        final String nextIndent = indent + JsonWalk.INDENT_MAP;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + paraType + "|");
        Type[] types = paraType.getActualTypeArguments();
        if (types.length != 2) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": # of types (" + types.length + ") <> 2");
            throw new IllegalArgumentException(msg);
        }
        Type keyType = types[0];
        if (!(keyType instanceof Class)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": keyClass: Not Class: |" + keyType + "|");
            throw new IllegalArgumentException(msg);
        }
        Class keyClass = (Class) keyType;
        if (!keyClass.getName().equals(JsonWalk.STRING)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": keyClass: " + JsonWalk.STRING + " only: |" + keyClass.getName() + "|");
            throw new ClassNotFoundException(msg);
        }
        Type valType = types[1];
////////
////////......
////////
        if (object == null)
            return Json.NULL;
        if (!(object instanceof Map)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Map: |" + object + "|");
            throw new IllegalArgumentException(msg);
        }
        Map<?, ?> map = (Map<?, ?>) object;
        List<String> sa = new ArrayList<String>();
        for (Object key: map.keySet()) {
            String str = walkType(valType, map.get(key), nextIndent);
            sa.add(String.format("'%s':%s", key, str));
        }
        return '{' + (sa.isEmpty() ? "" : Joint.join(sa, ",")) + '}';
    }
    private static String walkType(Type type, Object object, String indent)
    throws ClassNotFoundException, IllegalAccessException
    {
        final String traceStr0 = "walkType";
        final String nextIndent = indent + JsonWalk.INDENT_TYPE;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + type + "|");
        if (type instanceof Class) {
            return walkClass((Class) type, object, nextIndent);
        } else if (type instanceof ParameterizedType) {
            return walkParametrizedType((ParameterizedType) type, object, nextIndent);
        }
        if (type instanceof GenericArrayType) {
            // find "raw" class
            GenericArrayType gaType = (GenericArrayType) type;
            Type t;
            while ((t = gaType.getGenericComponentType()) instanceof GenericArrayType)
                gaType = (GenericArrayType) t;
            Class c = (Class) ((ParameterizedType) t).getRawType();
            String rawClassStr = c.getName();
            if (!rawClassStr.equals(JsonWalk.MAP)) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": GenericArrayType: rawClass: Map only: |" + rawClassStr + "|");
                throw new ClassNotFoundException(msg);
            }
////////
////////......
////////
            if (object == null)
                return Json.NULL;
            List<String> sa = new ArrayList<String>();
            for (int i = 0; i < Array.getLength(object); ++i) {
                String str = JsonWalk.walkType(((GenericArrayType) type).getGenericComponentType(), Array.get(object, i), nextIndent);
                sa.add(str);
            }
            return '[' + (sa.isEmpty() ? "" : Joint.join(sa, ",")) + ']';
        }
        String msg = AnsiText.fatalStr0(traceStr0 + ": Class or ParameterizedType only: |" + type.toString() + "|");
        throw new ClassNotFoundException(msg);
    }
    public static String walkClass(Class cls, Object object, String indent)
    throws ClassNotFoundException, IllegalAccessException
    {
        final String traceStr0 = "walkClass";
        final String nextIndent = indent + JsonWalk.INDENT_CLASS;
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + cls + "|");
        TypeVariable[] tv = cls.getTypeParameters();
        if (tv.length > 0) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Non-generic class only: |" + cls + "|");
            throw new ClassNotFoundException(msg);
        }

        if (cls.isArray()) {
            Class c = cls.getComponentType();
////////
////////......
////////
            if (object == null)
                return Json.NULL;
            List<String> sa = new ArrayList<String>();
            for (int i = 0; i < Array.getLength(object); ++i) {
                String str = walkClass(c, Array.get(object, i), nextIndent);
                sa.add(str);
            }
            return '[' + (sa.isEmpty() ? "" : Joint.join(sa, ",")) + ']';
        }

        if (cls.isPrimitive()) {
            // boolean, byte, short, int, long, char, float, and double
            String str = cls.getSimpleName();
////////
////////......
////////
            if (str.equals("boolean")) {
                if (!(object instanceof Boolean)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Boolean: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Boolean) object).toString());
            }
            if (str.equals("byte")) {
                if (!(object instanceof Byte)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Byte: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Byte) object).toString());
            }
            if (str.equals("short")) {
                if (!(object instanceof Short)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Short: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Short) object).toString());
            }
            if (str.equals("int")) {
                if (!(object instanceof Integer)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Integer: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Integer) object).toString());
            }
            if (str.equals("long")) {
                if (!(object instanceof Long)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Long: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Long) object).toString());
            }
            if (str.equals("char")) {
                if (!(object instanceof Character)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Character: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Character) object).toString());
            }
            if (str.equals("float")) {
                if (!(object instanceof Float)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Float: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Float) object).toString());
            }
            if (str.equals("double")) {
                if (!(object instanceof Double)) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof Double: |" + object + "|");
                    throw new IllegalArgumentException(msg);
                }
                return String.format("'%s'", ((Double) object).toString());
            }
            String msg = AnsiText.fatalStr0(traceStr0 + ": unhandled/non-reachable case");
            throw new IllegalArgumentException(msg);
        }

        if (cls.getName().equals(JsonWalk.STRING)) {
////////
////////......
////////
            if (object == null)
                return Json.NULL;
            if (!(object instanceof String)) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": Not instanceof String: |" + object + "|");
                throw new IllegalArgumentException(msg);
            }
            return '\'' + Json.escapeString(((String) object)) + '\'';
        }

        if (!JsonWalk.isJsonable(cls)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Jsonable only: |" + cls + "|");
            throw new ClassNotFoundException(msg);
        }

////////
////////......
////////
        // as it is; no herited members, but private members included
        // See the page:
        //  http://docs.oracle.com/javase/tutorial/reflect/class/classMembers.html
        List<String> sa = new ArrayList<String>();
        for (Field field: cls.getDeclaredFields()) {
            String key = field.getName();
            if (JsonWalk.TRACE)
                println(indent + traceStr0 + ": Field: |" + key + "|");
            if (field.isSynthetic()) {
                // ignore: see the end of page:
                //  http://docs.oracle.com/javase/tutorial/reflect/member/fieldModifiers.html
                if (JsonWalk.TRACE)
                    println(indent + traceStr0 + "    isSynthetic");
                continue;
            }
            if (field.isEnumConstant()) {
                // ignore for simplicity; use other type
                if (JsonWalk.TRACE)
                    println(indent + traceStr0 + "    isEnumConstant");
                continue;
            }
            Type fieldType = field.getGenericType();
////////
////////......
////////
            String str = walkType(fieldType, field.get(object), nextIndent);
            sa.add(String.format("'%s':%s", key, str));
        }
        //return String.format("{%s}", sa.isEmpty() ? "" : "\n" + indent + "    " + Joint.join(sa, ",\n" + indent + "    ") + "\n" + indent);
        return String.format("{%s}", sa.isEmpty() ? "" : Joint.join(sa, ","));
    }
////////////////////////////////////////////////////////////////////////////////}


////////////////////////////////////////////////////////////////////////////////{
/*
 * given Json.BaseType set by Json.setJson with json string,
 * these fill the class that given json string represents
 */
////////////////////////////////////////////////////////////////////////////////
    private static Object setWalkParametrizedTypeJson(Type type, Json.BaseType jsonType, String indent)
    throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        final String traceStr0 = "setWalkParametrizedTypeJson";
        final String nextIndent = indent + JsonWalk.INDENT_PARAMETRIZEDTYPE;
        if (jsonType == null) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": No jsonType for |" + type.toString() + "|");
            throw new NullPointerException(msg);
        }
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + type + "|");
        if (!(type instanceof ParameterizedType)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not ParameterizedType: |" + type + "|");
            throw new IllegalArgumentException(msg);
        }
        ParameterizedType paraType = (ParameterizedType) type;
        String rawClassStr = ((Class) paraType.getRawType()).getName();
        if (rawClassStr.equals(JsonWalk.LIST)) {
            return JsonWalk.setWalkListJson(paraType, jsonType, nextIndent);
        } else if (rawClassStr.equals(JsonWalk.MAP)) {
            return JsonWalk.setWalkMapJson(paraType, jsonType, nextIndent);
        }
        String msg = AnsiText.fatalStr0(traceStr0 + ": rawClass: List or Map only: |" + rawClassStr + "|");
        throw new ClassNotFoundException(msg);
    }
    private static Object setWalkListJson(ParameterizedType paraType, Json.BaseType jsonType, String indent)
    throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        final String traceStr0 = "setWalkListJson";
        final String nextIndent = indent + JsonWalk.INDENT_LIST;
        if (jsonType == null) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": No jsonType for |" + paraType.toString() + "|");
            throw new NullPointerException(msg);
        }
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + paraType + "|");
        Type[] types = paraType.getActualTypeArguments();
        if (types.length != 1) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": # of types (" + types.length + ") <> 1");
            throw new IllegalArgumentException(msg);
        }
        Type type = types[0];
////////
////////......
////////
        if (jsonType.type == Json.Type.NULL) {
            return null;
        }
        if (jsonType.type != Json.Type.LIST) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.LIST: |" + jsonType.type + "|");
            throw new IllegalArgumentException(msg);
        }
        List<Object> oa = new ArrayList<Object>();
        for (Json.BaseType jsType: ((Json.ListType) jsonType).toList()) {
            Object obj = setWalkTypeJson(type, jsType, nextIndent);
            oa.add(obj);
        }
        return oa;
    }
    private static Object setWalkMapJson(ParameterizedType paraType, Json.BaseType jsonType, String indent)
    throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        final String traceStr0 = "setWalkMapJson";
        final String nextIndent = indent + JsonWalk.INDENT_MAP;
        if (jsonType == null) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": No jsonType for |" + paraType.toString() + "|");
            throw new NullPointerException(msg);
        }
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + paraType + "|");
        Type[] types = paraType.getActualTypeArguments();
        if (types.length != 2) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": # of types (" + types.length + ") <> 2");
            throw new IllegalArgumentException(msg);
        }
        Type keyType = types[0];
        if (!(keyType instanceof Class)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": keyClass: Not Class: |" + keyType + "|");
            throw new IllegalArgumentException(msg);
        }
        Class keyClass = (Class) keyType;
        if (!keyClass.getName().equals(JsonWalk.STRING)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": keyClass: " + JsonWalk.STRING + " only: |" + keyClass.getName() + "|");
            throw new ClassNotFoundException(msg);
        }
        Type valType = types[1];
////////
////////......
////////
        if (jsonType.type == Json.Type.NULL) {
            return null;
        }
        if (jsonType.type != Json.Type.MAP) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.MAP: |" + jsonType.type + "|");
            throw new IllegalArgumentException(msg);
        }
        Map<String, Json.BaseType> map = ((Json.MapType) jsonType).toMap();
        Map<String, Object> om = new HashMap<String, Object>();
        for (String key: map.keySet()) {
            Object obj = setWalkTypeJson(valType, map.get(key), nextIndent);
            om.put(key, obj);
        }
        return om;
    }
    private static Object setWalkTypeJson(Type type, Json.BaseType jsonType, String indent)
    throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        final String traceStr0 = "setWalkTypeJson";
        final String nextIndent = indent + JsonWalk.INDENT_TYPE;
        if (jsonType == null) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": No jsonType for |" + type.toString() + "|");
            throw new NullPointerException(msg);
        }
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + type + "|");
        if (type instanceof Class) {
            return setWalkClassJson((Class) type, jsonType, nextIndent);
        } else if (type instanceof ParameterizedType) {
            return setWalkParametrizedTypeJson((ParameterizedType) type, jsonType, nextIndent);
        }
        if (type instanceof GenericArrayType) {
            // find "raw" class
            GenericArrayType gaType = (GenericArrayType) type;
            Type t;
            while ((t = gaType.getGenericComponentType()) instanceof GenericArrayType)
                gaType = (GenericArrayType) t;
            Class c = (Class) ((ParameterizedType) t).getRawType();
            String rawClassStr = c.getName();
            if (!rawClassStr.equals(JsonWalk.MAP)) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": GenericArrayType: rawClass: Map only: |" + rawClassStr + "|");
                throw new ClassNotFoundException(msg);
            }
////////
////////......
////////
            if (jsonType.type == Json.Type.NULL) {
                return null;
            }
            if (jsonType.type != Json.Type.LIST) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.LIST: |" + jsonType.type + "|");
                throw new IllegalArgumentException(msg);
            }
            List<Json.BaseType> ja = ((Json.ListType) jsonType).toList();
            Object rObject = Array.newInstance(c, ja.size());
            for (int i = 0; i < ja.size(); ++i) {
                Object obj = JsonWalk.setWalkTypeJson(((GenericArrayType) type).getGenericComponentType(), ja.get(i), nextIndent);
                Array.set(rObject, i, obj);
            }
            return rObject;
        }
        String msg = AnsiText.fatalStr0(traceStr0 + ": Class or ParameterizedType only: |" + type.toString() + "|");
        throw new ClassNotFoundException(msg);
    }
    public static Object setWalkClassJson(Class cls, Json.BaseType jsonType, String indent)
    throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        final String traceStr0 = "setWalkClassJson";
        final String nextIndent = indent + JsonWalk.INDENT_CLASS;
        if (jsonType == null) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": No jsonType for |" + cls.getName() + "|");
            throw new NullPointerException(msg);
        }
        if (JsonWalk.TRACE)
            println(indent + traceStr0 + ": |" + cls + "|");
        TypeVariable[] tv = cls.getTypeParameters();
        if (tv.length > 0) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Non-generic class only: |" + cls + "|");
            throw new ClassNotFoundException(msg);
        }

        if (cls.isArray()) {
            Class c = cls.getComponentType();
////////
////////......
////////
            if (jsonType.type == Json.Type.NULL) {
                return null;
            }
            if (jsonType.type != Json.Type.LIST) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.LIST: |" + jsonType.type + "|");
                throw new IllegalArgumentException(msg);
            }
            List<Json.BaseType> ja = ((Json.ListType) jsonType).toList();
            Object rObject = Array.newInstance(c, ja.size());
            for (int i = 0; i < ja.size(); ++i) {
                Object obj = setWalkClassJson(c, ja.get(i), nextIndent);
                Array.set(rObject, i, obj);
            }
            return rObject;
        }

        if (cls.isPrimitive()) {
            // boolean, byte, short, int, long, char, float, and double
            String str = cls.getSimpleName();
////////
////////......
////////
            if (jsonType.type != Json.Type.STRING) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.STRING: |" + jsonType.type + "|");
                throw new IllegalArgumentException(msg);
            }
            String s = ((Json.StringType) jsonType).toString();
            if (str.equals("boolean")) {
                return new Boolean(s);
            }
            if (str.equals("byte")) {
                return new Byte(s);
            }
            if (str.equals("short")) {
                return new Short(s);
            }
            if (str.equals("int")) {
                return new Integer(s);
            }
            if (str.equals("long")) {
                return new Long(s);
            }
            if (str.equals("char")) {
                char[] chars = s.toCharArray();
                if (chars.length != 1) {
                    String msg = AnsiText.fatalStr0(traceStr0 + ": # of characters <> 1: |" + s + "|(" + chars.length + ")");
                    throw new IllegalArgumentException(msg);
                }
                if (chars.length == 0)
                    return new Character('\u0000'); // default: http://docs.oracle.com/javase/specs/jls/se7/html/jls-4.html#jls-4.12.5
                return new Character(chars[0]);
            }
            if (str.equals("float")) {
                return new Float(s);
            }
            if (str.equals("double")) {
                return new Double(s);
            }
            String msg = AnsiText.fatalStr0(traceStr0 + ": unhandled/non-reachable case");
            throw new IllegalArgumentException(msg);
        }

        if (cls.getName().equals(JsonWalk.STRING)) {
////////
////////......
////////
            if (jsonType.type == Json.Type.NULL)
                return null;
            if (jsonType.type != Json.Type.STRING) {
                String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.STRING: |" + jsonType.type + "|");
                throw new IllegalArgumentException(msg);
            }
            return new String(((Json.StringType) jsonType).toString());
        }

        if (!JsonWalk.isJsonable(cls)) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Jsonable only: |" + cls + "|");
            throw new ClassNotFoundException(msg);
        }

////////
////////......
////////
        if (jsonType.type != Json.Type.MAP) {
            String msg = AnsiText.fatalStr0(traceStr0 + ": Not Json.Type.MAP: |" + jsonType.type + "|");
            throw new IllegalArgumentException(msg);
        }
        Map<String, Json.BaseType> map = ((Json.MapType) jsonType).toMap();
        Object rObject = cls.newInstance();
        // as it is; no herited members, but private members included
        // See the page:
        //  http://docs.oracle.com/javase/tutorial/reflect/class/classMembers.html
        for (Field field: cls.getDeclaredFields()) {
            String key = field.getName();
            if (JsonWalk.TRACE)
                println(indent + traceStr0 + ": Field: |" + key + "|");
            if (field.isSynthetic()) {
                // ignore: see the end of page:
                //  http://docs.oracle.com/javase/tutorial/reflect/member/fieldModifiers.html
                if (JsonWalk.TRACE)
                    println(indent + traceStr0 + "    isSynthetic");
                continue;
            }
            if (field.isEnumConstant()) {
                // ignore for simplicity; use other type
                if (JsonWalk.TRACE)
                    println(indent + traceStr0 + "    isEnumConstant");
                continue;
            }
            Type fieldType = field.getGenericType();
////////
////////......
////////
            Object obj = setWalkTypeJson(fieldType, map.get(key), nextIndent);
            field.set(rObject, obj);
        }
        return rObject;
    }
////////////////////////////////////////////////////////////////////////////////}


    public static void main(String[] args)
    {
        // given class Result, get json string
        Result result = new Result();
        JsonWalk.TRACE = true;  // see what's going on
        String jsonStr = null;
        try {
            jsonStr = JsonWalk.walkClass(Result.class, result, "    ");
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        println(String.format("|%s|(%d)", jsonStr, jsonStr.length()));
        //jsonStr = "{'a':[],'boolean0':'false','byte0':'0','short0':'0','int0':'0','long0':'0','char0':' ','float0':'0.0','double0':'0.0','string0':'NULL','doc':[],'x':{},'y':'NULL'}";
        //jsonStr = "{'a':[],'boolean0':'false','byte0':'0','short0':'0','int0':'0','long0':'0','char0':' ','float0':'0.0','double0':'0.0','string0':NULL,'doc':[],'x':{},'y':NULL}";
        //jsonStr = "{'map0': {'a': NULL}, 'a':NULL,'boolean0':'false','byte0':'0','short0':'0','int0':'0','long0':'0','char0':' ','float0':'0.0','double0':'0.0','string0':NULL,'doc':[],'x':{},'y':[{'a': NULL}, NULL]}";
        // set Json.BaseType
        jsonStr = "{'map0': {'a': NULL, 'b': 'help', 'c': 'hello, world'}, 'a':NULL,'b':NULL,'sa0':NULL,'boolean0':'false','byte0':'0','short0':'0','int0':'0','long0':'0','char0':' ','float0':'0.0','double0':'0.0','string0':NULL,'doc':" + dick + ",'x':{},'y':[{'a': NULL}, NULL]}";
        Json.BaseType jsonType = Json.setJson(jsonStr, true, true);
        jsonType.print();   // see it
        Result r = null;
        try {
            r = (Result) JsonWalk.setWalkClassJson(Result.class, jsonType, "    ");
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (InstantiationException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        // is result equal to r?
        String jsStr = null;
        try {
            jsStr = JsonWalk.walkClass(Result.class, r, "    ");
        }
        catch (ClassNotFoundException e) {e.printStackTrace();}
        catch (IllegalAccessException e) {e.printStackTrace();}
        println(String.format("|%s|(%d)", jsStr, jsStr.length()));
        println(jsStr.equals(jsonStr));
        String s = "{'a':[],'boolean0':'false','byte0':'0','short0':'0','int0':'0','long0':'0','char0':'','float0':'0.0','double0':'0.0','string0':NULL,'doc':[],'x':{},'y':NULL}";
        println(String.format("|%s|(%d)", s, s.length()));
        println(jsonStr.equals(s));
    }
}
