#
# Midnight Commander needs this to run in color mode
#
if ( ${?TERM} && ! ${?COLORTERM} ) then
    if ($TERM !~ rxvt.* && "`tput colors || echo 0`" > 8) then
	setenv COLORTERM truecolor
    endif
endif
