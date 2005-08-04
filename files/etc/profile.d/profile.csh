#
# profile.csh:		 Set interactive profile environment
#
# Used configuration files:
#
#     /etc/sysconfig/windowmanager
#     /etc/sysconfig/suseconfig
#     /etc/sysconfig/mail
#     /etc/sysconfig/proxy
#     /etc/sysconfig/console
#

set noglob
set sysconf=""
foreach sys (/etc/sysconfig/windowmanager	\
	     /etc/sysconfig/suseconfig		\
	     /etc/sysconfig/mail		\
	     /etc/sysconfig/proxy		\
	     /etc/sysconfig/console)
    if (! -s ${sys:q} ) continue
    set sysconf="${sysconf} ${sys}"
end
unset sys

set val=""
foreach line ( "`grep -vh '^#' $sysconf`" )
    set val="${line:q:s/=/ /}"
    set arr=( $val )
    eval set val="${arr[2-]}"
    switch (${line:q})
    case CWD_IN_ROOT_PATH=*:
	if ( ${line:q} !~ *=*yes* ) continue
	if ( "$path[*]" =~ *.* )    continue
	if ( $uid <  100 ) set -l path=( $path . )
	breaksw
    case CWD_IN_USER_PATH=*:
	if ( ${line:q} !~ *=*yes* ) continue
	if ( "$path[*]" =~ *.* )    continue
	if ( $uid >= 100 ) set -l path=( $path . )
	breaksw
    case FROM_HEADER=*:
	setenv FROM_HEADER ${val:q}
	breaksw
    case SCANNER_TYPE=*:
	setenv SCANNER_TYPE ${val:q}
	breaksw
    case PROXY_ENABLED=*:
	set proxy_enabled=${val:q}
	breaksw
    case HTTP_PROXY=*:
	setenv http_proxy ${val:q}
	breaksw
    case HTTPS_PROXY=*:
	setenv https_proxy ${val:q}
	breaksw
    case FTP_PROXY=*:
	setenv ftp_proxy ${val:q}
	breaksw
    case GOPHER_PROXY=*:
	setenv gopher_proxy ${val:q}
	breaksw
    case NO_PROXY=*:
	setenv no_proxy ${val:q}
	breaksw
    case DEFAULT_WM=*:
	set default_wm=${val:q}
	breaksw
    case CONSOLE_MAGIC=*:
	set console_magic=${val:q}
	breaksw
    default:
	breaksw
    endsw
end
unset sysconf line

if ( -d /usr/lib/qt3 ) then
    setenv QTDIR /usr/lib/qt3
endif

if ( -d /usr/lib/dvgt_help ) then
    setenv DV_IMMED_HELP /usr/lib/dvgt_help
endif

if ( -d /usr/lib/rasmol ) then
    setenv RASMOLPATH /usr/lib/rasmol
endif

if ( ${?proxy_enabled} ) then
    if ( "$proxy_enabled" != "yes" ) then
	unsetenv http_proxy https_proxy ftp_proxy gopher_proxy no_proxy
    endif
    unset proxy_enabled
endif

if (! ${?default_wm} )     set default_wm=twm
if ( ${%default_wm} == 0 ) set default_wm=twm
set default_wm=${default_wm:t}

#
# Do not use the `which' builtin nor set path to avoid a rehash
#
foreach val ($path /usr/X11R6/bin /opt/gnome/bin /usr/openwin/bin)
    if ( ${val:q} =~ *.* ) continue
    set val=${val:q}/${default_wm:q}
    if ( ! -x ${val:q} ) continue
    setenv WINDOWMANAGER ${val:q}
    break
end
unset val default_wm

if ( ${?loginsh} && ${?console_magic} && "$tty" =~ tty* ) then
    if ( "$TERM" == "linux" && -o /dev/$tty ) then
	echo -n "\033$console_magic"
    endif
endif

unset noglob
#
# end of profile.csh
