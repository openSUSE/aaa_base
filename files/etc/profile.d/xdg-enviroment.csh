if ( ${?XDG_DATA_DIRS} ) then
    setenv XDG_DATA_DIRS /usr/local/share/:/usr/share/:/etc/opt/kde3/share/:/opt/kde3/share/:/opt/gnome/share/
endif
if ( ${?XDG_CONFIG_DIRS} ) then
    setenv XDG_CONFIG_DIRS /usr/local/etc/xdg/:/etc/xdg/:/etc/opt/gnome/xdg/
endif
