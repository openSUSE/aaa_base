alias o 'less'
alias .. 'cd ..'
alias ... 'cd ../..'
alias cd.. 'cd ..'
alias rd rmdir
alias md 'mkdir -p'
alias startx 'if ( ! -x /usr/bin/startx ) echo "No startx installed";\
	      if (   -x /usr/bin/startx ) /usr/bin/startx |& tee ${HOME}/.xsession-error'
alias remount '/usr/bin/mount -o remount,\!*'
alias xd 'set d = `readlink -f "\!^"` ; cd `dirname "$d"`; unset d'
