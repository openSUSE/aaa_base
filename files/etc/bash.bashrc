# /etc/bash.bashrc for SUSE Linux
#
# PLEASE DO NOT CHANGE /etc/bash.bashrc There are chances that your changes
# will be lost during system upgrades.  Instead use /etc/bash.bashrc.local
# for bash or /etc/ksh.kshrc.local for ksh or /etc/zsh.zshrc.local for the
# zsh or /etc/ash.ashrc.local for the plain ash bourne shell  for your local
# settings, favourite global aliases, VISUAL and EDITOR variables, etc ...

#
# Check which shell is reading this file
# check if variables are read-only before setting them
# for example in a restricted shell
if unset noprofile 2>/dev/null ; then
  noprofile=false
fi
if unset restricted 2>/dev/null ; then
  restricted=false
fi
: ${_is_save:=unset}
if test -z "$is" ; then
 if test -f /proc/mounts ; then
  if ! is=$(readlink /proc/$$/exe 2>/dev/null) ; then
    case "$0" in
    *pcksh)	is=ksh	;;
    *bash)	is=bash	;;
    *)		is=sh	;;
    esac
  fi
  case "$is" in
    */bash)	is=bash
	while read -r -d $'\0' a ; do
	    case "$a" in
	    --noprofile)
		readonly noprofile=true ;;
	    --restricted)
		readonly restricted=true ;;
	    esac
	done < /proc/$$/cmdline
	case "$0" in
	sh|-sh|*/sh)
		is=sh	;;
	esac		;;
    */ash)	is=ash  ;;
    */dash)	is=ash  ;;
    */ksh)	is=ksh  ;;
    */ksh93)	is=ksh  ;;
    */pdksh)	is=ksh  ;;
    */mksh)	is=ksh  ;;
    */lksh)	is=ksh  ;;
    */*pcksh)	is=ksh  ;;
    */zsh)	is=zsh  ;;
    */*)	is=sh   ;;
  esac
  #
  # `r' in $- occurs *after* system files are parsed
  #
  for a in $SHELL ; do
    case "$a" in
      */rootsh) ;;
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
_path ()
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
test -z "$UID"  && readonly  UID=`_path id -ur 2> /dev/null`
test -z "$EUID" && readonly EUID=`_path id -u  2> /dev/null`

if test -s /etc/profile.d/ls.bash
then . /etc/profile.d/ls.bash
elif test -s /usr/etc/profile.d/ls.bash
then . /usr/etc/profile.d/ls.bash
fi

if test -s /etc/profile.d/mc.sh
then . /etc/profile.d/mc.sh
elif test -s /usr/share/mc/mc.sh
then . /usr/share/mc/mc.sh
fi

#
# Avoid trouble with Emacs shell mode
#
if test "$EMACS" = "t" ; then
    _path tset -I -Q
    _path stty cooked pass8 dec nl -echo
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
	    : ${HISTTIMEFORMAT="%F %H:%M:%S "}
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
	        shift $(($#-2))
		echo "..$1/$2"
	    fi ) ; }
	#
	# Set xterm prompt with short path (last 18 characters)
	#
	if _path tput hs 2>/dev/null || _path tput -T $TERM+sl hs 2>/dev/null || \
	   _path tput -T ${TERM%%[.-]*}+sl hs 2>/dev/null || \
	   [[ $TERM = *xterm* || $TERM = *gnome* || $TERM = *konsole* || $TERM = *xfce* || $TERM = *foot* ]]
	then
	    #
	    # Mirror prompt in terminal "status line", which for graphical
	    # terminals usually is the window title. KDE konsole in
	    # addition needs to have "%w" in the "tabs" setting, ymmv for
	    # other console emulators.
	    #
	    if [[ $TERM = *xterm* || $TERM = *gnome* || $TERM = *konsole* || $TERM = *xfce* ]]
	    then
		# https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Miscellaneous
		_tsl=$(echo -en '\e]2;')
		_isl=$(echo -en '\e]1;')
		_fsl=$(echo -en '\007')
	    elif _path tput -T $TERM+sl tsl 2>/dev/null ; then
		_tsl=$(_path tput -T $TERM+sl tsl 2>/dev/null)
		_isl=''
		_fsl=$(_path tput -T $TERM+sl fsl 2>/dev/null)
	    elif _path tput -T ${TERM%%[.-]*}+sl tsl 2>/dev/null ; then
		_tsl=$(_path tput -T $TERM+sl tsl 2>/dev/null)
		_isl=''
		_fsl=$(_path tput -T $TERM+sl fsl 2>/dev/null)
	    else
		_tsl=$(_path tput tsl 2>/dev/null)
		_isl=''
		_fsl=$(_path tput fsl 2>/dev/null)
	    fi
	    if [[ $TERM = *foot* ]]
	    then
		# Do not save cursor during writing status line for "foot" nor restore it
		_sc=''
		_rc=''
	    else
		_sc=$(tput sc 2>/dev/null)
	    	_rc=$(tput rc 2>/dev/null)
	    fi
	    if test -n "$_tsl" -a -n "$_isl" -a "$_fsl" ; then
		TS1="$_sc$_tsl%s@%s:%s$_fsl$_isl%s$_fsl$_rc"
	    elif test -n "$_tsl" -a "$_fsl" ; then
		TS1="$_sc$_tsl%s@%s:%s$_fsl$_rc"
	    fi
	    unset _isl _tsl _fsl _sc _rc
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
	    	_bred="$(_path tput bold 2> /dev/null; _path tput setaf 1 2> /dev/null)"
	    	_sgr0="$(_path tput sgr0 2> /dev/null)"
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
	if test -z "$EMACS" -a -z "$MC_SID" -a "$restricted" != true -a \
		-z "$STY" -a -n "$DISPLAY" -a ! -r $HOME/.bash.expert
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
	if test "${PS1-\$ }" = '$ ' -o "${PS1-\$ }" = '# ' ; then
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
	if test "$UID" = 0 ; then
	    PS1="${HOST}:"'${PWD}'" # "
	else
	    PS1="${USER}@${HOST}:"'${PWD}'"> "
	fi
	;;
    esac
    PS2='> '

    if test "$is" = "ash" ; then
	# The ash shell does not have an alias builtin in
	# therefore we use functions here. This is a seperate
	# file because other shells may run into trouble
	# if they parse this even if they do not expand.
	if test -s /etc/profile.d/alias.ash
	then . /etc/profile.d/alias.ash
	elif test -s /usr/etc/profile.d/alias.ash
        then . /usr/etc/profile.d/alias.ash
	fi
    else
	if test -s /etc/profile.d/alias.bash
	then . /etc/profile.d/alias.bash
	elif test -s /usr/etc/profile.d/alias.bash
	then . /usr/etc/profile.d/alias.bash
	fi
	test -s $HOME/.alias && . $HOME/.alias
    fi

    #
    # Expert mode: if we find $HOME/.bash.expert we skip our settings
    # used for interactive completion and read in the expert file.
    #
    if test "$is" = "bash" -a -r $HOME/.bash.expert ; then
	. $HOME/.bash.expert
    elif test "$is" = "bash" -a "$restricted" != true ; then
	# Complete builtin of the bash 2.0 and higher
	case "$BASH_VERSION" in
	[2-9].*)
	    if test -e /etc/bash_completion ; then
		. /etc/bash_completion
	    elif test -s /etc/profile.d/bash_completion.sh ; then
		. /etc/profile.d/bash_completion.sh
	    elif test -s /usr/etc/profile.d/bash_completion.sh ; then
		. /usr/etc/profile.d/bash_completion.sh
	    elif test -s /etc/profile.d/complete.bash ; then
		. /etc/profile.d/complete.bash
	    elif test -s /usr/etc/profile.d/complete.bash ; then
		. /usr/etc/profile.d/complete.bash
	    fi
	    # Do not source twice if already handled by bash-completion
	    if [[ -n $BASH_COMPLETION_COMPAT_DIR && $BASH_COMPLETION_COMPAT_DIR != /etc/bash_completion.d ]]; then
		for s in /etc/bash_completion.d/*.sh ; do
		    test -r $s && . $s
		done
	    elif [[ -n $BASH_COMPLETION_COMPAT_DIR && $BASH_COMPLETION_COMPAT_DIR != /usr/etc/bash_completion.d ]]; then
		for s in /usr/etc/bash_completion.d/*.sh ; do
		    test -r $s && . $s
		done
	    fi
	    if test -e $HOME/.bash_completion ; then
		. $HOME/.bash_completion
	    fi
	    if test -f /etc/bash_command_not_found
	    then . /etc/bash_command_not_found
	    elif test -f /usr/etc/bash_command_not_found
	    then . /usr/etc/bash_command_not_found
	    fi
	    ;;
	*)  ;;
	esac
    fi

    # Do not save dupes and lines starting by space in the bash history file
    : ${HISTCONTROL=ignoreboth}
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
	if test -f /etc/zsh_command_not_found
	then . /etc/zsh_command_not_found
	elif test -f /usr/etc/zsh_command_not_found
	then . /usr/etc/zsh_command_not_found
	fi
    fi
    ;;
esac

# Source /etc/profile.d/vte.sh, which improvies usage of VTE based terminals.
# It is vte.sh's responsibility to 'not load' when it's not applicable (not inside a VTE term)
# If you want to 'disable' this functionality, set the sticky bit on /etc/profile.d/vte.sh
if test -r /etc/profile.d/vte.sh -a ! -k /etc/profile.d/vte.sh
then . /etc/profile.d/vte.sh
elif test -r /usr/etc/profile.d/vte.sh -a ! -k /usr/etc/profile.d/vte.sh
then . /usr/etc/profile.d/vte.sh
fi

# Source /etc/profile.d/distrobox_profile.sh, which improves usage and prompt clarity when using Distrobox.
# It is distrobox_profile.sh's responsibility to 'not load' when it's not applicable (not inside a Distrobox)
# distrobox_profile.sh will also take care of differences in behaviour needed by different shells (eg bash vs zsh)
if test -s /etc/profile.d/distrobox_profile.sh ; then
	. /etc/profile.d/distrobox_profile.sh
elif test -s /usr/etc/profile.d/distrobox_profile.sh ; then
	. /usr/etc/profile.d/distrobox_profile.sh
fi

if test "$_is_save" = "unset" ; then
    #
    # Just in case the user excutes a command with ssh or sudo
    #
    if test \( -n "$SSH_CONNECTION" -o -n "$SUDO_COMMAND" \) -a -z "$PROFILEREAD" -a "$noprofile" != true ; then
	_is_save="$is"
	_SOURCED_FOR_SSH=true
	. /etc/profile > /dev/null 2>&1
	unset _SOURCED_FOR_SSH
	is="$_is_save"
	_is_save=unset
    fi
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
# (for options defined by your sysadmin, not SUSE Linux)
#
case "$is" in
bash) test -s /etc/bash.bashrc.local && . /etc/bash.bashrc.local ;;
ksh)  test -s /etc/ksh.kshrc.local   && . /etc/ksh.kshrc.local ;;
zsh)  test -s /etc/zsh.zshrc.local   && . /etc/zsh.zshrc.local ;;
ash)  test -s /etc/ash.ashrc.local   && . /etc/ash.ashrc.local
esac
test -s /etc/sh.shrc.local && . /etc/sh.shrc.local

if test "$_is_save" = "unset" ; then
    unset is _is_save
fi

if test "$restricted" = true -a -z "$PROFILEREAD" ; then
    PATH=/usr/lib/restricted/bin
    export PATH
fi
#
# End of /etc/bash.bashrc
#
