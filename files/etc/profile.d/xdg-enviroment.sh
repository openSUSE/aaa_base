if test -z "$XDG_DATA_DIRS" ; then
    export XDG_DATA_DIRS=/usr/local/share/:/usr/share/:/etc/opt/kde3/share/:/opt/kde3/share/:/opt/gnome/share/
fi
if test -z "$XDG_CONFIG_DIRS" ; then
    export XDG_CONFIG_DIRS=/usr/local/etc/xdg/:/etc/xdg/:/etc/opt/gnome/xdg/
fi
