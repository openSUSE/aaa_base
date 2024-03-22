# fallback in case TERM is unknown
if ( -x /usr/bin/tput ) then
    /usr/bin/tput cols >& /dev/null
    if ( $? != 0 ) then
	setenv TERM vt220
    endif
endif
