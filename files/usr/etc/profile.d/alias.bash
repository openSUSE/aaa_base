#
# Some useful functions
#
if test -z "$restricted" ; then
    startx  () {
        test -x /usr/bin/startx || {
            echo "No startx installed" 1>&2
            return 1;
        }
        /usr/bin/startx ${1+"$@"} 2>&1 | tee $HOME/.xsession-errors
    }
    remount () { /usr/bin/mount -o remount,${1+"$@"} ; }
fi

#
# Set some generic aliases
#
alias o='less'
alias ..='cd ..'
alias ...='cd ../..'
alias cd..='cd ..'
if test "$is" != "ksh" ; then
    alias -- +='pushd .'
    alias -- -='popd'
fi
alias rd=rmdir
alias egrep='grep -E --color=auto'
alias fgrep='grep -F --color=auto'
alias grep='grep --color=auto'
if ip --color=auto -V > /dev/null 2>/dev/null ; then
    alias ip='ip --color=auto'
fi
alias md='mkdir -p'
if test "$is" = "bash" -a ! -x /usr/bin/which ; then
    #
    # Other shells use the which command in path (e.g. ash) or
    # their own builtin for the which command (e.g. ksh and zsh).
    #
    _which () {
	local file=$(type -p ${1+"$@"} 2>/dev/null)
	if test -n "$file" -a -x "$file"; then
	    echo "$file"
	    return 0
	fi
	hash -r
	type -P ${1+"$@"}
    }
    alias which=_which
fi
alias rehash='hash -r'
if test "$is" != "ksh" ; then
    alias beep='echo -en "\007"' 
else
    alias beep='echo -en "\x07"'
fi
alias unmount='echo "Error: Try the command: umount" 1>&2; false'
# jump to the directory where the file is (e.g., xd $(location something))
xd () {
    cd "$(dirname "$(readlink -f "$1")")"
}
