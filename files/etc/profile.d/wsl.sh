# WSL does not utilitze this pam functionality currently.
if test `umask` = 0000; then
    UMASK_LOGIN_DEFS=`sed -ne 's/^UMASK[[:space:]]*//p' /etc/login.defs`
    test "$UMASK_LOGIN_DEFS" && umask "$UMASK_LOGIN_DEFS"
    unset UMASK_LOGIN_DEFS
fi
