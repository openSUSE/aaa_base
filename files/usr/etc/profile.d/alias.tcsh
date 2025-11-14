alias o 'less'
alias .. 'cd ..'
alias ... 'cd ../..'
alias cd.. 'cd ..'
alias rd rmdir
if ( -X tput && ${?TERM} && -t 1 ) then
    if ( `tput colors` >= 8 ) then
	alias egrep 'grep -E --color=auto'
	alias fgrep 'grep -F --color=auto'
	alias grep 'grep --color=auto'
	ip --color=auto -V >& /dev/null
	if ( $? == 0 ) alias ip 'ip --color=auto'
    else
	alias egrep 'grep -E'
	alias fgrep 'grep -F'
    endif
else
    alias egrep 'grep -E'
    alias fgrep 'grep -F'
endif
alias md 'mkdir -p'
alias startx 'if ( ! -x /usr/bin/startx ) echo "No startx installed";\
	      if (   -x /usr/bin/startx ) /usr/bin/startx |& tee ${HOME}/.xsession-error'
alias remount '/usr/bin/mount -o remount,\!*'
alias xd 'set d = `readlink -f "\!^"` ; cd "${d:h}"; unset d'
