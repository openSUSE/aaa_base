#
# lang.csh:	Set interactive language environment
#
# Used configuration files:
#
#     /etc/locale.conf
#     $HOME/.i18n
#

unset _save

#
# Check if, e.g. already done on the remote SSH side
#
set _locale_error=`(locale > /dev/null) |& cat`
if (${%_locale_error} > 0) then
    if (${?LANG}) setenv LANG C.UTF-8
    unsetenv LC_ALL
    foreach lc (LC_ADDRESS LC_COLLATE LC_CTYPE    \
		LC_IDENTIFICATION LC_MEASUREMENT LC_MESSAGES  \
		LC_MONETARY LC_NAME LC_NUMERIC LC_PAPER       \
		LC_TELEPHONE LC_TIME )
	eval unsetenv $lc
    end
else
    if (${?LANG})then
	set _save=$LANG
    endif
endif
unset _locale_error

#
# Already done by the GDM
#
if ( ${?GDM_LANG} ) then
    if ( ! ${?_RC_LANG} && -s /etc/locale.conf ) then
	eval `sed -rn -e 's/^(LANG)=/set _RC_\1=/p' < /etc/locale.conf`
    endif
    if ( ${?_RC_LANG} ) then
	if ( "$_RC_LANG" == "$GDM_LANG" ) then
	    unsetenv GDM_LANG
	else
	    setenv LANG $GDM_LANG
	endif
	unset _RC_LANG
    endif
    if ( ${?LANG} ) set _save=$LANG
endif

#
# Get the system and after that the users configuration
#
if ( -s /etc/locale.conf) then
    foreach line ("`sed -rn '/^[^#]/p' < /etc/locale.conf`")
	switch ("$line")
	case L*:
	    # Allow GDM to override system settings
	    if ( ${?GDM_LANG} ) continue
	    eval set ${line}
	    breaksw
	default:
	    breaksw
	endsw
    end
    unset line
endif
if ( -s $HOME/.i18n ) then
    eval `sed -rn -e 's/^((export[[:space:]]+)?(LANG|LC_[A-Z_]+|INPUT_METHOD|_save))=/set \3=/p' < $HOME/.i18n`
    if ( ${?INPUT_METHOD} ) then
	setenv INPUT_METHOD $INPUT_METHOD
	unset INPUT_METHOD
    endif
endif
if ( ${?_save} ) then
    set LANG=$_save
    unset _save
endif

#
# Handle all LC and the LANG variable
#
foreach lc (LANG LC_ADDRESS LC_ALL LC_COLLATE LC_CTYPE    \
	    LC_IDENTIFICATION LC_MEASUREMENT LC_MESSAGES  \
	    LC_MONETARY LC_NAME LC_NUMERIC LC_PAPER       \
	    LC_TELEPHONE LC_TIME )
    eval set val=\${\?$lc}
    if ( $val == 0 ) continue
    eval set val=\$$lc
    if ( ${%val} == 0 ) then
	eval unsetenv $lc
    else
	eval setenv $lc $val
    endif
end
unset lc val

#
# Special LC_ALL handling because the LC_ALL
# overwrites all LC but not the LANG variable
#
if ( ${?LC_ALL} ) then
    set LC_ALL=$LC_ALL
    if ( ${%LC_ALL} > 0 && "$LC_ALL" != "$LANG" ) then
	setenv LC_ALL $LC_ALL
    else
	unsetenv LC_ALL
    endif
    unset LC_ALL
endif

end:

#
# end of lang.csh
