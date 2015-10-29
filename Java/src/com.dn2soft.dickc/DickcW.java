package com.dn2soft.dickc;

import com.dn2soft.dickc.DickcWrapper;
import com.dn2soft.dickc.dictionary.Cambridge;
import com.dn2soft.util.GetOpt;


import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import javax.swing.*;
import javax.swing.text.*;

import java.util.List;
import java.util.regex.Pattern;
import java.util.regex.Matcher;


public class DickcW
    extends JPanel
    implements
        ActionListener,
        PropertyChangeListener,
        MouseMotionListener,
        MouseListener
{
/*
    static final String optStr = "k";//"kax";
    static final String optStrForExpert = "svt";//"sNvthHT";
 */
    private Cambridge.Flag  flag    = new Cambridge.Flag();

    private boolean USE_PROGRESSBAR = false;
    private boolean SHOW_LOG = false;
// #ifdef USE_PROGRESSBAR
    private JProgressBar    progressBar;
// #endif //USE_PROGRESSBAR
    private JButton     startButton;
    private JTextArea   taskOutput;
    private Task        task;
// TextComponentDemo.java   // {
    private AbstractDocument    doc;
    private final String    newline = "\n";
    private final String    LOOKUP_CMD_STR = "lookup";
    private final String    WORD_CMD_STR = "word";
    private JTextPane   textPane;
    private JTextField  wordStrField;
// TextComponentDemo.java   // }
// TextDemo.java    // {
    SimpleAttributeSet  ATTR_TEXT = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_WORD = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_IPA = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_POS = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_GW = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_DEF = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_EXAMPLE_DOT = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_EXAMPLE = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_LINK = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_INFO = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_WARN = new SimpleAttributeSet();
    SimpleAttributeSet  ATTR_ERROR = new SimpleAttributeSet();
    Cambridge.Result    result = null;
    int startIndexOfClickableLine = -1; // start index of clickable line
// TextDemo.java    // }

    public DickcW() {
        super(new BorderLayout());
/*
    public static class Flag {
        boolean uk      = false;
        boolean showAll     = false;
        boolean playPron    = false;
        boolean showLink    = true;
        boolean ansi        = true;
        boolean verbose     = false;
        boolean trace       = false;
        boolean showHref0   = false;
        boolean showHref0Only   = false;// if set, showHref0 implied
        boolean followWord  = false;// gui mode only
        boolean resultOnly  = false;
    }
 */
        // except...
        flag.ansi       = false;
        flag.followWord     = true;
        flag.resultOnly     = true;
        init();
    }
    public DickcW(Cambridge.Flag flag) {
        super(new BorderLayout());
        this.flag = flag;
        init();
    }
    private void    init() {
        //Create the demo's UI.
        Font    font = new Font("SansSerif", Font.PLAIN, 20);
        startButton = new JButton("look up");
        //startButton.setName("startButton");
        startButton.setFont(font);
//      StyleConstants.setForeground(ATTR_TEXT, new Color(0xBA, 0xBD, 0xB6));
        startButton.setForeground(new Color(0xBA, 0xBD, 0xB6));
        startButton.setBackground(new Color(0x2E, 0x34, 0x36));
        startButton.setActionCommand(LOOKUP_CMD_STR);
        startButton.addActionListener(this);

// #ifdef USE_PROGRESSBAR
    if (USE_PROGRESSBAR) {
        progressBar = new JProgressBar(0, 100);
        //progressBar.setName("progressBar");
        progressBar.setFont(font);
        progressBar.setForeground(new Color(0xBA, 0xBD, 0xB6));
        progressBar.setBackground(new Color(0x2E, 0x34, 0x36));
        progressBar.setValue(0);
        //Call setStringPainted now so that the progress bar height
        //stays the same whether or not the string is shown.
        progressBar.setStringPainted(true); 
    }
// #endif //USE_PROGRESSBAR

        wordStrField = new JTextField(20);
        //wordStrField.setName("wordStrField");
        wordStrField.setFont(font);
        wordStrField.setForeground(new Color(0xBA, 0xBD, 0xB6));
        wordStrField.setBackground(new Color(0x2E, 0x34, 0x36));
        wordStrField.setActionCommand(WORD_CMD_STR);
        wordStrField.addActionListener(this);
        wordStrField.setText("type word here");
        wordStrField.selectAll();

        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
//      JPanel panel = new JPanel(new BorderLayout());
        //panel.setName("panel");
        panel.setBackground(new Color(0x2E, 0x34, 0x36));
        panel.add(wordStrField);
        panel.add(startButton);
//      panel.add(wordStrField, BorderLayout.CENTER);
//      panel.add(startButton, BorderLayout.LINE_END);
// #ifdef USE_PROGRESSBAR
    if (USE_PROGRESSBAR) {
        panel.add(progressBar);
    }
// #endif //USE_PROGRESSBAR

        add(panel, BorderLayout.PAGE_START);
//      add(new JScrollPane(taskOutput), BorderLayout.CENTER);
//      setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
    // TextComponentDemo.java   // {
        textPane = new JTextPane();
        //textPane.setName("textPane");
        textPane.setEditable(false);
        textPane.setBackground(new Color(0x2E, 0x34, 0x36));
        textPane.setCaretPosition(0);
        // keep view port fixed
        DefaultCaret    caret = (DefaultCaret) textPane.getCaret();
        caret.setUpdatePolicy(DefaultCaret.NEVER_UPDATE);

//      textPane.setMargin(new Insets(5,5,5,5));
        StyledDocument  styledDoc = textPane.getStyledDocument();
        if (styledDoc instanceof AbstractDocument) {
            doc = (AbstractDocument)styledDoc;
        } else {
            System.err.println("Text pane's document isn't an AbstractDocument!");
            System.exit(-1);
        }
        JScrollPane scrollPane = new JScrollPane(textPane);
        //scrollPane.setName("scrollPane");
        scrollPane.setPreferredSize(new Dimension(800, 400));

        taskOutput = new JTextArea(5, 20);
        //taskOutput.setName("taskOutput");
        taskOutput.setMargin(new Insets(5,5,5,5));
        taskOutput.setEditable(false);

        //Create the text area for the status log and configure it.
        JScrollPane scrollPaneForLog = new JScrollPane(taskOutput);
        //scrollPaneForLog.setName("scrollPaneForLog");
        scrollPane.setMinimumSize(new Dimension(100, 0));
        scrollPaneForLog.setMinimumSize(new Dimension(100, 0));
        scrollPaneForLog.setPreferredSize(new Dimension(100, 0));

        //Create a split pane for the change log and the text area.
        JSplitPane splitPane = new JSplitPane(
            JSplitPane.VERTICAL_SPLIT,
            scrollPane, scrollPaneForLog
        );
        if (!SHOW_LOG) {
            splitPane.setDividerSize(0);
        }
        //splitPane.setName("splitPane");
        splitPane.setOneTouchExpandable(true);
        splitPane.setResizeWeight(1.0); // hide bottom if possible
    // TextComponentDemo.java   // }

        //Add the components.
        add(splitPane, BorderLayout.CENTER);

        textPane.setCaretPosition(0);

        textPane.addMouseMotionListener(this);
        textPane.addMouseListener(this);

        setFont();
    }
    private void    setFont()
    {
        int fnSize = 20;
        //String    fnName = "Monospaced";
        String  fnName = "SansSerif";
        ATTR_TEXT = new SimpleAttributeSet();
        StyleConstants.setFontFamily(ATTR_TEXT, fnName);
        StyleConstants.setFontSize(ATTR_TEXT, fnSize);
        StyleConstants.setBold(ATTR_TEXT, false);
        StyleConstants.setForeground(ATTR_TEXT, new Color(0xBA, 0xBD, 0xB6));
        ATTR_WORD = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_WORD, new Color(0x34, 0xE2, 0xE2));
        ATTR_IPA = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_IPA, new Color(0x72, 0x9F, 0xCF));
        ATTR_POS = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_POS, new Color(0xFC, 0xE9, 0x4F));
        ATTR_GW = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_GW, new Color(0xAD, 0x7F, 0xA8));
        ATTR_DEF = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_DEF, new Color(0x8A, 0xE2, 0x34));
        ATTR_EXAMPLE_DOT = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_EXAMPLE_DOT, new Color(0x34, 0xE2, 0xE2));
        ATTR_EXAMPLE = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_EXAMPLE, new Color(0x06, 0x98, 0x9A));
        ATTR_LINK = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_LINK, new Color(0x34, 0x65, 0xA4));
        ATTR_INFO = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_INFO, new Color(0xC4, 0xA0, 0x00));
        ATTR_WARN = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_WARN, Color.yellow);
        ATTR_ERROR = new SimpleAttributeSet(ATTR_TEXT);
        StyleConstants.setForeground(ATTR_ERROR, Color.red);
    }

    private void    lookup()
    {
        setLookupUI();
        task = new Task();
// #ifdef USE_PROGRESSBAR
    if (USE_PROGRESSBAR) {
        task.addPropertyChangeListener(this);
    }
// #endif //USE_PROGRESSBAR
        task.execute();
    }
    /**
     * Invoked when the user presses the start button.
     */
    public void actionPerformed(ActionEvent evt) {
        if (
            !evt.getActionCommand().equals(WORD_CMD_STR)
                &&
            !evt.getActionCommand().equals(LOOKUP_CMD_STR)
        )
            return;
        if (wordStrField.getText().equals(""))
            return;
        lookup();
    }

    //// only in called when USE_PROGRESSBAR set
// #ifdef USE_PROGRESSBAR
    public void propertyChange(PropertyChangeEvent evt) {
        if ("progress" == evt.getPropertyName()) {
            int progress = (Integer) evt.getNewValue();
            progressBar.setIndeterminate(true);
            progressBar.setValue(progress);
            taskOutput.append(String.format("Completed %d%% of task.", progress) + newline);
        }
    }
// #endif //USE_PROGRESSBAR

    private String  getWordStr(MouseEvent e, boolean b)
    {
        String  wordStr = null;
        String  word = null;
try {
    while (true) {
        if (result == null)
            break;

        int dot = textPane.viewToModel(e.getPoint());
        Element root = doc.getDefaultRootElement();
        int iLine = root.getElementIndex(dot);  // i-th line
        Element lineElement = root.getElement(iLine);   // i-th line element
        int iStartOffset = lineElement.getStartOffset();
        int iEndOffset = lineElement.getEndOffset();

        // line
        String  line = null;
    try {
        line = doc.getText(iStartOffset, iEndOffset - iStartOffset);
    }
    catch (BadLocationException ignore) {}
        if (line == null)
            break;

        // word
    while (flag.followWord) {
        //taskOutput.append("[" + iStartOffset + ", " + dot + ", " + iEndOffset + "]" + newline);
        if (dot == iEndOffset - 1)
            break;
        int iCol = dot - iStartOffset - 1;
        if (iCol < 0)
            iCol = 0;
        //taskOutput.append("<" + line.charAt(iCol) + ">" + newline);
        if (!Character.toString(line.charAt(iCol)).matches("[a-zA-Z]"))
            break;
        int ii = iCol;
        for (int i = iCol - 1; i >= 0; --i) {
            if (Character.toString(line.charAt(i)).matches("[a-zA-Z]"))
                ii = i;
            else
                break;
        }
        int fi = iCol;
        for (int i = iCol + 1; i < line.length(); ++i) {
            if (Character.toString(line.charAt(i)).matches("[a-zA-Z]"))
                fi = i;
            else
                break;
        }
/*
        int ii = line.lastIndexOf(" ", iCol);
        ++ii;
        int fi = line.indexOf(" ", iCol);
        if (fi < 0)
            fi = line.length() - 1;
 */
        //taskOutput.append("(" + iLine + ", " + iCol + "): (" + ii + ", " + fi + ")" + newline);
        String  w = null;
        try {
            w = doc.getText(iStartOffset + ii, fi - ii + 1);
        }
        catch (BadLocationException ignore) {}
        if (w == null)
            break;
        //taskOutput.append("|" + w + "|" + newline);
        if (w.startsWith("/") && w.endsWith("/"))
            break;
        word = w.replaceAll("[^a-zA-Z]*", "");
        //taskOutput.append("|" + w + "|" + newline);
        break;
    }
        if (
            startIndexOfClickableLine >= 0 &&
            iLine >= startIndexOfClickableLine &&
            (iStartOffset <= dot && dot < (iEndOffset - 1)) // 1:= "\n"
        ) {
            if (result.tryWordList != null && result.tryWordList.isEmpty()) {
                wordStr = result.tryWordList.get(iLine - startIndexOfClickableLine);
            } else {
                wordStr = result.doc.get(0).more.infoList.get(iLine - startIndexOfClickableLine).href;
            }
        } else {
            wordStr = null;
        }
        break;
    }   // end of while
}
finally {
//  taskOutput.append("wordStr: \"" + wordStr + "\"" + newline);
//  taskOutput.append("word: \"" + word + "\"" + newline);
}
        return wordStr == null ? word : wordStr;
    }
// MouseMotionListener  // {
    public void mouseMoved(MouseEvent e) {
        if (getWordStr(e, false) == null)
            textPane.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
        else
            textPane.setCursor(new Cursor(Cursor.HAND_CURSOR));
    }
    public void mouseDragged(MouseEvent e) {}
// MouseMotionListener  // }
// MouseListener    // {
    public void mouseClicked(MouseEvent e) {
        String  s = getWordStr(e, true);
        if (s == null)
            return;
        taskOutput.append(s + newline);
        wordStrField.setText(s);

        lookup();
    }
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    public void mousePressed(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}
// MouseListener    // }
    private void walk(Container parent, int depth) {
        String  tab = new String(new char[depth]).replace("\0", "    ");
        System.out.println(tab + parent.getName());
        System.out.println(tab + parent.getComponentCount());
        if (parent.getComponentCount() == 0)
            return;
        for (Component c: parent.getComponents())
            walk((Container) c, depth + 1);
    }
    private void setWaitOrFree(Container parent, boolean bWait) {
        parent.setCursor(new Cursor(bWait ? Cursor.WAIT_CURSOR : Cursor.DEFAULT_CURSOR));
        if (parent.getComponentCount() == 0)
            return;
        for (Component c: parent.getComponents()) {
            c.setCursor(new Cursor(bWait ? Cursor.WAIT_CURSOR : Cursor.DEFAULT_CURSOR));
            setWaitOrFree((Container) c, bWait);
        }
    }
    private void setDoneUI()
    {
        //walk(textPane.getRootPane().getContentPane(), 0);
        setWaitOrFree(textPane.getRootPane().getContentPane(), false);
// #ifdef USE_PROGRESSBAR
    if (USE_PROGRESSBAR) {
        progressBar.setIndeterminate(false);
    }
// #endif //USE_PROGRESSBAR
        startButton.setEnabled(true);
        taskOutput.append("Done!\n");
        wordStrField.selectAll();
        wordStrField.requestFocus();
        textPane.setCaretPosition(0);
        textPane.setText("");
    }
    private void setLookupUI()
    {
        setWaitOrFree(textPane.getRootPane().getContentPane(), true);
        taskOutput.setText("");
// #ifdef USE_PROGRESSBAR
    if (USE_PROGRESSBAR) {
        progressBar.setIndeterminate(true);
    }
// #endif //USE_PROGRESSBAR
        startButton.setEnabled(false);
    }


class Task extends SwingWorker<Cambridge.Result, Void> {
    @Override
    public Cambridge.Result doInBackground()
    throws java.io.IOException, InterruptedException, ClassNotFoundException, java.sql.SQLException
    {
        setProgress(0);
        String wordStr = wordStrField.getText();
        Cambridge.Result    result = null;
        if (!wordStr.equals(""))
            result = DickcWrapper.lookup(wordStr, flag);
        setProgress(100);
        return result;
    }

        /*
         * Executed in event dispatch thread
         */
    public void done() {
        setDoneUI();

        result = null;
    try {
        result = get();
    }
    catch (InterruptedException ignore) {}
    // Tutorial > Concurrency in Swing > Simple Background Tasks    // {
    catch (java.util.concurrent.ExecutionException e) {
        String  why = null;
        Throwable   cause = e.getCause();
        if (cause != null) {
            why = cause.getMessage();
        } else {
            why = e.getMessage();
        }
        System.err.println("done: Error: " + why);
    }
    // Tutorial > Concurrency in Swing > Simple Background Tasks    // }

        int lineIndex = 0;
try {
        if (result == null) {
            startIndexOfClickableLine = -1;
            doc.insertString(doc.getLength(), "null", ATTR_ERROR);
            return;
        }
    if (result.doc.isEmpty()) {
        doc.insertString(doc.getLength(), "WARN", ATTR_WARN);
        doc.insertString(doc.getLength(), ": ", ATTR_TEXT);
        doc.insertString(doc.getLength(), result.wordStr, ATTR_WORD);
        doc.insertString(doc.getLength(), " not found" + newline, ATTR_TEXT);
        if (result.tryWordList != null && !result.tryWordList.isEmpty()) {
            lineIndex = 2;
            doc.insertString(doc.getLength(), "Try these:" + newline, ATTR_TEXT);
            for (String w: result.tryWordList) {
                doc.insertString(doc.getLength(), "        ", null);
                doc.insertString(doc.getLength(), w, ATTR_INFO);
                doc.insertString(doc.getLength(), newline, null);
            }
        }
        return;
    }
    for (Cambridge.Result.Page page: result.doc) {
    for (Cambridge.Block block: page.content) {
        Cambridge.Block.Head    head = block.head;
        doc.insertString(doc.getLength(), head.word, ATTR_WORD);
        if (!head.ipa.isEmpty()) {
            doc.insertString(doc.getLength(), " ", null);
            int i = 0;
            for (String s: head.ipa) {
                if (i++ != 0)
                    doc.insertString(doc.getLength(), ",", ATTR_WORD);
                doc.insertString(doc.getLength(), "/" + s + "/", ATTR_IPA);
            }
        }
        // pos is an array: "à la carte"
        if (!head.pos.isEmpty()) {
            doc.insertString(doc.getLength(), " ", null);
            doc.insertString(doc.getLength(), "(", ATTR_POS);
            int i = 0;
            for (String s: head.pos) {
                if (i++ != 0)
                    doc.insertString(doc.getLength(), ",", ATTR_WORD);
                doc.insertString(doc.getLength(), s, ATTR_POS);
            }
            doc.insertString(doc.getLength(), ")", ATTR_POS);
        }
        if (head.gw != null && !head.gw.isEmpty()) {
            doc.insertString(doc.getLength(), " ", null);
            doc.insertString(doc.getLength(), "[" + head.gw + "]", ATTR_GW);
        }
        doc.insertString(doc.getLength(), newline, null);
        ++lineIndex;
        for (Cambridge.Block.Body body: block.bodyList) {
            if (body.def != null) {
                doc.insertString(doc.getLength(), body.def, ATTR_DEF);
                doc.insertString(doc.getLength(), newline, null);
                ++lineIndex;
            }
            for (String e: body.example) {
                doc.insertString(doc.getLength(), "    ", null);
                doc.insertString(doc.getLength(), "·", ATTR_EXAMPLE_DOT);
                doc.insertString(doc.getLength(), " ", null);
                doc.insertString(doc.getLength(), e, ATTR_EXAMPLE);
                doc.insertString(doc.getLength(), newline, null);
                ++lineIndex;
            }
        }
    }
    }
    if (!result.doc.get(0).more.infoList.isEmpty()) {
        doc.insertString(doc.getLength(), newline, null);
        doc.insertString(doc.getLength(), "more:", ATTR_INFO);
        doc.insertString(doc.getLength(), newline, null);
        lineIndex += 2;
    }
    for (Cambridge.More.Info info: result.doc.get(0).more.infoList) {
        doc.insertString(doc.getLength(), info.word, ATTR_WORD);
        if (info.pos != null && !info.pos.isEmpty()) {
            doc.insertString(doc.getLength(), " ", null);
            doc.insertString(doc.getLength(), "(" + info.pos + ")", ATTR_POS);
        }
        if (info.gw != null && !info.gw.isEmpty()) {
            doc.insertString(doc.getLength(), " ", null);
            doc.insertString(doc.getLength(), "[" + info.gw + "]", ATTR_GW);
        }
        doc.insertString(doc.getLength(), newline, null);
//      doc.insertString(doc.getLength(), newline + "    ", null);
//      doc.insertString(doc.getLength(), info.href, ATTR_LINK);
//      doc.insertString(doc.getLength(), newline, null);
    }
}
catch (BadLocationException ignore) {}
        startIndexOfClickableLine = lineIndex;
        }
}
/*
    private static void createAndShowGUI()
    {
        //Create and set up the window.
        JFrame  frame = new JFrame("DickcW");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //Create and set up the content pane.
        JComponent  newContentPane = new DickcW();
        newContentPane.setOpaque(true);
        frame.setContentPane(newContentPane);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }

    public static void main(String[] args) {
    try {
        boolean ansi = false;
        GetOpt.Option   option = GetOpt.getopt(args, optStr + optStrForExpert, ansi);
        flag.uk         = option.hasKey("-k");
        flag.showAll        = option.hasKey("-a");
        flag.showLink       = !option.hasKey("-N");
        flag.playPron       = option.hasKey("-s");
        flag.verbose        = option.hasKey("-v");
        flag.trace      = option.hasKey("-t");
        flag.showHref0      = option.hasKey("-h");
        flag.showHref0Only  = option.hasKey("-H");
    }
    catch (GetOpt.GetOptStrException e) {
        String  why = null;
        Throwable   cause = e.getCause();
        if (cause != null) {
            why = cause.getMessage();
        } else {
            why = e.getMessage();
        }
        System.err.println(why);
        System.exit(-1);
    }
    catch (GetOpt.GetOptParseException e) {
        String  why = null;
        Throwable   cause = e.getCause();
        if (cause != null) {
            why = cause.getMessage();
        } else {
            why = e.getMessage();
        }
        System.err.println(why);
        System.exit(-1);
    }
        flag.ansi       = false;
        flag.resultOnly     = true;

        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }

 */
}
