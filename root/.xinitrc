#!/bin/bash
#
# .xinitrc
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
    WINDOWMANAGER=/usr/X11R6/bin/twm
fi

#
# Load system and users resources if not already done
# (XSESSION_IS_UP set by xdm in $XLIBDIR/xdm/Xsession)
#
if test "$XSESSION_IS_UP" != "yes" ; then
    XLIBDIR=/usr/X11R6/lib/X11
    test -r $XLIBDIR/Xmodmap    && xmodmap $XLIBDIR/Xmodmap
    test -r $HOME/.Xmodmap      && xmodmap $HOME/.Xmodmap
    test -r $XLIBDIR/Xresources && xrdb -load -retain $XLIBDIR/Xresources
    test -r $HOME/.Xdefaults    && xrdb -I$HOME -merge $HOME/.Xdefaults
    test -r $HOME/.Xresources   && xrdb -I$HOME -merge $HOME/.Xresources
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
