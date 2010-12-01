#!/bin/bash -login

[ -f /etc/profile ] && . /etc/profile

[ -f $HOME/.profile ] && . $HOME/.profile

if [ -f $HOME/.xinitrc ]; then
	exec /bin/sh  $HOME/.xinitrc
else
    if test -z "$WINDOWMANAGER" ; then
        WINDOWMANAGER=/usr/X11R6/bin/fvwm2
    fi
    exec $WINDOWMANAGER
fi

