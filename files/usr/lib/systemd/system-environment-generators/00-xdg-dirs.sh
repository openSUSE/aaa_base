#!/bin/sh

# XDG directories, see
# https://specifications.freedesktop.org/basedir-spec/latest/ar01s03.html
#
# System XDG - SUSE configured
# According to spec this would only read /etc when unset
echo "XDG_CONFIG_DIRS=/etc/xdg:/usr/local/etc/xdg:/usr/etc/xdg"
#
# System XDG - explicit default
echo "XDG_DATA_DIRS=/usr/local/share/:/usr/share/"
