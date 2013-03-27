#!/bin/bash
#
#%stage: setup
#%programs: /bin/ln
#%modules:
#%dontshow

if test -w /root/etc/ -a ! -L /root/etc/mtab ; then
	echo "replacing /etc/mtab with symlink to /proc/self/mounts"
	ln -sf /proc/self/mounts /root/etc/mtab
fi
