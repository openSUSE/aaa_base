if test -z "$XDG_DATA_DIRS" ; then
    XDG_DATA_DIRS=/usr/local/share/
    for xdgdir in /usr/share /etc/opt/*/share /opt/*/share /usr/share/gnome ; do
	test -d $xdgdir && test -d $xdgdir/applications && XDG_DATA_DIRS="$XDG_DATA_DIRS:$xdgdir"
    done
    export XDG_DATA_DIRS
	
fi
if test -z "$XDG_CONFIG_DIRS" ; then
    export XDG_CONFIG_DIRS=/usr/local/etc/xdg/
    for xdgdir in /etc/xdg /etc/opt/*/xdg ; do
	test -d $xdgdir && XDG_CONFIG_DIRS="$XDG_CONFIG_DIRS:$xdgdir"
    done
    export XDG_CONFIG_DIRS
fi
unset xdgdir
