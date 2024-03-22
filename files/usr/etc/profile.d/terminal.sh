# fallback in case TERM is unknown
if [ -x /usr/bin/tput ] && ! /usr/bin/tput cols >/dev/null 2>&1; then
    TERM=vt220
fi
