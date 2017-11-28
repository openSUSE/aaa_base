# WSL does not utilitze this pam functionality currently.
set is_wsl=0
if ( -f /proc/version ) set is_wsl=`grep -ic microsoft /proc/version`

if ( $is_wsl == 1 ) then
    if ( ${?orig_path} ) then
	set -f path=($orig_path $path)
    endif
    if (`umask` == 0) then
	set umask_login_defs=`sed -ne 's/^UMASK[[:space:]]*//p' /etc/login.defs`
	if ( $umask_login_defs ) umask $umask_login_defs
	unset umask_login_defs
    endif
endif
