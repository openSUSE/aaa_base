#
# Midnight Commander needs this to run in color mode
#
if test -n "${TERM}" -a "${TERM%-*}" != rxvt -a -z "${COLORTERM}"; then
    if test "$(tput colors 2>/dev/null || echo 0)" -gt 8; then
	: ${COLORTERM=truecolor}
	export COLORTERM
    fi
fi
