# restore WSL path and set umask as WSL doesn't use pam to open a login shell
__profile_setup_wsl() {
    test -n "$WSL_DISTRO_NAME" || return 0

    if test -n "$ORIG_PATH" ; then
	PATH=$ORIG_PATH:$PATH
    fi

    if test $(umask) -eq 0000; then
	local logindefs
	for logindefs in /etc/login.defs /usr/etc/login.defs; do
	    test -e "$logindefs" || continue
	    break
	done
	if test -e "$logindefs"; then
	    local umask_login_defs=`sed -ne 's/^UMASK[[:space:]]*//p' "$logindefs"`
	    if test -n "$umask_login_defs"; then
		umask "$umask_login_defs"
	    fi
	fi
    fi
}

__profile_setup_wsl
unset __profile_setup_wsl
