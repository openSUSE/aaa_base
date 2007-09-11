for xdgdir in /usr/local/share /usr/share /etc/opt/*/share /opt/*/share /usr/share/gnome ; do
   if test -d $xdgdir && test -d $xdgdir/applications; then
      if test -z "$XDG_DATA_DIRS"; then
         XDG_DATA_DIRS="$xdgdir"
      else
         XDG_DATA_DIRS="$XDG_DATA_DIRS:$xdgdir"
      fi
   fi
done
export XDG_DATA_DIRS

for xdgdir in /usr/local/etc/xdg /etc/xdg /etc/opt/*/xdg ; do
   if test -d $xdgdir; then
      if test -z "XDG_CONFIG_DIRS"; then
         XDG_CONFIG_DIRS="$xdgdir"
      else
         XDG_CONFIG_DIRS="$XDG_CONFIG_DIRS:$xdgdir"
      fi
   fi
done
export XDG_CONFIG_DIRS

unset xdgdir
