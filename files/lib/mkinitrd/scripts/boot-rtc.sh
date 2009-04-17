#!/bin/bash
#
#%stage: boot
#%depends: start
#%modules: rtc_cmos
#%provides: rtc
#%programs: /bin/date /bin/ln /bin/mknod /sbin/modprobe /bin/usleep
#%dontshow

if test -n "$(modprobe -C /dev/null --ignore-install --show-depends rtc_cmos 2>/dev/null)" -a ! -e /sys/class/rtc/rtc0
then
    load_modules
    typeset -i rtccount=300
    while ((rtccount-- > 0)) ; do
	test -e /sys/class/rtc/rtc0 && break
	usleep 10000
    done
    unset rtccount
    if test ! -e /dev/rtc0 ; then
	mknod -m 0644 /dev/rtc0 c 250 0
	ln -sf rtc0 /dev/rtc
    fi
    if test -e /etc/localtime
    then
	echo -n 'System time: '
	date --rfc-822
    fi
fi
