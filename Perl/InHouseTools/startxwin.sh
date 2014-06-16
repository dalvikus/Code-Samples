#! /bin/sh

# A user has reported that setting DISPLAY to the actual hostname
# gives the X Server an increase in responsiveness.  We will
# give this a try and see if it causes and problems.
# Use DISPLAY=127.0.0.1:0.0 if you have problems.
# Harold - 2002/05/08
export DISPLAY=$HOSTNAME:0.0
PATH=/usr/X11R6/bin:$PATH

# Cleanup from last run.
rm -rf /tmp/.X11-unix


# Startup the X Server, the twm window manager, and an xterm.
# 
# Notice that the window manager and the xterm will wait for
# the server to finish starting before trying to connect; the
# error "Cannot Open Display: 127.0.0.1:0.0" is not due to the
# clients attempting to connect before the server has started, rather
# that error is due to a bug in some versions of cygwin1.dll.  Upgrade
# to the latest cygwin1.dll if you get the "Cannot Open Display" error.
# See the Cygwin/XFree86 FAQ for more information:
# http://xfree86.cygwin.com/docs/faq/
#
# The error "Fatal server error: could not open default font 'fixed'" is
# caused by using a DOS mode mount for the mount that the Cygwin/XFree86
# fonts are accessed through.  See the Cygwin/XFree86 FAQ for more 
# information:
# http://xfree86.cygwin.com/docs/faq/cygwin-xfree-faq.html#q-error-font-eof


# Start the X Server.
XWin -fullscreen &
#XWin -screen 0 1024 768 &

# Start the twm window manager.
#twm &  
#fvwm2 &
wmaker &
#mwm &

# Set a background color to hide that nasty X stipple.
#xsetroot -solid aquamarine4

# Start an xterm.
#xterm -sl 1000 -sb -rightbar -ms red -fg yellow -bg black -e /bin/bash &
rxvt -fn -adobe-courier-medium-r-normal--18-180-75-75-m-110-iso8859-10 -geometry +100+100 -sb -sr -title "a VT102 emulator for the X window system" -e /bin/bash --login -i &
#xterm -fn -adobe-courier-medium-r-normal--18-180-75-75-m-110-iso8859-10 -geometry +100+100 -sl 1000 -sb -rightbar -title "terminal emulator for X" -e /bin/bash --login -i &

# Return from sh.
exit
