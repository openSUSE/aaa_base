#
# lang.csh:	Set interactive language environment
#
# Used configuration files:
#
#     /etc/sysconfig/language
#     $HOME/.i18n
#

#
# Already done by the remote SSH side
#
if ( ${?SSH_SENDS_LOCALE} ) goto end

#
# Already done by the GDM
#
if ( ${?GDM_LANG} ) then
    eval `sed -rn -e 's/^(RC_LANG)=/set _\1=/p' < /etc/sysconfig/language`
    if ( ${?_RC_LANG} ) then
	if ( "$_RC_LANG" == "$GDM_LANG" ) then
	    unsetenv GDM_LANG
	else
	    setenv LANG $GDM_LANG
	endif
	unset _RC_LANG
    endif
endif

unset _save
if ( ${?LANG} ) then
    set _save=$LANG
endif

#
# Get the system and after that the users configuration
#
if ( -s /etc/sysconfig/language ) then
    # Allow GDM to override system settings
    if ( ${?GDM_LANG} ) then
	if ( "$uid" == 0 ) then
	    eval `sed -rn -e 's/^(ROOT_USES_LANG)=/set \1=/p' < /etc/sysconfig/language`
	else
	    set ROOT_USES_LANG=yes
	endif
    else
	eval `sed -rn \
	    -e 's/^RC_((LANG|LC_[A-Z_]+))=/set \1=/p' -e 's/^(ROOT_USES_LANG)=/set \1=/p' \
	    < /etc/sysconfig/language`
    endif
endif
if ( -s $HOME/.i18n ) then
    eval `sed -rn -e 's/^((LANG|LC_[A-Z_]+))=/set \1=/p' < $HOME/.i18n`
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
    if  ( "$ROOT_USES_LANG" == "yes" ) then
	if ( ${%val} == 0 ) then
	    eval unsetenv $lc
	else
	    eval setenv $lc $val
	endif
    else if ( "$ROOT_USES_LANG" == "ctype" ) then
	if ( "$lc" == "LANG" ) continue
	if ( "$lc" == "LC_CTYPE" ) then
	    setenv LC_CTYPE $LANG
	    setenv LANG POSIX
	else
	    eval unsetenv $lc
	endif
    else
	if ( "$lc" == "LANG" ) then
	    setenv LANG POSIX
	else
	    eval unsetenv $lc
	endif
    endif
    eval unset $lc
end
unset lc val
unset ROOT_USES_LANG

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
# end of lang.sh
