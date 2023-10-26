# fallback in case TERM is unknown
if ! tput cols >/dev/null 2>&1; then
    TERM=vt220
fi
