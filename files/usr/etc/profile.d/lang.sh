#
# lang.sh:	Set interactive language environment
#
# Used configuration files:
#
#     /etc/sysconfig/language
#     $HOME/.i18n
#

unset _save

#
# Check if, e.g. already done on the remote SSH side
#
if test -n "$(locale 2>&1 1>/dev/null)"
then
    test -z "$LANG" || LANG=C.UTF-8
    unset LC_ALL
    for lc in LC_CTYPE LC_NUMERIC LC_TIME LC_COLLATE         \
              LC_MONETARY LC_MESSAGES LC_PAPER LC_NAME LC_ADDRESS  \
              LC_TELEPHONE LC_MEASUREMENT LC_IDENTIFICATION
    do
	eval unset $lc
    done
else
    test -n "$LANG" && _save="$LANG"
fi

#
# Already done by the GDM
#
if test -n "$GDM_LANG" ; then
    if test -s /etc/sysconfig/language ; then
	eval $(sed -rn -e 's/^(RC_LANG)=/_\1=/p' < /etc/sysconfig/language)
    fi
    if test -z "$_RC_LANG" -a -s /etc/locale.conf ; then
	eval $(sed -rn -e 's/^(LANG)=/_RC_\1=/p' < /etc/locale.conf)
    fi
    if test "$_RC_LANG" = "$GDM_LANG" ; then
	unset GDM_LANG
    else
	LANG=$GDM_LANG
    fi
    unset _RC_LANG
    test -n "$LANG" && _save="$LANG"
fi

#
# Get the system and after that the users configuration
# Note: ROOT_USES_LANG might be become overwritten for root
#
ROOT_USES_LANG=yes
if test -s /etc/locale.conf ; then
    while read line ; do
	case "$line" in
	\#*|"")
		continue
		;;
	L*)
		# Allow GDM to override system settings
		test -n "$GDM_LANG" && continue
		eval ${line}
	esac
    done < /etc/locale.conf
    unset line
fi
if test -s /etc/sysconfig/language ; then
    while read line ; do
	case "$line" in
	\#*|"")
		continue
		;;
	RC_*)
		# Allow GDM to override system settings
		test -n "$GDM_LANG" && continue
		eval val=${line##*=}
		test -z "$val" && continue
		eval ${line#RC_}
		;;
	ROOT_USES_LANG*)
		test "$UID" = 0 && eval $line
		;;
	esac
    done < /etc/sysconfig/language
    unset line val
fi

test -s $HOME/.i18n && . $HOME/.i18n

test -n "$_save" && LANG="$_save"
unset _save

#
# Handle all LC and the LANG variable
#
for lc in LANG LC_ADDRESS LC_ALL LC_COLLATE LC_CTYPE	\
	  LC_IDENTIFICATION LC_MEASUREMENT LC_MESSAGES	\
	  LC_MONETARY LC_NAME LC_NUMERIC LC_PAPER	\
	  LC_TELEPHONE LC_TIME
do
    eval val="\$$lc"
    if  test "$ROOT_USES_LANG" = "yes"    ; then
	if test -z "$val" ; then
	    eval unset $lc
	else
	    eval $lc=\$val
	    eval export $lc
	fi
    elif test "$ROOT_USES_LANG" = "ctype" ; then
	test "$lc" = "LANG" && continue
	if test "$lc" = "LC_CTYPE" ; then
	    LC_CTYPE=$LANG
	    LANG=POSIX
	    export LANG LC_CTYPE
	else
	    eval unset $lc
	fi
    else
	if test "$lc" = "LANG" ; then
	    LANG=C.UTF-8
	    export LANG
	else
	    eval unset $lc
	fi
    fi
done

unset lc val
unset ROOT_USES_LANG

#
# Special LC_ALL handling because the LC_ALL
# overwrites all LC but not the LANG variable
#
if test -n "$LC_ALL" -a "$LC_ALL" != "$LANG" ; then
    export LC_ALL
else
    unset LC_ALL
fi

#
# end of lang.sh
