/*
 * There are few ways to deploy applet:
 *    http://docs.oracle.com/javase/tutorial/deployment/applet/deployingApplet.html
 * Since this applet internally connects to "another" address,
 * without "full" permission, it does not work.
 * Without a certificate signed by an certificate authority (CA),
 * only one way (in "applet" tag below) is a choice.
 * Moreover, at least self-sign is needed:
 *    keytool -genkeypair -keystore keystore -alias icentrik -dname "CN=Gwang-Ho KIM, OU=R&D, O=DN2 Soft, L=San Diego, S=California, C=US"
 *    jarsigner -keystore keystore -signedjar dickSigned.jar lib/dick.jar icentrik
 */
/*
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en-US">
  <head>
    <title>Cambridge Dictionary Look-up</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  </head>
  <body>
    <applet
        alt = "Cambridge Dictionary Look-up"
        code = 'com.dn2soft.dick.DickApplet'
        archive = 'dickSigned.jar',
        width = 700,
        height = 500 >
      <param name="permissions" value="all-permissions">
    </applet>
  </body>
</html>
 */
package com.dn2soft.dick;

import com.dn2soft.dick.DickW;

import javax.swing.JApplet;

public class DickApplet extends JApplet {
    public void init() {
        try {
            javax.swing.SwingUtilities.invokeAndWait(new Runnable() {
                public void run() {
                    createGUI();
                }
            });
        } catch (Exception e) {
            System.err.println("createGUI didn't complete successfully");
        }
    }

    private void createGUI() {
        DickW newContentPane = new DickW();
        newContentPane.setOpaque(true);
        setContentPane(newContentPane);
    }
}
