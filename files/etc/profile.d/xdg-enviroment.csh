set nonomatch
if ( ! ${?XDG_DATA_DIRS} ) then
    setenv XDG_DATA_DIRS /usr/local/share/
    foreach xdgdir ( /usr/share /etc/opt/*/share /opt/*/share )
	if ( -d $xdgdir ) then
	    if ( -d $xdgdir/applications ) then
		setenv XDG_DATA_DIRS "${XDG_DATA_DIRS}:${xdgdir}"
	    endif
	endif
    end

endif
if ( ! ${?XDG_CONFIG_DIRS} ) then
    setenv XDG_CONFIG_DIRS /usr/local/etc/xdg/
    foreach xdgdir ( /etc/xdg /etc/opt/*/xdg )
	if ( -d $xdgdir ) then
	    setenv XDG_CONFIG_DIRS "${XDG_CONFIG_DIRS}:${xdgdir}"
	endif
    end
endif
unset nonomatch
unset xdgdir
