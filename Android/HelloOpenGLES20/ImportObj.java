import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.FileNotFoundException;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

import java.io.ObjectOutputStream;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.util.zip.GZIPOutputStream;
import java.io.ObjectInputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.util.zip.GZIPInputStream;

class ImportObj
{
    static class Group
    {
        public String name;
        public Map<String, ArrayInterface> map;
        Group(String name1, Map<String, ArrayInterface> map1)
        {
            name = name1;
            map = map1;
        }
    }
    static interface ArrayInterface
    {
    }
    static class VertexArray extends ArrayList<List<StringValue>> implements ArrayInterface
    {
        VertexArray(ArrayList<List<StringValue>> svll)
        {
            super(svll);
        }
        VertexArray()
        {
        }
    }
    static class IndexArray extends ArrayList<List<int[]>> implements ArrayInterface
    {
        IndexArray(ArrayList<List<int[]>> iall)
        {
            super(iall);
        }
        IndexArray()
        {
        }
    }
    private static List<Group> parse(String objFile)
    {
        List<Group> gl = new ArrayList<Group>();
        try {
            BufferedReader br = new BufferedReader(new FileReader(objFile));
            int nLine = 0;
            String group = null;
            boolean incomplete = false;
            String line = null;
            VertexArray v_a = new VertexArray();
            VertexArray vt_a = new VertexArray();
            VertexArray vn_a = new VertexArray();
            IndexArray f_a = new IndexArray();
            while (true) {
                line = br.readLine();
                if (line == null)
                    break;
                else
                    ++nLine;

                line = line.trim();
//              System.out.printf("line: |%s|%n", line);
                if (line.isEmpty())
                    continue;
                String[] sa = line.split("\\s+");
//              for (String s: sa) {
//                  System.out.printf("s: |%s|%n", s);
//              }
                String c = sa[0];   // command
                if (c.startsWith("#"))  // comment
                    continue;
                incomplete = sa[sa.length - 1].equals("\\");
                if (incomplete) {
                    assert c.equals("f") || c.equals("v") || c.equals("vt") || c.equals("vn") : String.format("|%s|: command: |%s|: incomplete: valid commands for incomplete are |f|, |v|, |vt|, |vn|", line, c);
                }
                if (c.equals("g")) {
                    if (group != null) {
                        Map<String, ArrayInterface> map = new HashMap<String, ArrayInterface>();
                        map.put("v", new VertexArray(v_a));
                        map.put("vt", new VertexArray(vt_a));
                        map.put("vn", new VertexArray(vn_a));
                        map.put("f", new IndexArray(f_a));
                        gl.add(new Group(group, map));
                        group = null;
                    }
                    group = sa[1];
                    v_a.clear();
                    vt_a.clear();
                    vn_a.clear();
                    f_a.clear();
                    continue;
                }
                assert c.equals("f") || c.equals("v") || c.equals("vt") || c.equals("vn") : String.format("|%s|: command: |%s|: invalid commands", line, c);
                List<int[]> ial = null;
                List<StringValue> svl = null;
                if (c.equals("f")) {
                    ial = ImportObj.getIndices(sa, 1, sa.length - (incomplete ? 2 : 1));
                    assert ial != null && (ial.size() == 3 || ial.size() == 4) : String.format("invalid |f| command: |%s|", line);
                } else {
                    svl = ImportObj.getNumbers(sa, 1, sa.length - (incomplete ? 2 : 1));
                    assert svl != null : String.format("|%s|: not all numbers", line);
                }
                while (incomplete) {
                    line = br.readLine(); ++nLine;
                    assert line != null : String.format("FATAL: |%s|: file ends with incomplete", line);
                    // trim
                    line = line.trim();
                    if (line.isEmpty())
                        continue;
                    sa = line.split("\\s+");
                    if (sa[0].startsWith("#"))  // comment
                        continue;
                    incomplete = sa[sa.length - 1].equals("\\");
                    if (c.equals("f")) {
                        List<int[]> ial1 = ImportObj.getIndices(sa, 0, sa.length - (incomplete ? 2 : 1));
                        assert ial1 != null : String.format("invalid |f| command: |%s|", line);
                        for (int[] ia: ial1)
                            ial.add(ia);
                    } else {
                        List<StringValue> svl1 = ImportObj.getNumbers(sa, 0, sa.length - (incomplete ? 2 : 1));
                        assert svl1 != null : String.format("|%s|: not all numbers", line);
                        for (StringValue sv: svl1)
                            svl.add(sv);
                    }
                }
                if (c.equals("f")) {
                    assert ial != null && (ial.size() == 3 || ial.size() == 4) : String.format("|%s|: invalid |f| command", line);
                    f_a.add(ial);
                    continue;
                }
                assert svl != null : String.format("|%s|: invalid |%s| command", line, c);
                if (c.equals("v") || c.equals("vn"))
                    assert svl.size() == 3 : String.format("|%s|: not exact three values for |v| or |vn|", line);
                else    // "vt"
                    assert svl.size() == 2 : String.format("|%s|: not exact two values for |vt|", line);
                if (c.equals("v")) {
                    v_a.add(svl);
                } else if (c.equals("vt")) {
                    vt_a.add(svl);
                } else {    // "vn"
                    vn_a.add(svl);
                }
            }
            if (group != null) {
                Map<String, ArrayInterface> map = new HashMap<String, ArrayInterface>();
                map.put("v", new VertexArray(v_a));
                map.put("vt", new VertexArray(vt_a));
                map.put("vn", new VertexArray(vn_a));
                map.put("f", new IndexArray(f_a));
                gl.add(new Group(group, map));
                group = null;
                v_a.clear();
                vt_a.clear();
                vn_a.clear();
                f_a.clear();
            }

            br.close();
            System.out.println("# of lines = " + nLine);
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
        return gl;
    }
    private static List<int[]> getIndices(String[] sa, int startIndex, int endIndex)
    {
        if (sa == null)
            return null;
        List<int[]> ial = new ArrayList<int[]>();
        for (int i = startIndex; i <= endIndex; ++i) {
            // valid formats:
            //  vi   face index only
            //  v1/vt1  face and texture indices
            //  v1/vt1/vn1  face, texture, normal indices
            //  v1//vn1     face, normal indices
            int[] ia = {0, 0, 0};
            String[] a = sa[i].split("/");
            for (int j = 0; j < a.length; ++j) {
                if (j == 1 && a[j].isEmpty()) {
                    ia[1] = 0;  // 0 means None
                    continue;
                }
                try {
                    int idx = Integer.parseInt(a[j]);
                    assert idx > 0 : "positive index only";
                    ia[j] = idx;
                } catch (NumberFormatException e) {
                    System.err.println(e.getMessage());
                    return null;
                }
            }
            ial.add(ia);
        }
        return ial;
    }
    static class StringValue
    {
        public String s;   // for debugging
        public float f;
        StringValue(String s1, float f1)
        {
            s = s1;
            f = f1;
        }
    }
    private static List<StringValue> getNumbers(String[] sa, int startIndex, int endIndex)
    {
        if (sa == null)
            return null;
        if (startIndex < 0) {
            System.err.printf("%s: index = %d: out of range%n", startIndex);
            return null;
        }
        if (endIndex >= sa.length) {
            System.err.printf("%s: index = %d: out of range%n", startIndex);
            return null;
        }

        List<StringValue> svl = new ArrayList<StringValue>();
        for (int i = startIndex; i <= endIndex; ++i) {
            try {
                float f = Float.parseFloat(sa[i]);
                svl.add(new StringValue(sa[i], f));
            } catch (NumberFormatException e) {
                System.err.println(e.getMessage());
                return null;
            }
        }
        return svl;
    }
    static private void checkIndices(List<Group> gl)
    {
        int nva = 0;
        int nvta = 0;
        int nvna = 0;
        for (Group g: gl) {
            Map<String, ArrayInterface> map = g.map;
            VertexArray v_a = (VertexArray) map.get("v");
            VertexArray vt_a = (VertexArray) map.get("vt");
            VertexArray vn_a = (VertexArray) map.get("vn");
            nva += v_a.size();
            nvta += vt_a.size();
            nvna += vn_a.size();
        }
        int f3 = 0;
        int f4 = 0;
        for (Group g: gl) {
            Map<String, ArrayInterface> map = g.map;
            IndexArray f_a = (IndexArray) map.get("f");
            for (List<int[]> ial: f_a) {
                if (ial.size() == 3)
                    ++f3;
                else
                    ++f4;
                for (int[] ia: ial) {
                    int iv = ia[0];
                    int ivt = ia[1];
                    int ivn = ia[2];
                    assert iv > 0 && iv <= nva : String.format("iv: %d (nva: %d)", iv, nva);
                    assert ivt >= 0 && ivt <= nvta : String.format("ivt: %d (nvta: %d)", ivt, nvta);
                    assert ivn >= 0 && ivn <= nvna : String.format("ivn: %d (nvna: %d)", ivn, nvna);
                }
            }
        }
        System.out.printf("# of triangles = %d, # of quadrilateral = %d%n", f3, f4);
    }
    private static void rebuildObj(List<Group> gl, boolean useFloat)
    {
        for (Group g: gl) {
            Map<String, ArrayInterface> map = g.map;
            System.out.printf("g %s%n", g.name);
            for (List<StringValue> svl: (VertexArray) map.get("v")) {
                StringValue sv1 = svl.get(0);
                StringValue sv2 = svl.get(1);
                StringValue sv3 = svl.get(2);
                if (useFloat)
                    System.out.printf("v %f %f %f%n", sv1.f, sv2.f, sv3.f);
                else
                    System.out.printf("v %s %s %s%n", sv1.s, sv2.s, sv3.s);
            }
            for (List<StringValue> svl: (VertexArray) map.get("vt")) {
                StringValue svt1 = svl.get(0);
                StringValue svt2 = svl.get(1);
                System.out.printf("vt %s %s%n", svt1.s, svt2.s);
            }
            for (List<StringValue> svl: (VertexArray) map.get("vn")) {
                StringValue svn1 = svl.get(0);
                StringValue svn2 = svl.get(1);
                StringValue svn3 = svl.get(2);
                System.out.printf("vn %s %s %s%n", svn1.s, svn2.s, svn3.s);
            }
            for (List<int[]> ial: (IndexArray) map.get("f")) {
                System.out.printf("f");
                for (int[] ia: ial) {
                    System.out.printf(" ");
                    System.out.printf("%d", ia[0]);
                    if (ia[1] == 0 && ia[2] == 0)
                        continue;
                    if (ia[1] == 0 && ia[2] != 0)
                        System.out.printf("//%d", ia[2]);
                    else if (ia[1] != 0 && ia[2] == 0)
                        System.out.printf("/%d", ia[1]);
                    else
                        System.out.printf("/%d/%d", ia[1], ia[2]);
                }
                System.out.println();
            }
            VertexArray vt_a = (VertexArray) map.get("vt");
            VertexArray vn_a = (VertexArray) map.get("vn");
            IndexArray f_a = (IndexArray) map.get("f");
        }
    }
    static class Info
    {
        public float L;
        public float[] Cm;
        public float[] Cbb;
    }
    private static Info calculateScaleAndCenter(List<Group> gl)
    {
        float Xm = Float.POSITIVE_INFINITY, XM = Float.NEGATIVE_INFINITY;
        float Ym = Float.POSITIVE_INFINITY, YM = Float.NEGATIVE_INFINITY;
        float Zm = Float.POSITIVE_INFINITY, ZM = Float.NEGATIVE_INFINITY;
        float X = 0, Y = 0, Z = 0;  // center for Cm
        int nv = 0;
        for (Group g: gl) {
            Map<String, ArrayInterface> map = g.map;
            VertexArray v_a = (VertexArray) map.get("v");
            nv += v_a.size();
            for (List<StringValue> svl: v_a) {
                float x = svl.get(0).f;
                float y = svl.get(1).f;
                float z = svl.get(2).f;
                X += x; Y += y; Z += z;
                if (x < Xm)
                    Xm = x;
                else if (x > XM)
                    XM = x;
                if (y < Ym)
                    Ym = y;
                else if (y > YM)
                    YM = y;
                if (z < Zm)
                    Zm = z;
                else if (z > ZM)
                    ZM = z;
            }
        }
        System.out.printf("Bounding Box: (%f, %f), (%f, %f), (%f, %f)%n", Xm, XM, Ym, YM, Zm, ZM);
        float lx = XM - Xm;
        float ly = YM - Ym;
        float lz = ZM - Zm;
        Info info = new Info();
        info.L = lx > ly ? lx : ly;
        if (lz > info.L)
            info.L = lz;
        System.out.printf("(%f, %f, %f)%n", lx, ly, lz);
        System.out.printf("L: %f%n", info.L);
        info.Cm = new float[] {X / nv, Y / nv, Z / nv};
        info.Cbb = new float[] {(Xm + XM) / 2, (Ym + YM) / 2, (Zm + ZM) / 2};
        System.out.printf("Center of Points: (%f, %f, %f)%n", info.Cm[0], info.Cm[1], info.Cm[2]);
        System.out.printf("Center of Bounding Box: (%f, %f, %f)%n", info.Cbb[0], info.Cbb[1], info.Cbb[2]);
        return info;
    }
    private static void fitToCube(List<Group> gl, Info info)
    {
        for (Group g: gl) {
            Map<String, ArrayInterface> map = g.map;
            VertexArray v_a = (VertexArray) map.get("v");
            for (List<StringValue> svl: v_a) {
                svl.get(0).f = (svl.get(0).f - info.Cbb[0]) / info.L;
                svl.get(1).f = (svl.get(1).f - info.Cbb[1]) / info.L;
                svl.get(2).f = (svl.get(2).f - info.Cbb[2]) / info.L;
            }
        }
    }
    private static void export(List<Group> gl, String objFile)
    {
        final String oviaFile = objFile + "_ovia.gz";
        try {
            ObjectInputStream ois = new ObjectInputStream(new GZIPInputStream(new BufferedInputStream(new FileInputStream(oviaFile))));
            float[] va3 = (float[]) ois.readObject();
            System.out.println("IN: # of floats = " + va3.length);
            int k = ois.readInt();
            System.out.println("IN: # of groups = " + k);
            for (int i = 0; i < k; ++i) {
                String name = (String) ois.readObject();
                System.out.printf("IN: name: |%s|%n", name);
                int[] ia3 = (int[]) ois.readObject();
                System.out.println("IN: # of indides = " + ia3.length);
            }
            ois.close();
        } catch (IOException e) {     // new FileOutputStream(...)
            System.err.println(e.getMessage());
        } catch (ClassNotFoundException e) {
            System.err.println(e.getMessage());
        }

        try {
            ObjectOutputStream oos = new ObjectOutputStream(new GZIPOutputStream(new BufferedOutputStream(new FileOutputStream(oviaFile))));

            int nv3 = 0;
            for (Group g: gl) {
                Map<String, ArrayInterface> map = g.map;
                VertexArray v_a = (VertexArray) map.get("v");
                nv3 += 3 * v_a.size();
            }
            float[] va3 = new float[nv3];
            int i = 0;
            for (Group g: gl) {
                Map<String, ArrayInterface> map = g.map;
                VertexArray v_a = (VertexArray) map.get("v");
                for (List<StringValue> svl: v_a) {
                    StringValue sv1 = svl.get(0);
                    StringValue sv2 = svl.get(1);
                    StringValue sv3 = svl.get(2);
                    va3[i++] = sv1.f;
                    va3[i++] = sv2.f;
                    va3[i++] = sv3.f;
                }
            }
            System.out.println("OUT: # of floats = " + nv3);
            oos.writeObject(va3);
            oos.writeInt(gl.size());
/*
        for (float f: va3)
            System.out.println(f);
 */
            for (int k = 0; k < gl.size(); ++k) {
                Group g = gl.get(k);
                Map<String, ArrayInterface> map = g.map;
                IndexArray f_a = (IndexArray) map.get("f");
                int[] ia3 = new int[3 * f_a.size()];
                System.out.printf("OUT: |%s|: # of faces = %d (%d)%n", g.name, f_a.size(), 3 * f_a.size());
                oos.writeObject(g.name);
                int j = 0;
                for (List<int[]> ial: f_a) {
                    assert ial.size() == 3 : "Triangle only";
                    for (int[] ia: ial) {
                        int iv = ia[0];
                        ia3[j++] = iv - 1;
                    }
                }
                oos.writeObject(ia3);
    /*
                if (k != 7)
                    continue;
                for (int idx: ia3)
                    System.out.println(idx);
     */
            }
            oos.close();
        } catch (IOException e) {     // new FileOutputStream(...)
            System.err.println(e.getMessage());
        }
    }

    public static void main(String[] args)
    {
        if (args.length == 0) {
            System.out.println("Usage: ...");
            return;
        }
        List<Group> gl = ImportObj.parse(args[0]);
        ImportObj.checkIndices(gl);
//      ImportObj.rebuildObj(gl, false);
        Info info = ImportObj.calculateScaleAndCenter(gl);
        ImportObj.fitToCube(gl, info);
//      ImportObj.calculateScaleAndCenter(gl);
////    ImportObj.rebuildObj(gl, true);
        ImportObj.export(gl, args[0]);
    }
}
