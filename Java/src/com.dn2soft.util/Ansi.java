package com.dn2soft.util;

/**
 * http://www.termsys.demon.co.uk/vtansi.htm
 */
public class Ansi
{
    private Ansi() {}   // prevent this class from being instanciated

    static final String ESC = "";
    static final String HIDE_CURSOR_STR = ESC + "[?25l";
    static final String SHOW_CURSOR_STR = ESC + "[?25h";
    static final String SAVE_CURSOR_STR = ESC + "[s";
    static final String RESTORE_CURSOR_STR = ESC + "[u";
    static final String SAVE_CURSOR_ATTR_STR = ESC + "7";
    static final String RESTORE_CURSOR_ATTR_STR = ESC + "8";
    static final String CLEAR_SCREEN_STR = ESC + "[2J";
    static final String CLEAR_END_OF_LINE_STR = ESC + "[K";
    static final String CLEAR_START_OF_LINE_STR = ESC + "[1K";
    static final String CLEAR_LINE_STR = ESC + "[2K";
    static final String CLEAR_DOWN_STR = ESC + "[1J";
    static final String CLEAR_UP_STR = ESC + "[2J";

    // and move cursor to home: (1, 1)
    public static void clearScreen() {System.out.printf(CLEAR_SCREEN_STR);}
    public static void clearEndOfLine() {System.out.printf(CLEAR_END_OF_LINE_STR);}
    public static void clearStartOfLine() {System.out.printf(CLEAR_START_OF_LINE_STR);}
    public static void clearLine() {System.out.printf(CLEAR_LINE_STR);}
    // including current line
    public static void clearDown() {System.out.printf(CLEAR_DOWN_STR);}
    // including current line
    public static void clearUp() {System.out.printf(CLEAR_UP_STR);}

        public static void hideCursor() {System.out.printf(HIDE_CURSOR_STR);}
        public static void showCursor() {System.out.printf(SHOW_CURSOR_STR);}
/*
    public static class CursorMovement
    {
        private CursorMovement() {}
 */
        public static void moveTo(int row, int col)
        {
            if (row < 1 || col < 1)
                throw new IllegalArgumentException(String.format("(%d, %d): Both should be positive", row, col));
            System.out.printf(ESC + "[" + row + ";" + col + "H");
        }
        public static void goHome() {moveTo(1, 1);}
        public static void moveRelativeTo(int row, int col)
        {
            if (row > 0)
                Ansi.down(row);
            else if (row < 0)
                Ansi.up(-row);
            if (col > 0)
                Ansi.forward(col);
            else if (col < 0)
                Ansi.backward(-col);
        }

        public static void up(int row)
        {
            if (row < 1)
                throw new IllegalArgumentException(String.format("%d: Should be positive", row));
            System.out.printf(ESC + "[" + row + "A");
        }
        public static void up() {up(1);}
        public static void down(int row)
        {
            if (row < 1)
                throw new IllegalArgumentException(String.format("%d: Should be positive", row));
            System.out.printf(ESC + "[" + row + "B");
        }
        public static void down() {down(1);}

        public static void forward(int col)
        {
            if (col < 1)
                throw new IllegalArgumentException(String.format("%d: Should be positive", col));
            System.out.printf(ESC + "[" + col + "C");
        }
        public static void forward() {forward(1);}
        public static void backward(int col)
        {
            if (col < 1)
                throw new IllegalArgumentException(String.format("%d: Should be positive", col));
            System.out.printf(ESC + "[" + col + "D");
        }
        public static void backward() {backward(1);}

        public static void saveCursor() {System.out.printf(SAVE_CURSOR_STR);}
        public static void restoreCursor() {System.out.printf(RESTORE_CURSOR_STR);}
        public static void saveCursorAttr() {System.out.printf(SAVE_CURSOR_ATTR_STR);}
        public static void restoreCursorAttr() {System.out.printf(RESTORE_CURSOR_ATTR_STR);}
/*
    }
 */
}
