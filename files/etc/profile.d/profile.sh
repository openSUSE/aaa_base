#
# profile.sh:		 Set interactive profile environment
#
# Used configuration files:
#
#     /etc/sysconfig/windowmanager
#     /etc/sysconfig/suseconfig
#     /etc/sysconfig/mail
#     /etc/sysconfig/proxy
#     /etc/sysconfig/console
#     /etc/sysconfig/news
#

for sys in /etc/sysconfig/windowmanager	\
	   /etc/sysconfig/suseconfig	\
	   /etc/sysconfig/mail		\
	   /etc/sysconfig/proxy		\
	   /etc/sysconfig/console	\
	   /etc/sysconfig/news
do
    test -s $sys || continue
    while read line ; do
	case "$line" in
	\#*|"") continue ;;
        esac
	eval val=${line#*=}
	case "$line" in
	CWD_IN_ROOT_PATH=*)
	    test "$val" = "yes" || continue
	    test $UID -lt 100 && PATH=$PATH:.
	    ;;
	CWD_IN_USER_PATH=*)
	    test "$val" = "yes" || continue
	    test $UID -ge 100 && PATH=$PATH:.
	    ;;
	FROM_HEADER=*)
	    FROM_HEADER="${val}"
	    export FROM_HEADER
	    ;;
	SCANNER_TYPE=*)
	    SCANNER_TYPE="${val}"
	    export SCANNER_TYPE
	    ;;
	PROXY_ENABLED=*)
	    PROXY_ENABLED="${val}"
	    ;;
	HTTP_PROXY=*)
	    test -n "$val" || continue
	    http_proxy="${val}"
	    export http_proxy
	    ;;
	HTTPS_PROXY=*)
	    test -n "$val" || continue
	    https_proxy="${val}"
	    export https_proxy
	    ;;
	FTP_PROXY=*)
	    test -n "$val" || continue
	    ftp_proxy="${val}"
	    export ftp_proxy
	    ;;
	GOPHER_PROXY=*)
	    test -n "$val" || continue
	    gopher_proxy="${val}"
	    export gopher_proxy
	    ;;
	NO_PROXY=*)
	    test -n "$val" || continue
	    no_proxy="${val}"
	    export no_proxy
	    ;;
	DEFAULT_WM=*)
	    DEFAULT_WM="${val}"
	    ;;
	CONSOLE_MAGIC=*)
	    CONSOLE_MAGIC="${val}"
	    ;;
	ORGANIZATION=*)
	    test -n "$val" || continue
	    ORGANIZATION="${val}"
	    export ORGANIZATION
	    ;;
	NNTPSERVER=*)
	    NNTPSERVER="${val}"
	    test -z "$NNTPSERVER" && NNTPSERVER=news
	    export NNTPSERVER
	esac
    done < $sys
done
unset sys line val

if test -d /usr/lib/dvgt_help ; then
    DV_IMMED_HELP=/usr/lib/dvgt_help
    export DV_IMMED_HELP
fi

if test -d /usr/lib/rasmol ; then
    RASMOLPATH=/usr/lib/rasmol
    export RASMOLPATH
fi

if test "$PROXY_ENABLED" != "yes" ; then
    unset http_proxy https_proxy ftp_proxy gopher_proxy no_proxy
fi
unset PROXY_ENABLED

if test -n "$DEFAULT_WM" -a -z "$WINDOWMANAGER" ; then
    SAVEPATH=$PATH
    PATH=$PATH:/usr/X11R6/bin:/opt/gnome/bin:/usr/openwin/bin
    WINDOWMANAGER="`type -p ${DEFAULT_WM##*/}`"
    PATH=$SAVEPATH
    export WINDOWMANAGER
    unset SAVEPATH
fi
unset DEFAULT_WM

if test -n "$CONSOLE_MAGIC" ; then
    case "$(tty 2> /dev/null)" in
    /dev/tty*)
	if test "$TERM" = "linux" -a -t ; then
	    # Use /bin/echo due ksh can not do that
	    /bin/echo -en "\033$CONSOLE_MAGIC"
	fi
    esac
fi
#
# end of profile.sh
