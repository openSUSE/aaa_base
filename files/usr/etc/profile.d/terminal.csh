# fallback in case TERM is unknown
tput cols >& /dev/null
if ( $? != 0 ) then
    setenv TERM vt220
endif
