# /etc/profile.d/complete.bash for SuSE Linux
#
#
# This feature has its own file because some other shells
# do not like the way how the bash assigns arrays
#
# REQUIRES bash 2.0 and higher
#

shopt -s extglob

function _cd_ ()
{
    local c=${COMP_WORDS[COMP_CWORD]}
    case "$c" in
    \~*) COMPREPLY=($(compgen -u "$c")) ;;
    *)   COMPREPLY=($(compgen -d "$c")) ;;
    esac
}

complete -A directory -F _cd_	cd rmdir pushd mkdir chroot chrootx
complete -A directory -A file	chown chgrp chmod chattr ln
complete -A directory -A file	more cat less strip grep vi ed

_file_ ()
{
    # bash `complete' is broken because you can not combine
    # -d, -f, and -X pattern without missing directories.
    local c=${COMP_WORDS[COMP_CWORD]}
    local e="compgen -f -X"

    case "$1" in
    compress)		COMPREPLY=($($e '*.Z'			$c)) ;;
    bunzip2)		COMPREPLY=($($e '!*.bz2'		$c)) ;;
    gunzip)		COMPREPLY=($($e '!*.gz'			$c)) ;;
    uncompress)		COMPREPLY=($($e '!*.Z'			$c)) ;;
    unzip)		COMPREPLY=($($e '!*.+(zip|jar|exe)'	$c)) ;;
    gs|ghostview)	COMPREPLY=($($e '!*.+(ps|PS|pdf|PDF)'	$c)) ;;
    gv)			COMPREPLY=($($e '!*.+(ps|ps.gz|pdf|PDF)' $c)) ;;
    acroread|xpdf)	COMPREPLY=($($e '!*.+(pdf|PDF)'		$c)) ;;
    dvips|xdvi)		COMPREPLY=($($e '!*.+(dvi|DVI)'		$c)) ;;
    tex|latex)		COMPREPLY=($($e '!*.+(tex|TEX|texi)'	$c)) ;;
    esac
}

complete -d -X '.*' -F _file_		compress \
					bunzip2 \
					gunzip \
					uncompress \
					unzip \
					gs ghostview \
					gv \
					acroread xpdf \
					dvips xdvi \
					tex latex

complete -A function -A alias -A command -A builtin type

complete -A function			function
complete -A alias			alias unalias
complete -A variable			unset local readonly
complete -A variable -A file		export
complete -A variable -A export		unset
complete -A shopt			shopt
complete -A setopt			set
complete -A helptopic			help
complete -A user			talk su login sux
complete -A builtin			builtin
complete -A export			printenv
complete -A command			command which nohup exec nice eval 
complete -A command -A file		ltrace strace gdb
HOSTFILE=""
test -s $HOME/.hosts && HOSTFILE=$HOME/.hosts
complete -A hostname			ping telnet rsh ssh slogin \
					rlogin traceroute nslookup
complete -A stopped -P '%'		bg
complete -A job -P '%'			fg jobs disown

#
# End of /etc/profile.d/complete.bash
#
