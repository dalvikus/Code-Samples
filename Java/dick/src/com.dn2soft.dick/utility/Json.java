package com.dn2soft.dick.utility;

import com.dn2soft.dick.utility.Joint;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;

public class Json {
    final public static String NULL = "NULL";
    final private static String TRUE = "true";
    final private static String FALSE = "false";
    public static String  escapeString(String str)
    {
        return (str == null || str.isEmpty()) ? "" : str.replace("\\", "\\\\").replace("'", "\\\'");
//      return (str == null || str.isEmpty()) ? "" : str.replace("\"", "\\\"");
    }
/*
    enum Bracket {
        NONE,
        CURLY_BRACKET,
        BRACKET,
        PARENTHESIS
    }
 */
    public enum Type {
        NULL,
        STRING,
        LIST,
        TUPLE,
        MAP,
        BOOLEAN,
        DOUBLE,
        LONG,
    }
    public static class BaseType {
        static final int TABSIZE = 2;
        public Type    type;
        BaseType   parent;
        public BaseType() {type = Type.NULL;}
        public BaseType(Type t) {type = t;}

        String getMapStr(String indentStr)
        {
            int maxlen = -1;
            for (String key: ((MapType) this).map.keySet()) {
                int len = key.length();
                len += 2;   // "'...'"
                if (len > maxlen)
                    maxlen = len;
                //println(indentStr + key);
            }
            maxlen += 2;    // ": "
            //println(maxlen);
            List<String>    a = new ArrayList<String>();
            StringBuilder   sb = new StringBuilder();
            for (int i = 0; i < TABSIZE; ++i)
                sb.append(' ');
            String  tabStr = sb.toString();
            for (String key: ((MapType) this).map.keySet()) {
                String  keyStr = "'" + key + "': ";
                sb = new StringBuilder();
                for (int i = 0; i < maxlen - (key.length() + 2) - 2; ++i)
                    sb.append(' ');
                keyStr += sb.toString();
                //println("|" + keyStr + "|");
                sb = new StringBuilder();
                for (int i = 0; i < maxlen; ++i)
                    sb.append(' ');
                String  iStr = indentStr + tabStr + sb.toString();
                String  valStr;
                BaseType   val = ((MapType) this).map.get(key);
                switch (val.type) {
                case MAP:
                    valStr = val.getMapStr(iStr);
                    break;
                case LIST:
                case TUPLE:
                    valStr = val.getListStr(iStr, val.type == Type.TUPLE);
                    break;
                case NULL:
                    valStr = NULL;
                    break;
                default:    //case Type.STRING:
                    valStr = "'" + ((StringType) val).str + "'";
                    break;
                }
                a.add(tabStr + keyStr + valStr);
            }
            String  s = "";
            if (!a.isEmpty()) {
                s = indentStr + Joint.join(a, "\n" + indentStr) + "\n";
            }
            return "{" + "\n" + s + indentStr + "}";
        }
        String  getListStr(String indentStr, boolean isTuple)
        {
            List<String>    a = new ArrayList<String>();
            StringBuilder   sb = new StringBuilder();
            for (int i = 0; i < TABSIZE; ++i)
                sb.append(' ');
            String  tabStr = sb.toString();
            String  iStr = indentStr + tabStr;
            for (BaseType elm: ((ListType) this).list) {
                String  valStr;
                switch (elm.type) {
                case MAP:
                    valStr = elm.getMapStr(iStr);
                    break;
                case LIST:
                case TUPLE:
                    valStr = elm.getListStr(iStr, elm.type == Type.TUPLE);
                    break;
                case NULL:
                    valStr = NULL;
                    break;
                default:    //case Type.STRING:
                    valStr = "'" + ((StringType) elm).str + "'";
                    break;
                }
                a.add(valStr);
            }
            String  s = "";
            if (!a.isEmpty()) {
                s = iStr + Joint.join(a, "\n" + iStr) + "\n";
            }
            String  left = isTuple ? "(" : "[";
            String  right = isTuple ? ")" : "]";
            return left + "\n" + s + indentStr + right;
        }
        public void print(int level, String indentStr0) {
            StringBuilder   sb = new StringBuilder();
            for (int i = 0; i < TABSIZE; ++i)
                sb.append(' ');
            String  tabStr = sb.toString();
            sb = new StringBuilder();
            for (int i = 0; i < level; ++i)
                sb.append(tabStr);
            String  indentStr = indentStr0 + sb.toString();
            switch (type) {
            case MAP:
                System.out.println("MAP\n" + indentStr + getMapStr(indentStr));
                break;
            case LIST:
            case TUPLE:
                System.out.println((type == Type.TUPLE ? "TUPLE" : "LIST") + "\n" + indentStr + getListStr(indentStr, type == Type.TUPLE));
                break;
            case NULL:
                System.out.println("NULL\n" + indentStr + "null");
                break;
            default:    //case Type.STRING:
                System.out.println("STRING\n" + indentStr + "'" + ((StringType) this).str + "'");
                break;
            }
        }
        public void print() {
            print(0, "");
        }
        public void print(int level) {
            print(level, "");
        }
        public void print(String indentStr0) {
            print(0, indentStr0);
        }
/*
        public static Object setObject(BaseType js) {
            Object o = null;
            switch (js.type) {
            case STRING:
                o = ((StringType) js).str;
                break;
            case LIST:
            case TUPLE:
                List<Object> oa = new ArrayList<Object>();
                for (BaseType t: ((ListType) js).list)
                    oa.add(BaseType.setObject(t));
                o = oa;
                break;
            case MAP:
                Map<String, Object> om = new HashMap<String, Object>();
                for (Map.Entry<String, BaseType> entry: ((MapType) js).map.entrySet())
                    om.put(entry.getKey(), BaseType.setObject(entry.getValue()));
                o = om;
                break;
            default:
                o = null;
                break;
            }
            return o;
        }
 */
    }
    public static class StringType extends BaseType {
        private String str;
        public StringType(String s)
        {
            super(Type.STRING);
            str = s;
        }
        public String toString() {return str;}
    }
    public static class BooleanType extends BaseType {
        private boolean bl;
        public BooleanType(boolean b)
        {
            super(Type.BOOLEAN);
            bl = b;
        }
        public boolean booleanValue() {return bl;}
    }
    public static class DoubleType extends BaseType {
        private double dbl;
        public DoubleType(double d)
        {
            super(Type.DOUBLE);
            dbl = d;
        }
        public double doubleValue() {return dbl;}
    }
    public static class LongType extends BaseType {
        private long lng;
        public LongType(long l)
        {
            super(Type.LONG);
            lng = l;
        }
        public long longValue() {return lng;}
    }
    public static class ListType extends BaseType {
        public ListType(char c)
        {
            super(c == '[' ? Type.LIST : Type.TUPLE);
        }
        private List<BaseType> list = new ArrayList<BaseType>();
        public List<BaseType> toList() {return list;}
    }
    public static class MapType extends BaseType {
        public MapType()
        {
            super(Type.MAP);
        }
        private Map<String, BaseType> map = new HashMap<String, BaseType>();
        public Map<String, BaseType> toMap() {return map;}
    }

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
    private static String fatalStr(String message, String upToString) {
        return "FATAL: " + message + (upToString == null ? "" : "\n" + upToString);
    }
    private static String fatalStr(String message) {
        return fatalStr(message, null);
    }
    public static BaseType setJson(String str) {
        return setJson(str, false, false);
    }
    public static BaseType setJson(String str, boolean verbose) {
        return setJson(str, verbose, false);
    }
    // To enforce assertion is enabled: from
    //  http://docs.oracle.com/javase/7/docs/technotes/guides/language/assert.html
/*
    static {
        boolean assertsEnabled = false;
        assert assertsEnabled = true;   // Intentional side effect!!!
        if (!assertsEnabled)
            throw new RuntimeException("Asserts must be enabled!!!");
    }
 */
    private static BaseType setUnquotedJson(String unquotedStr, boolean verbose)
    {
        if (verbose)
            println("unquoted str: |" + unquotedStr + "|");
        if (unquotedStr.equals(NULL))
            return new BaseType();
        if (unquotedStr.equals(TRUE))
            return new BooleanType(true);
        if (unquotedStr.equals(FALSE))
            return new BooleanType(false);
        // try float first
        Double dbl = null;
        try {
            dbl = new Double(unquotedStr);
        } catch (NumberFormatException e) {}
        if (dbl != null)
            return new DoubleType(dbl.doubleValue());
        Long lng = null;
        try {
            lng = new Long(unquotedStr);
        } catch (NumberFormatException e) {}
        return lng == null ? null : new LongType(lng.longValue());
    }
    public static BaseType setJson(String str, boolean verbose, boolean trace) {
        if (trace)
            verbose = true;
        if (verbose)
            println("|" + str + "|");
        char[]  chars = str.toCharArray();
        // collect all characters up to now; for debugging info
        StringBuilder   upToToken = new StringBuilder();
        // some inits
        String      key = null;
        BaseType    rootType = null;
        BaseType    currentType = null;
        // check if it is in quoted string
        boolean     inStringToken = false;      // tell if it is in a quoted string
        char        beginStringChar = '\u0000'; // tells if a string is quoted by  '"' or '\''; '\u0000' means not-in-quote state
                                                //  http://docs.oracle.com/javase/specs/jls/se7/html/jls-4.html#jls-4.12.5
        // for quoated string
        StringBuilder   stringToken = new StringBuilder();
        String          lastString = null;
        // for unquoated(non-string) chars
        StringBuilder   lastToken = new StringBuilder();    // space(' ', '\n', '\t') delimits string
        String          unquotedStr = null;
        for (int i = 0; i < chars.length; ++i) {
            char    ch = chars[i];
            upToToken.append(ch);
            switch (ch) {
            case '\\':
                assert inStringToken : fatalStr("Token '\\': Only in the string", upToToken.toString());
                assert (i + 1) < chars.length : fatalStr("Token '\\': No more tokens", upToToken.toString());
                char c = chars[++i];
                assert c == '\\' || c == '"' || c == '\'' : fatalStr("Token '\\': Next token(" + c + "): Invalid: '\"','\\','\\\'' only", upToToken.toString());
                stringToken.append(c);
                break;
            case ':':
                if (inStringToken) {
                    stringToken.append(ch);
                    break;
                }
                assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                assert lastToken.toString().isEmpty() : "unhandled |" + lastToken.toString() + "|";
                assert currentType.type == Type.MAP : fatalStr("Token ':': Current bracket: " + Type.MAP + ": not a '{'", upToToken.toString());
                assert stringToken.toString().isEmpty() : fatalStr("Token ':': Unhandled string: |" + stringToken.toString() + "|", upToToken.toString());
                assert !lastString.isEmpty() : fatalStr("Token ':': No key", upToToken.toString());
                assert key == null : fatalStr("unhandled key: |" + key + "|");
                key = lastString;
                lastString = null;
                stringToken = new StringBuilder();
                break;
            case ' ':
            case '\n':
            case '\t':
                if (inStringToken) {
                    stringToken.append(ch);
                    break;
                }
                if (lastToken.length() > 0) {
                    assert unquotedStr == null : fatalStr("unhandled str: |" + unquotedStr + "|");
                    unquotedStr = lastToken.toString();
                    ////assert unquotedStr.equals(NULL) : "unhandled str: |" + unquotedStr + "|: \"" + NULL + "\" only";
                    BaseType t = setUnquotedJson(unquotedStr, verbose);
                    assert t != null : fatalStr("unhandled unquoted str: |" + unquotedStr + "|");
                    lastToken.setLength(0);
                }
                break;
            case '"':
            case '\'':
                if (inStringToken) {
                    if (beginStringChar == ch) {
                        if (trace)
                            printf("TRACE: new string: |%s|\n", stringToken.toString());
                        lastString = stringToken.toString();
                        stringToken = new StringBuilder();
                        inStringToken = false;
                    } else {
                        stringToken.append(ch);
                    }
                } else {
                    assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                    assert lastToken.toString().isEmpty() : "unhandled |" + lastToken.toString() + "|";
                    assert lastString == null : fatalStr("unhandled lastString: |" + lastString + "|");
                    inStringToken = true;
                    beginStringChar = ch;
                    assert stringToken.toString().isEmpty() : fatalStr("Token '" + ch + "': Unhandled string: |" + stringToken.toString() + "|", upToToken.toString());
                    stringToken = new StringBuilder();
                }
                break;
            case '{':
            case '[':
            case '(':
                if (inStringToken) {
                    stringToken.append(ch);
                    break;
                }
                assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                assert lastToken.toString().isEmpty() : "unhandled |" + lastToken.toString() + "|";
                if (rootType == null) {
                    assert currentType == null : fatalStr("Token '" + ch + "': rootType is null but currentType is NOT null", upToToken.toString());
                } else {
                    assert currentType != null : fatalStr("Token '" + ch + "': currentType is null", upToToken.toString());
                }
                if (currentType != null) {
                    assert currentType.type != Type.STRING : fatalStr("Token '" + ch + "': currentType.type is Type.STRING", upToToken.toString());
                    if (currentType.type == Type.MAP) {
                        assert key != null : fatalStr("Token '" + ch + "': No key", upToToken.toString());
                    }
                }
                BaseType   t0 = ch == '{' ? new MapType() : new ListType(ch);
                t0.parent = currentType;
                if (rootType == null) {
                    rootType = t0;
                } else {
                    if (currentType.type == Type.MAP) {
                        assert key != null : fatalStr("Token '" + ch + "': No key", upToToken.toString());
                        ((MapType) currentType).map.put(key, t0);
                        key = null;
                    } else {
                        ((ListType) currentType).list.add(t0);
                    }
                }
                currentType = t0;

                assert stringToken.toString().isEmpty() : fatalStr("Token '" + ch + "': Unhandled string: |" + stringToken.toString() + "|", upToToken.toString());
                stringToken = new StringBuilder();
                break;
            case '}':
            case ']':
            case ')':
            // in case of '}])', proceed as if ',' is encountered
            // this is why last ',' is valid in the definition of array(list or map) in other languages(python, perl, php, etc.): e.g., a = [1,2,3,]
            case ',':
                if (inStringToken) {
                    stringToken.append(ch);
                    break;
                }
                // like ','
                assert currentType.type != null : fatalStr("Token ',': Not in a bracket", upToToken.toString());
                if (!lastToken.toString().isEmpty()) {
                    assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                    unquotedStr = lastToken.toString(); lastToken.setLength(0);
                }
                assert stringToken.toString().isEmpty() : fatalStr("Unhandled, incomplete string: |" + stringToken.toString() + "|", upToToken.toString());
                String  value = null;
                if (lastString != null) {
                    value = lastString;
                    lastString = null;
                    assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                }
                if (currentType.type == Type.MAP) {
                    if (key == null && unquotedStr != null) {
                        assert key != null : fatalStr("Token ',': No key but unhandled str: |" + unquotedStr + "|", upToToken.toString());
                    }
                    if (ch == ',') {
                        if (((MapType) currentType).map.isEmpty()) {
                            assert key != null : fatalStr("Token ',': No key", upToToken.toString());
                        } else {
                            // see below
                        }
                    }
                    if (key != null) {
                        BaseType t1 = new BaseType();
                        if (value == null) {
                            if (unquotedStr == null) {
                                assert value != null : fatalStr("Token '" + ch + "': No value", upToToken.toString());
                            } else {
                                ////assert unquotedStr.equals(NULL) : "unhandled str: |" + unquotedStr + "|: \"" + NULL + "\" only";
                                if (unquotedStr.equals(NULL))
                                    t1 = new BaseType();
                                else
                                    t1 = new StringType(unquotedStr);
                                unquotedStr = null;
                            }
                        } else {
                            t1 = new StringType(value);
                        }
                        if (verbose)
                            System.err.printf("INFO: Key: '%s', Val: %s\n", key, t1.type == Type.NULL ? "NULL" : "'" + value + "'");
                        ((MapType) currentType).map.put(key, t1);
                        key = null;
                    }
                } else {
                    if (value != null && unquotedStr != null) {
                        assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                    }
                    if (unquotedStr != null) {
                        ////assert unquotedStr.equals(NULL) : "unhandled str: |" + unquotedStr + "|: \"" + NULL + "\" only";
                    }
                    if (unquotedStr == null && value == null) {
                        if (ch == ',' && ((ListType) currentType).list.isEmpty()) {
                            assert value != null : fatalStr("Token ',': No value", upToToken.toString());
                        }
                    } else {
                        BaseType t1;
                        if (value == null) {    // value == null, unquotedStr != null
                            ////
                            unquotedStr = null;
                            t1 = new BaseType();
                        } else {    // value != null, unquotedStr = null
                            t1 = new StringType(value);
                        }
                        ((ListType) currentType).list.add(t1);
                        if (verbose)
                            System.err.printf("INFO: Element: %s\n", value == null ? NULL : "'" + value + "'");
                    }
                }
                if (ch == ',')  // ',' itself
                    break;

                if (ch == '}') {
                    assert currentType.type == Type.MAP : fatalStr("Token '" + ch + "': no match '}'" + stringToken.toString() + "", upToToken.toString());
                } else if (ch == ']') {
                    assert currentType.type == Type.LIST : fatalStr("Token '" + ch + "': no match ']'" + stringToken.toString() + "", upToToken.toString());
                } else {    // ')'
                    assert currentType.type == Type.TUPLE : fatalStr("Token '" + ch + "': no match ')'" + stringToken.toString() + "", upToToken.toString());
                }
                currentType = currentType.parent;
                assert stringToken.toString().isEmpty() : fatalStr("Token '" + ch + "': Unhandled string: |" + stringToken.toString() + "|", upToToken.toString());
                stringToken = new StringBuilder();
                break;
            default:
                if (inStringToken) {
                    stringToken.append(ch);
                    break;
                }
                assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
                lastToken.append(ch);
                break;
            }
        }
        assert currentType == null : fatalStr("Unmatched bracket", upToToken.toString());
        if (!lastToken.toString().isEmpty()) {
            assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
            unquotedStr = lastToken.toString(); lastToken.setLength(0);
        }
        assert stringToken.toString().isEmpty() : fatalStr("Unhandled, incomplete string: |" + stringToken.toString() + "|", upToToken.toString());
        assert key == null : fatalStr("Unhandled key: |" + key + "|", upToToken.toString());
        if (lastString != null) {
            assert rootType == null : fatalStr("lastString: |" + lastString + "|, but rootType is NOT null", upToToken.toString());
            assert currentType == null : fatalStr("lastString: |" + lastString + "|, but currentType is NOT null", upToToken.toString());
            assert unquotedStr == null : "unhandled str: |" + unquotedStr + "|";
            rootType = new StringType(lastString);
        } else if (unquotedStr != null) {
            assert rootType == null : fatalStr("unhandled str: |" + unquotedStr + "|, but rootType is NOT null", upToToken.toString());
            assert currentType == null : fatalStr("unhandled str: |" + unquotedStr + "|, but currentType is NOT null", upToToken.toString());
            ////
            assert unquotedStr.equals(NULL) : "unhandled str: |" + unquotedStr + "|: \"" + NULL + "\" only";
            unquotedStr = null;
            rootType = new BaseType();
        }
        return rootType;
    }
    public static void main(String[] args) {
/*
        String  str = "{'doc': [[{'body': [{'example': ['The word \"environment\" means different things to different people.', 'She spoke so fast I couldn\0xe2\0x80\0x99t understand a word (= anything she said).'], 'def': 'a single unit of language that has meaning and can be spoken or written:'}], 'head': {'gw': '', 'pron': ['', '', 'http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/word_/word.mp3', 'http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/word_/word.ogg'], 'ipa': ['w\0xc9\0x9crd'], 'word': 'word', 'pos': ['n']}}, {'body': [{'example': ['a strongly worded letter'], 'def': ''}], 'head': {'gw': '', 'pron': ['', '', 'http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/worde/worded.mp3', 'http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/worde/worded.ogg'], 'ipa': ['\0xcb\0x88w\0xc9\0x9crd\0xc2\0xb7\0xc9\0x99d'], 'word': 'worded', 'pos': ['adj']}}]], 'more': [{'gw': 'BRIEF STATEMENT', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_2', 'word': 'word', 'pos': 'n'}, {'gw': 'NEWS', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_3', 'word': 'word', 'pos': 'n'}, {'gw': 'PROMISE', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_4', 'word': 'word', 'pos': 'n'}, {'gw': 'ORDER', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_5', 'word': 'word', 'pos': 'n'}, {'gw': 'LANGUAGE UNIT', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_6', 'word': 'word', 'pos': 'v'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/code-word', 'word': 'code word', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/swear-word', 'word': 'swear word', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-processing', 'word': 'word processing', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-processor', 'word': 'word processor', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/four-letter-word', 'word': 'four-letter word', 'pos': 'n'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word', 'word': 'breathe a word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word', 'word': 'man of his word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/the-word-is', 'word': 'the word is', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-for-word', 'word': 'word for word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-of-mouth', 'word': 'word of mouth', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word', 'word': 'say a word, at breathe a word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word', 'word': 'woman of her word, at man of his word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/a-household-name', 'word': 'a household word, at a household name', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/true-to-your-word', 'word': 'true to your word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/get-a-word-in-edgewise', 'word': 'get a word in edgewise', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something', 'word': 'have the final word on something, at have the last word on something', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/the-last-word-in-something', 'word': 'the last word in something', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/take-someone-s-word-for-it', 'word': 'take someone\0xe2\0x80\0x99s word for it', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something', 'word': 'have the last word on something', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone', 'word': 'put in a good word for someone', 'pos': 'idiom'}]}";
        System.out.println(str);
 */
        String  str = "{'doc': [[{'body': [{'example': ['The word \"environment\" means different things to different people.', 'She spoke so fast I couldn\0xe2\0x80\0x99t understand a word (= anything she said).'], 'def': 'a single unit of language that has meaning and can be spoken or written:'}], 'head': {'gw': '', 'pron': ['', '', 'http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/word_/word.mp3', 'http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/word_/word.ogg'], 'ipa': ['w\0xc9\0x9crd'], 'word': 'word', 'pos': ['n']}}, {'body': [{'example': ['a strongly worded letter'], 'def': ''}], 'head': {'gw': '', 'pron': ['', '', 'http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/worde/worded.mp3', 'http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/worde/worded.ogg'], 'ipa': ['\0xcb\0x88w\0xc9\0x9crd\0xc2\0xb7\0xc9\0x99d'], 'word': 'worded', 'pos': ['adj']}}]], 'more': [{'gw': 'BRIEF STATEMENT', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_2', 'word': 'word', 'pos': 'n'}, {'gw': 'NEWS', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_3', 'word': 'word', 'pos': 'n'}, {'gw': 'PROMISE', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_4', 'word': 'word', 'pos': 'n'}, {'gw': 'ORDER', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_5', 'word': 'word', 'pos': 'n'}, {'gw': 'LANGUAGE UNIT', 'arl': 1, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_6', 'word': 'word', 'pos': 'v'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/code-word', 'word': 'code word', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/swear-word', 'word': 'swear word', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-processing', 'word': 'word processing', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-processor', 'word': 'word processor', 'pos': 'n'}, {'gw': '', 'arl': 2, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/four-letter-word', 'word': 'four-letter word', 'pos': 'n'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word', 'word': 'breathe a word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word', 'word': 'man of his word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/the-word-is', 'word': 'the word is', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-for-word', 'word': 'word for word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-of-mouth', 'word': 'word of mouth', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word', 'word': 'say a word, at breathe a word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word', 'word': 'woman of her word, at man of his word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/a-household-name', 'word': 'a household word, at a household name', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/true-to-your-word', 'word': 'true to your word', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/get-a-word-in-edgewise', 'word': 'get a word in edgewise', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something', 'word': 'have the final word on something, at have the last word on something', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/the-last-word-in-something', 'word': 'the last word in something', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/take-someone-s-word-for-it', 'word': 'take someone\0xe2\0x80\0x99s word for it', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something', 'word': 'have the last word on something', 'pos': 'idiom'}, {'gw': '', 'arl': 7, 'href': 'http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone', 'word': 'put in a good word for someone', 'pos': 'idiom'}]}";
        str = "{'A': 3, 'doc': [3, [{'head': {'word': 'word', 'gw': '', 'pos': ['n'], 'pron': ['', '', 'http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/word_/word.mp3', 'http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/word_/word.ogg'], 'ipa': ['wɜrd']}, 'body': [{'def': 'a single unit of language that has meaning and can be spoken or written:', 'example': ['The word \"environment\" means different things to different people.', 'She spoke so fast I couldn’t understand a word (= anything she said).']}]}, {'head': {'word': 'worded', 'gw': '', 'pos': ['adj'], 'pron': ['', '', 'http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/worde/worded.mp3', 'http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/worde/worded.ogg'], 'ipa': ['ˈwɜrd·əd']}, 'body': [{'def': '', 'example': ['a strongly worded letter']}]}]], 'more': [{'word': 'word', 'pos': 'n', 'gw': 'BRIEF STATEMENT', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_2', 'arl': 1}, {'word': 'word', 'pos': 'n', 'gw': 'NEWS', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_3', 'arl': 1}, {'word': 'word', 'pos': 'n', 'gw': 'PROMISE', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_4', 'arl': 1}, {'word': 'word', 'pos': 'n', 'gw': 'ORDER', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_5', 'arl': 1}, {'word': 'word', 'pos': 'v', 'gw': 'LANGUAGE UNIT', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word_6', 'arl': 1}, {'word': 'code word', 'pos': 'n', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/code-word', 'arl': 2}, {'word': 'swear word', 'pos': 'n', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/swear-word', 'arl': 2}, {'word': 'word processing', 'pos': 'n', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-processing', 'arl': 2}, {'word': 'word processor', 'pos': 'n', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-processor', 'arl': 2}, {'word': 'four-letter word', 'pos': 'n', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/four-letter-word', 'arl': 2}, {'word': 'breathe a word', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word', 'arl': 7}, {'word': 'man of his word', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word', 'arl': 7}, {'word': 'the word is', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/the-word-is', 'arl': 7}, {'word': 'word for word', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-for-word', 'arl': 7}, {'word': 'word of mouth', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/word-of-mouth', 'arl': 7}, {'word': 'say a word, at breathe a word', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word', 'arl': 7}, {'word': 'woman of her word, at man of his word', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word', 'arl': 7}, {'word': 'a household word, at a household name', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/a-household-name', 'arl': 7}, {'word': 'true to your word', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/true-to-your-word', 'arl': 7}, {'word': 'get a word in edgewise', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/get-a-word-in-edgewise', 'arl': 7}, {'word': 'have the final word on something, at have the last word on something', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something', 'arl': 7}, {'word': 'the last word in something', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/the-last-word-in-something', 'arl': 7}, {'word': 'take someone’s word for it', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/take-someone-s-word-for-it', 'arl': 7}, {'word': 'have the last word on something', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something', 'arl': 7}, {'word': 'put in a good word for someone', 'pos': 'idiom', 'gw': '', 'href': 'http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone', 'arl': 7}]}";
        //str = "'hello, world'";
        str = "(1, {'a': 'A', 'x': [1, 2, 3, []]})";
        println(str);
        BaseType   json = setJson(str);
        if (json != null)
            json.print(1, "....");
    }
}
