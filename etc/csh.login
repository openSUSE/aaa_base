#
# System csh.login for tcsh,
# (c) Werner Fink '93
#
if ( -o /dev/$tty && ${?prompt} ) then
    # Console
    if ( ! ${?TERM} )           setenv TERM linux
    if ( "$TERM" == "unknown" ) setenv TERM linux
    # No tset available on SlackWare
    if ( -x "`which stty`" ) stty sane cr0 pass8 dec
    if ( -x "`which tset`" ) tset -I -Q
    unsetenv TERMCAP
    settc km yes
endif
umask 022
setenv SHELL /bin/tcsh
if ( -f /var/spool/mail/$USER ) then
    setenv MAIL /var/spool/mail/$USER
    set mail=$MAIL
endif

set _xpath
set _tpath=( /opt/bin /usr/andrew/bin \
	     /usr/lib/java/bin /usr/games )

if ( ${?OPENWINHOME} && -d $OPENWINHOME/bin ) then
    set _xpath=( $OPENWINHOME/bin )
endif
if ( -d /usr/bin/X11 ) then
    set _xpath=( /usr/bin/X11   $_xpath )
else if ( -d /usr/X11R6/bin ) then
    set _xpath=( /usr/X11R6/bin $_xpath )
else if ( -d /usr/X11/bin ) then
    set _xpath=( /usr/X11/bin   $_xpath )
endif

set _tpath=( $_tpath $_xpath ~/bin )

#
# Doing only one rehash
#
foreach _d ($_tpath)
    if ( -d $_d ) set path=( $_d $path )
end

unset _tpath
unset _xpath

#
# SuSEconfig stuff
#
if ( -r /etc/SuSEconfig/csh.login ) then
    source /etc/SuSEconfig/csh.login
endif

if ( "$uid" == "0" ) then
    set who=( "%n has %a %l from %M." )
    set watch=( any any )
endif
#
# Go home
cd
#
# An x session
if (${?WINDOWID}) then
    if ( -f /etc/motd ) cat /etc/motd
    echo "Directory: $cwd"
    #
    # shadow passwd
    # Note: on normal console this will be done
    #       by /bin/login
    if ( -x "`which faillog`" && -r /var/log/faillog ) faillog -p
endif
#
# Do you really like this?
#if (-x "`which fortune`" ) then
#   echo " "
#   fortune -s
#   echo " "
#endif
#
# last but not least
date
##
