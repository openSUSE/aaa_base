# WSL does not utilitze this pam functionality currently.
set is_wsl=0
if ( -f /proc/version ) set is_wsl=`grep -ic microsoft /proc/version`

if ( $is_wsl == 1 ) then
    if ( ${?orig_path} ) then
	set -f path=($orig_path $path)
    endif
    if (`umask` == 0) then
	foreach logindefs ({,/usr}/etc/login.defs)
	    if ( ! -e $logindefs ) continue
	    break
	end
	if ( -e $logindefs ) then
	   set _umask_login_defs=`sed -ne 's/^UMASK[[:space:]]*//p' "$logindefs"`
	   if ( ${%_umask_login_defs} > 0) then
	       umask ${_umask_login_defs}
	   endif
	   unset _umask_login_defs
	endif
    endif
endif
