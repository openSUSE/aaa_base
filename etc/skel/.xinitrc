#!/bin/bash
#
# .xsession/.xinitrc
#

#
# What we do if we fail at least ... emergency fall back.
#
failsafe="xterm -ls -T Failsave -geometry 80x24-0-0"
trap "exec $failsafe" EXIT SIGHUP SIGINT SIGPIPE SIGTERM SIGIO

#
# Some bash (1 and 2) settings to avoid trouble on a
# failed program call.
#
test -n "$BASH" && set +o posix
no_exit_on_failed_exec=1
type shopt > /dev/null 2>&1 && shopt -s execfail

#
# This should be the default
#
export TERM=xterm

#
# choose a window manager
#
if test -n "$WINDOWMANAGER" ; then
    WINDOWMANAGER=`type -p $WINDOWMANAGER`
fi
if test -z "$WINDOWMANAGER" ; then
    if test -x /usr/X11R6/bin/startkde ; then
        WINDOWMANAGER=/usr/X11R6/bin/startkde
    elif test -x /usr/X11R6/bin/fvwm2 ; then
        WINDOWMANAGER=/usr/X11R6/bin/fvwm2
    elif test -x /usr/X11R6/bin/wmlist ; then
        for i in `/usr/X11R6/bin/wmlist` ; do
            WINDOWMANAGER=`type -p $i`
            test -n "$WINDOWMANAGER" && break
        done
    fi
fi

if test -z "$WINDOWMANAGER" ; then
    echo "Error. Can not find a window manager.  Please install one."
    echo "Exit for now."
    exit 1
fi

#
# load resources
#
if [ -f /usr/X11R6/lib/X11/Xmodmap ]; then
    xmodmap /usr/X11R6/lib/X11/Xmodmap
fi

if [ -f ~/.Xmodmap ]; then
    xmodmap ~/.Xmodmap
fi

if [ -f ~/.Xdefaults ]; then
    xrdb -I$HOME -merge ~/.Xdefaults
fi

if [ -f ~/.Xresources ]; then
    xrdb -I$HOME -merge ~/.Xresources
fi


#
# start some stuff
#


# day planer deamon
# pland &


# 
# finaly start the window manager
#
exec $WINDOWMANAGER 

# call failsafe
exit 0
