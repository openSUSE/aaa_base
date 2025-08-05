#
# profile.sh:		 Set interactive profile environment
#
# Used configuration files:
#
#     /etc/sysconfig/windowmanager
#     /etc/sysconfig/mail
#     /etc/sysconfig/proxy
#     /etc/sysconfig/console
#     /etc/sysconfig/news
#

for _sys in /etc/sysconfig/windowmanager	\
	   /etc/sysconfig/mail		\
	   /etc/sysconfig/proxy		\
	   /etc/sysconfig/console	\
	   /etc/sysconfig/news
do
    test -s $_sys || continue
    while read _line ; do
	case "$_line" in
	\#*|"") continue ;;
        esac
	eval "_val=${_line#*=}"
	case "$_line" in
	CWD_IN_ROOT_PATH=*)
	    test "$_val" = "yes" || continue
	    test $UID -lt 100 && PATH=$PATH:.
	    ;;
	CWD_IN_USER_PATH=*)
	    test "$_val" = "yes" || continue
	    test $UID -ge 100 && PATH=$PATH:.
	    ;;
	FROM_HEADER=*)
	    FROM_HEADER="${_val}"
	    export FROM_HEADER
	    ;;
	PROXY_ENABLED=*)
	    PROXY_ENABLED="${_val}"
	    ;;
	HTTP_PROXY=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    http_proxy="${_val}"
	    export http_proxy
	    ;;
	HTTPS_PROXY=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    https_proxy="${_val}"
	    export https_proxy
	    ;;
	FTP_PROXY=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    ftp_proxy="${_val}"
	    export ftp_proxy
	    ;;
	GOPHER_PROXY=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    gopher_proxy="${_val}"
	    export gopher_proxy
	    ;;
	SOCKS_PROXY=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    socks_proxy="${_val}"
	    export socks_proxy
	    SOCKS_PROXY="${_val}"
	    export SOCKS_PROXY
	    ;;
	SOCKS5_SERVER=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    SOCKS5_SERVER="${_val}"
	    export SOCKS5_SERVER
	    ;;
	NO_PROXY=*)
	    test "$PROXY_ENABLED" = "yes" || continue
	    no_proxy="${_val}"
	    export no_proxy
	    NO_PROXY="${_val}"
	    export NO_PROXY
	    ;;
	DEFAULT_WM=*)
	    DEFAULT_WM="${_val}"
	    ;;
	CONSOLE_MAGIC=*)
	    CONSOLE_MAGIC="${_val}"
	    ;;
	ORGANIZATION=*)
	    test -n "$_val" || continue
	    ORGANIZATION="${_val}"
	    export ORGANIZATION
	    ;;
	NNTPSERVER=*)
	    NNTPSERVER="${_val}"
	    test -z "$NNTPSERVER" && NNTPSERVER=news
	    export NNTPSERVER
	esac
    done < $_sys
done
unset -v _sys _line _val PROXY_ENABLED

if test -d /usr/lib/dvgt_help ; then
    DV_IMMED_HELP=/usr/lib/dvgt_help
    export DV_IMMED_HELP
fi

if test -d /usr/lib/rasmol ; then
    RASMOLPATH=/usr/lib/rasmol
    export RASMOLPATH
fi

if test -z "$WINDOWMANAGER" ; then
    _SAVEPATH=$PATH
    PATH=$PATH:/usr/X11R6/bin:/usr/openwin/bin
    _desktop=/usr/share/xsessions/${DEFAULT_WM}.desktop
    if test -s "$_desktop" ; then
	while read -r _line; do
	    case ${_line} in
	    Exec=/usr/bin/env*|Exec=env*)
		    WINDOWMANAGER="${_line#Exec=}"
		    break
		    ;;
	    Exec=*) WINDOWMANAGER="$(command -v ${_line#Exec=})"
		    break
	    esac
	done < $_desktop
    fi
    if test -n "$DEFAULT_WM" -a -z "$WINDOWMANAGER" ; then
	WINDOWMANAGER="$(command -v ${DEFAULT_WM##*/})"
    fi
    PATH=$_SAVEPATH
    unset -v _SAVEPATH _desktop
    if test -z "$WINDOWMANAGER" ; then
	WINDOWMANAGER=xterm
    fi
fi
unset -v DEFAULT_WM _line
export WINDOWMANAGER

if test -n "$CONSOLE_MAGIC" ; then
    case "$(tty 2> /dev/null)" in
    /dev/tty*)
	if test "$TERM" = "linux" -a -t ; then
	    # Use /bin/echo due ksh can not do that
	    /usr/bin/echo -en "\033$CONSOLE_MAGIC"
	fi
    esac
fi
#
# end of profile.sh
