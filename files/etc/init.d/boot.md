#! /bin/sh
#
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# /etc/init.d/boot.md
#
### BEGIN INIT INFO
# Provides:          boot.md
# Required-Start:    boot.proc boot.ibmsis
# Required-Stop:
# Default-Start:     B
# Default-Stop:
# Description:       start multiple devices
### END INIT INFO

. /etc/rc.status
#
# maybe we use "Multiple devices".  So initialize MD.
#
if test -f /etc/raidtab -a -x /sbin/raid0run ; then
    echo "Initializing Multiple Devices..."
    /sbin/raid0run -a
    if test -x /sbin/raidstart ; then
	/sbin/raidstart -a
    fi
elif test -f /etc/mdtab -a -x /sbin/mdadd ; then
    echo "Initializing Multiple Devices..."
    /sbin/mdadd -ar && MDADD_RETURN=0 || MDADD_RETURN=1
    if test $MDADD_RETURN -ne 0 ; then
        if test  -x /sbin/ckraid ; then
            echo "Initializing Multiple Devices failed.  Trying to recover it..."
	    /sbin/mdstop -a	
            for i in /etc/raid?.conf ; do
                /sbin/ckraid --fix $i
            done
            /sbin/mdadd -ar
	    rc_status -v1 -r
        else
            rc_status -v1 -r
        fi
    fi
fi

rc_splash "early stop"

