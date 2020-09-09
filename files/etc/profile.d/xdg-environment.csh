if ( ! ${?XDG_DATA_DIRS} ) then
    set XDG_DATA_DIRS
else
    set XDG_DATA_DIRS=(${XDG_DATA_DIRS:as/:/ /})
endif
set nonomatch
foreach xdgdir (/usr/local/share /usr/share /etc/opt/gnome/share /etc/opt/kde4/share /etc/opt/kde3/share /opt/gnome/share /opt/kde4/share /opt/kde3/share /usr/share/gnome)
    if ( -d "$xdgdir" && -d "$xdgdir/applications" ) then
	set -l XDG_DATA_DIRS=($XDG_DATA_DIRS $xdgdir)
    endif
end
unset nonomatch

set    xdgdir="${XDG_DATA_DIRS:q}"
unset  XDG_DATA_DIRS
setenv XDG_DATA_DIRS "${xdgdir:as/ /:/}"
unset  xdgdir

if ( ! ${?XDG_CONFIG_DIRS} ) then
    set XDG_CONFIG_DIRS
else
    set XDG_CONFIG_DIRS=(${XDG_CONFIG_DIRS:as/:/ /})
endif
set nonomatch
foreach xdgdir (/usr/local/etc/xdg /etc/xdg /usr/etc/xdg /etc/opt/gnome/xdg /etc/opt/kde4/xdg /etc/opt/kde3/xdg)
    if ( -d "$xdgdir" ) then
	set -l XDG_CONFIG_DIRS=($XDG_CONFIG_DIRS $xdgdir)
    endif
end
unset nonomatch

set    xdgdir="${XDG_CONFIG_DIRS:q}"
unset  XDG_CONFIG_DIRS
setenv XDG_CONFIG_DIRS "${xdgdir:as/ /:/}"
unset  xdgdir
