# keep in sync with /usr/lib/environment.d/50-xdg.conf
# System XDG - SUSE configured
XDG_CONFIG_DIRS=${XDG_CONFIG_DIRS:-/etc/xdg:/usr/local/etc/xdg:/usr/etc/xdg}
export XDG_CONFIG_DIRS

# System XDG - explicit defaults
XDG_DATA_DIRS=${XDG_DATA_DIRS:-/usr/local/share/:/usr/share/}
export XDG_DATA_DIRS

# User XDG - explicit defaults
XDG_DATA_HOME=${XDG_DATA_HOME:-${HOME:+$HOME/.local/share}}
export XDG_DATA_HOME
XDG_CONFIG_HOME=${XDG_CONFIG_HOME:-${HOME:+$HOME/.config}}
export XDG_CONFIG_HOME
XDG_STATE_HOME=${XDG_STATE_HOME:-${HOME:+$HOME/.local/state}}
export XDG_STATE_HOME
XDG_CACHE_HOME=${XDG_CACHE_HOME:-${HOME:+$HOME/.cache}}
export XDG_CACHE_HOME

# XDG_RUNTIME_DIR is set by pam_systemd


