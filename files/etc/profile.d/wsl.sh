# WSL does not utilitze this pam functionality currently.
if test -f /proc/version ; then
    IS_WSL=$(grep -i microsoft /proc/version)
fi

if test -n "$IS_WSL" ; then
    if test -n "$ORIG_PATH" ; then
	PATH=$ORIG_PATH:$PATH
    fi
    if test $(umask) -eq 0; then
	UMASK_LOGIN_DEFS=$(sed -ne 's/^UMASK[[:space:]]*//p' /etc/login.defs)
	test "$UMASK_LOGIN_DEFS" && umask "$UMASK_LOGIN_DEFS"
	unset UMASK_LOGIN_DEFS
    fi
fi
