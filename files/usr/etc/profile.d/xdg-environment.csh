# keep in sync with /usr/lib/environment.d/50-xdg.conf
# System XDG - SUSE configured
if ( ! ${?XDG_CONFIG_DIRS} ) then
  setenv XDG_CONFIG_DIRS /etc/xdg:/usr/local/etc/xdg:/usr/etc/xdg
endif

# System XDG - explicit defaults
if ( ! ${?XDG_DATA_DIRS} ) then
  setenv XDG_DATA_DIRS /usr/local/share/:/usr/share/
endif

# User XDG - explicit defaults
if (! ${?XDG_DATA_HOME} && ${?HOME} ) then
  setenv XDG_DATA_HOME $HOME/.local/share
endif
if (! ${?XDG_CONFIG_HOME} && ${?HOME} ) then
  setenv XDG_CONFIG_HOME $HOME/.config
endif
if (! ${?XDG_STATE_HOME} && ${?HOME} ) then
  setenv XDG_STATE_HOME $HOME/.local/state
endif
if (! ${?XDG_CACHE_HOME} && ${?HOME} ) then
  setenv XDG_CACHE_HOME $HOME/.cache
endif

# XDG_RUNTIME_DIR is set by pam_systemd



 
