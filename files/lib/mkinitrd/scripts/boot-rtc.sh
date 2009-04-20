#!/bin/bash
#
#%stage: boot
#%depends: start
#%modules: rtc_cmos
#%provides: rtc
#%programs: /bin/ln /bin/mknod /sbin/modprobe /bin/usleep
#%if: -n "$(modprobe -C /dev/null --set-version $kernel_version --ignore-install --show-depends rtc_cmos 2>/dev/null)"
#%dontshow

if test ! -e /sys/class/rtc/rtc0
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
fi
