#!/bin/bash
#
#%stage: boot
#%depends: start rtc
#%provides: clock
#%programs: /bin/date
#%dontshow

if test -e /etc/sysconfig/clock
then
    . /etc/sysconfig/clock
    case "$HWCLOCK" in
    *-l*)
	date --set "$(date --utc +'%Y-%m-%d %H:%M:%S.%N')" > /dev/null
	;;
    esac
fi
if test -e /etc/localtime
then
    echo -n 'System time: '
    date --rfc-822
fi
