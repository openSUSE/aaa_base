#!/bin/bash -login
#
# If ssh is configured and ssh-agent is wanted
# set this to "yes"
#
usessh="no"

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

test "$PROFILEREAD" != "true" && . /etc/profile
test -f $HOME/.profile && . $HOME/.profile

#
# Run ssh-agent only if ssh is configured and avaliable.
#
sshagent="no"
if test "$usessh" = "yes" -a -d $HOME/.ssh ; then
	type ssh-agent > /dev/null 2>&1 && sshagent="yes"
fi

if test -f $HOME/.xinitrc ; then
    if test "$sshagent" = "yes" ; then
	exec ssh-agent $HOME/.xinitrc
    else
	exec /bin/bash $HOME/.xinitrc
    fi
else
    if test -z "$WINDOWMANAGER" ; then
        WINDOWMANAGER=/usr/X11R6/bin/startkde
    fi
    exec $WINDOWMANAGER
fi

# call failsafe
exit 0
