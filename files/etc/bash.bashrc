# /etc/bash.bashrc for SuSE Linux
#
# PLEASE DO NOT CHANGE /etc/bash.bashrc There are chances that your changes
# will be lost during system upgrades.  Instead use /etc/bash.bashrc.local
# for your local settings, favourite global aliases, VISUAL and EDITOR
# variables, etc ...

#
# Check which shell is reading this file
#
if test -z "$is" ; then
 if test -f /proc/mounts ; then
  case "`/bin/ls -l /proc/$$/exe`" in
    */bash)	is=bash ;;
    */rbash)	is=bash ;;
    */ash)	is=ash  ;;
    */ksh)	is=ksh  ;;
    */zsh)	is=zsh  ;;
    */*)	is=sh   ;;
  esac
 else
  is=sh
 fi
fi

#
# Colored file listings
#
if test -x /usr/bin/dircolors ; then
    #
    # set up the color-ls environment variables:
    #
    if test -f $HOME/.dir_colors ; then
	eval `dircolors -b $HOME/.dir_colors`
    elif test -f /etc/DIR_COLORS ; then
	eval `dircolors -b /etc/DIR_COLORS`
    fi
fi

#
# ksh/ash soemtimes do not know
#
test -z "$UID"  &&  UID=`id -ur 2> /dev/null`
test -z "$EUID" && EUID=`id -u  2> /dev/null`
test -z "$USER" && USER=`id -un 2> /dev/null`
test -z "$MAIL" && MAIL=/var/spool/mail/$USER
test -z "$LOGNAME"  && LOGNAME=$USER

#
# ls color option depends on the terminal
# If LS_COLROS is set but empty, the terminal has no colors.
#
if test "${LS_COLORS+empty}" = "${LS_COLORS:+empty}" ; then
    LS_OPTIONS=--color=tty
else
    LS_OPTIONS=--color=none
fi
if test "$UID" = 0 ; then
    LS_OPTIONS="-a -N $LS_OPTIONS -T 0"
else
    LS_OPTIONS="-N $LS_OPTIONS -T 0"
fi

#
# Avoid trouble with Emacs shell mode
#
if test "$EMACS" = "t" ; then
    LS_OPTIONS='-N --color=none -T 0';
    tset -I -Q
    stty cooked pass8 dec nl -echo
fi
export LS_OPTIONS

#
# Set prompt and aliases to something useful for an interactive shell
#
case "$-" in
*i*)
    #
    # Some useful functions
    #
    startx  () { exec /usr/X11R6/bin/startx ${1+"$@"} 2>&1 | tee $HOME/.X.err ; }
    remount () { /bin/mount -o remount,${1+"$@"} ; }

    #
    # Set prompt to something useful
    #
    case "$is" in
    bash)
	# Returns short path (last two directoeries)
	spwd () {
	  ( IFS=/
	    set $PWD
	    if test $# -le 3 ; then
		echo "$PWD"
	    else
		eval echo \"..\${$(($#-1))}/\${$#}\"
	    fi ) ; }
	# Returns short path (last 18 characters)
	ppwd () {
	    local _w="$(dirs +0)"
	    if test ${#_w} -le 18 ; then
		echo "$_w"
	    else
		echo "...${_w:$((${#_w}-18))}"
	    fi ; }
	# If set: do not follow sym links
	# set -P
	#
	# Other prompting for root
	_t=""
	if test "$UID" = 0 ; then
	    _u="\h"
	    _p=" #"
	else
	    _u="\u@\h"
	    _p=">"
	    if test \( "$TERM" = "xterm" -o "${TERM#screen}" != "$TERM" \) -a -z "$EMACS" ; then
		_t="\[\e]2;\u@\h:\$(ppwd)\007\e]1;\h\007\]"
	    fi
	fi
	case $(locale charmap) in
		UTF-8)
		    _t=""
		;;
		*)
		;;
	esac
	# With full path on prompt
	PS1="${_t}${_u}:\w${_p} "
#	# With short path on prompt
#	PS1="${_t}${_u}:\$(spwd)${_p} "
#	# With physical path even if reached over sym link
#	PS1="${_t}${_u}:\$(pwd -P)${_p} "
	unset _u _p _t
	;;
    ash)
	cd () {
	    local ret
	    command cd "$@"
	    ret=$?
	    PWD=$(pwd)
	    if test "$UID" = 0 ; then
		PS1="${HOST}:${PWD} # "
	    else
		PS1="${USER}@${HOST}:${PWD}> "
	    fi
	    return $ret
	}
	cd .
	;;
    ksh)
	if test "$UID" = 0 ; then
	    PS1="${HOST}:"'${PWD}'" # "
	else
	    PS1="${USER}@${HOST}:"'${PWD}'"> "
	fi
	;;
    zsh)
#	setopt chaselinks
	if test "$UID" = 0; then
	    PS1='%n@%m:%~ # '
	else
	    PS1='%n@%m:%~> '
	fi
	;;
    *)
#	PS1='\u:\w\$ '
	PS1='\h:\w\$ '
	;;
    esac
    PS2='> '

    if test "$is" = "ash" ; then
	# The ash shell does not have an alias builtin in
	# therefore we use functions here. This is a seperate
	# file because other shells may run into trouble
	# if they parse this even if they do not expand.
	test -s /etc/profile.d/alias.ash && . /etc/profile.d/alias.ash
    else
	unalias ls 2>/dev/null
	if test "$is" = "zsh" ; then
	    alias ls='eval /bin/ls $LS_OPTIONS'
	else
	    alias ls='/bin/ls $LS_OPTIONS'
	fi
	alias dir='ls -l'
	alias ll='ls -l'
	alias la='ls -la'
	alias l='ls -alF'
	alias ls-l='ls -l'

	#
	# Set some generic aliases
	#
	alias o='less'
	alias ..='cd ..'
	alias ...='cd ../..'
	if test "$is" != "ksh" ; then
	    alias -- +='pushd .'
	    alias -- -='popd'
	fi
	alias rd=rmdir
	alias md='mkdir -p'
	alias which='type -p'
	alias rehash='hash -r'
	alias you='yast2 online_update'
	if test "$is" != "ksh" ; then
	    alias beep='echo -en "\007"' 
	else
	    alias beep='echo -en "\x07"'
	fi
	alias unmount='echo "Error: Try the command: umount" 1>&2; false'
	test -s $HOME/.alias && . $HOME/.alias
    fi

    # Complete builtin of the bash 2.0 and higher
    if test "$is" = "bash" ; then
	case "$BASH_VERSION" in
	[2-9].*)
	    if test -e $HOME/.bash_completion ; then
		. $HOME/.bash_completion
	    elif test -e /etc/bash_completion ; then
		. /etc/bash_completion
	    elif test -s /etc/profile.d/complete.bash ; then
		. /etc/profile.d/complete.bash
	    fi

	    ;;
	*)  ;;
	esac
    fi

    # Do not save dupes and lines starting by space in the bash history file
    HISTCONTROL=ignoreboth
    if test "$is" = "ksh" ; then
	# Use a ksh specific history file and enable
    	# emacs line editor
    	HISTFILE=$HOME/.kshrc_history
    	VISUAL=emacs
    fi
    ;;
esac

if test "$is" != "ash" ; then
    #
    # And now let's see if there is a local bash.bashrc
    # (for options defined by your sysadmin, not SuSE Linux)
    #
    test -s /etc/bash.bashrc.local && . /etc/bash.bashrc.local
fi

#
# End of /etc/bash.bashrc
#
