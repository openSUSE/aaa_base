#
# Midnight Commander needs this to run in color mode
#
if ( ${?TERM} ) then
    if ( "`tput colors || echo 0`" > 8) then
	setenv COLORTERM truecolor
    endif
endif
