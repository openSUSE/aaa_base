# /etc/bash.bashrc for SuSE Linux
#
# PLEASE DO NOT CHANGE /etc/bash.bashrc There are chances that your changes
# will be lost during system upgrades.  Instead use /etc/bash.bashrc.local
# for bash or /etc/ksh.kshrc.local for ksh or /etc/zsh.zshrc.local for the
# zsh or /etc/ash.ashrc.local for the plain ash bourne shell  for your local
# settings, favourite global aliases, VISUAL and EDITOR variables, etc ...

#
# Check which shell is reading this file
#
if test -z "$is" ; then
 if test -f /proc/mounts ; then
  if ! is=$(readlink /proc/$$/exe 2>/dev/null) ; then
    case "$0" in
    *pcksh)	is=ksh	;;
    *)		is=sh	;;
    esac
  fi
  case "$is" in
    */bash)	is=bash
	case "$0" in
	sh|-sh|*/sh)
		is=sh	;;
	esac		;;
    */ash)	is=ash  ;;
    */dash)	is=ash  ;;
    */ksh)	is=ksh  ;;
    */ksh93)	is=ksh  ;;
    */pdksh)	is=ksh  ;;
    */*pcksh)	is=ksh  ;;
    */zsh)	is=zsh  ;;
    */*)	is=sh   ;;
  esac
  #
  # `r' in $- occurs *after* system files are parsed
  #
  for a in $SHELL ; do
    case "$a" in
      */r*sh)
        readonly restricted=true ;;
      -r*|-[!-]r*|-[!-][!-]r*)
        readonly restricted=true ;;
      --restricted)
        readonly restricted=true ;;
    esac
  done
  unset a
 else
  is=sh
 fi
fi

#
# Call common progams from /bin or /usr/bin only
#
path ()
{
    if test -x /usr/bin/$1 ; then
	${1+"/usr/bin/$@"}
    elif test -x   /bin/$1 ; then
	${1+"/bin/$@"}
    fi
}


#
# ksh/ash sometimes do not know
#
test -z "$UID"  && readonly  UID=`path id -ur 2> /dev/null`
test -z "$EUID" && readonly EUID=`path id -u  2> /dev/null`

test -s /etc/profile.d/ls.bash && . /etc/profile.d/ls.bash

#
# Avoid trouble with Emacs shell mode
#
if test "$EMACS" = "t" ; then
    path tset -I -Q
    path stty cooked pass8 dec nl -echo
fi

#
# Set prompt and aliases to something useful for an interactive shell
#
case "$-" in
*i*)
    #
    # Set prompt to something useful
    #
    case "$is" in
    bash)
	# If COLUMNS are within the environment the shell should update
	# the winsize after each job otherwise the values are wrong
	case "$(declare -p COLUMNS 2> /dev/null)" in
	*-x*COLUMNS=*) shopt -s checkwinsize
	esac
	# Append history list instead of override
	shopt -s histappend
	# All commands of root will have a time stamp
	if test "$UID" -eq 0  ; then
	    HISTTIMEFORMAT=${HISTTIMEFORMAT:-"%F %H:%M:%S "}
	fi
	# Force a reset of the readline library
	unset TERMCAP
	#
	# Returns short path (last two directories)
	#
	spwd () {
	  ( IFS=/
	    set $PWD
	    if test $# -le 3 ; then
		echo "$PWD"
	    else
		eval echo \"..\${$(($#-1))}/\${$#}\"
	    fi ) ; }
	#
	# Set xterm prompt with short path (last 18 characters)
	#
	if path tput hs 2>/dev/null || path tput -T $TERM+sl hs 2>/dev/null ; then
	    #
	    # Mirror prompt in terminal "status line", which for graphical
	    # terminals usually is the window title. KDE konsole in
	    # addition needs to have "%w" in the "tabs" setting, ymmv for
	    # other console emulators.
	    #
	    if test "$TERM" = xterm ; then
		_tsl=$(echo -en '\e]2;')
		_isl=$(echo -en '\e]1;')
		_fsl=$(echo -en '\007')
	    else
		_tsl=$(path tput tsl 2>/dev/null || path tput -T $TERM+sl tsl 2>/dev/null)
		_isl=''
		_fsl=$(path tput fsl 2>/dev/null || path tput -T $TERM+sl fsl 2>/dev/null)
	    fi
	    _sc=$(tput sc 2>/dev/null)
	    _rc=$(tput rc 2>/dev/null)
	    if test -n "$_tsl" -a -n "$_isl" -a "$_fsl" ; then
		TS1="$_sc$_tsl%s@%s:%s$_fsl$_isl%s$_fsl$_rc"
	    elif test -n "$_tsl" -a "$_fsl" ; then
		TS1="$_sc$_tsl%s@%s:%s$_fsl$_rc"
	    fi
	    unset _tsl _fsl _sc _rc
	    ppwd () {
		local dir
		local -i width
		test -n "$TS1" || return;
		dir="$(dirs +0)"
		let width=${#dir}-18
		test ${#dir} -le 18 || dir="...${dir#$(printf "%.*s" $width "$dir")}"
		if test ${#TS1} -gt 17 ; then
		    printf "$TS1" "$USER" "$HOST" "$dir" "$HOST"
		else
		    printf "$TS1" "$USER" "$HOST" "$dir"
		fi
	    }
	else
	    ppwd () { true; }
	fi
	# If set: do not follow sym links
	# set -P
	#
	# Other prompting for root
	if test "$UID" -eq 0  ; then
	    if test -n "$TERM" -a -t ; then
	    	_bred="$(path tput bold 2> /dev/null; path tput setaf 1 2> /dev/null)"
	    	_sgr0="$(path tput sgr0 2> /dev/null)"
	    fi
	    # Colored root prompt (see bugzilla #144620)
	    if test -n "$_bred" -a -n "$_sgr0" ; then
		_u="\[$_bred\]\h"
		_p=" #\[$_sgr0\]"
	    else
		_u="\h"
		_p=" #"
	    fi
	    unset _bred _sgr0
	else
	    _u="\u@\h"
	    _p=">"
	fi
	if test -z "$EMACS" -a -z "$MC_SID" -a -z "$restricted" -a \
		-n "$DISPLAY" -a ! -r $HOME/.bash.expert
	then
	    _t="\[\$(ppwd)\]"
	else
	    _t=""
	fi
	case "$(declare -p PS1 2> /dev/null)" in
	*-x*PS1=*)
	    ;;
	*)
	    # With full path on prompt
	    PS1="${_t}${_u}:\w${_p} "
#	    # With short path on prompt
#	    PS1="${_t}${_u}:\$(spwd)${_p} "
#	    # With physical path even if reached over sym link
#	    PS1="${_t}${_u}:\$(pwd -P)${_p} "
	    ;;
	esac
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
	# Some users of the ksh are not common with the usage of PS1.
	# This variable should not be exported, because normally only
	# interactive shells set this variable by default to ``$ ''.
	if test "${PS1-\$ }" = '$ ' ; then
	    if test "$UID" = 0 ; then
		PS1="${HOST}:"'${PWD}'" # "
	    else
		PS1="${USER}@${HOST}:"'${PWD}'"> "
	    fi
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
#	PS1='\u:\w> '
	PS1='\h:\w> '
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
	test -s /etc/profile.d/alias.bash && . /etc/profile.d/alias.bash
	test -s $HOME/.alias && . $HOME/.alias
    fi

    #
    # Expert mode: if we find $HOME/.bash.expert we skip our settings
    # used for interactive completion and read in the expert file.
    #
    if test "$is" = "bash" -a -r $HOME/.bash.expert ; then
	. $HOME/.bash.expert
    elif test "$is" = "bash" ; then
	# Complete builtin of the bash 2.0 and higher
	case "$BASH_VERSION" in
	[2-9].*)
	    if test -e /etc/bash_completion ; then
		. /etc/bash_completion
	    elif test -s /etc/profile.d/bash_completion.sh ; then
		. /etc/profile.d/bash_completion.sh
	    elif test -s /etc/profile.d/complete.bash ; then
		. /etc/profile.d/complete.bash
	    fi
	    for s in /etc/bash_completion.d/*.sh ; do
		test -r $s && . $s
	    done
	    if test -e $HOME/.bash_completion ; then
		. $HOME/.bash_completion
	    fi
	    if test -f /etc/bash_command_not_found ; then
		. /etc/bash_command_not_found
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
    	: ${HISTFILE=$HOME/.kshrc_history}
    	: ${VISUAL=emacs}
	case $(set -o) in
	*multiline*) set -o multiline
	esac
    fi
    # command not found handler in zsh version
    if test "$is" = "zsh" ; then
	if test -f /etc/zsh_command_not_found ; then
	    . /etc/zsh_command_not_found
	fi
    fi
    ;;
esac

#
# Just in case the user excutes a command with ssh or sudo
#
if test \( -n "$SSH_CONNECTION" -o -n "$SUDO_COMMAND" \) -a -z "$PROFILEREAD" ; then
    _SOURCED_FOR_SSH=true
    . /etc/profile > /dev/null 2>&1
    unset _SOURCED_FOR_SSH
fi

#
# Set GPG_TTY for curses pinentry
# (see man gpg-agent and bnc#619295)
#
if test -t && type -p tty > /dev/null 2>&1 ; then
    GPG_TTY="`tty`"
    export GPG_TTY
fi

#
# And now let us see if there is e.g. a local bash.bashrc
# (for options defined by your sysadmin, not SuSE Linux)
#
case "$is" in
bash) test -s /etc/bash.bashrc.local && . /etc/bash.bashrc.local ;;
ksh)  test -s /etc/ksh.kshrc.local   && . /etc/ksh.kshrc.local ;;
zsh)  test -s /etc/zsh.zshrc.local   && . /etc/zsh.zshrc.local ;;
ash)  test -s /etc/ash.ashrc.local   && . /etc/ash.ashrc.local
esac
test -s /etc/sh.shrc.local && . /etc/sh.shrc.local

if test -n "$restricted" -a -z "$PROFILEREAD" ; then
    PATH=/usr/lib/restricted/bin
    export PATH
fi
#
# End of /etc/bash.bashrc
#
