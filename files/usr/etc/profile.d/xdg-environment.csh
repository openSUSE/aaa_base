# keep in sync with /usr/lib/environment.d/50-xdg.conf
# System XDG - SUSE configured
if ( ! ${?XDG_CONFIG_DIRS} ) then
  setenv XDG_CONFIG_DIRS /etc/xdg:/usr/local/etc/xdg:/usr/etc/xdg
endif

# System XDG - explicit defaults
if ( ! ${?XDG_DATA_DIRS} ) setenv XDG_DATA_DIRS /usr/local/share:/usr/share

# XDG_RUNTIME_DIR is set by pam_systemd
