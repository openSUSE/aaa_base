#!/bin/bash
#
# .xsession/.xinitrc
#

export TERM=xterm


#
# choose a window manager
#

if test -z "$WINDOWMANAGER" ; then
    WINDOWMANAGER=/usr/X11R6/bin/startkde
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
    xrdb -merge ~/.Xdefaults
fi

if [ -f ~/.Xresources ]; then
    xrdb -merge ~/.Xresources
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



