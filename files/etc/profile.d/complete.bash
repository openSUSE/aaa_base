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
    local o="$IFS" x
    IFS='
'
    case "$c" in
    \~*) COMPREPLY=($(compgen -u -- "$c")) ;;
    *)	 COMPREPLY=($(compgen -d -- "$c"))
	 case "$1" in
	 mkdir)
	     for x in $(compgen -f -S .d -- "${c%.}") ; do
		 test -d "${x%.d}" || COMPREPLY=(${COMPREPLY[@]} ${x})
	     done
	 esac
    esac
    IFS="$o"
}

complete -A directory -F _cd_		cd rmdir pushd chroot chrootx
complete -A directory -A file -F _cd_	mkdir

_file_ ()
{
    # bash `complete' is broken because you can not combine
    # -d, -f, and -X pattern without missing directories.
    local c=${COMP_WORDS[COMP_CWORD]}
    local a="${COMP_WORDS[@]}"
    local o="$IFS"
    local e s g=0

    shopt -q extglob && g=1
    test $g -eq 0 && shopt -s extglob

    case "$1" in
    compress)		e='*.Z'					;;
    bzip2)
	case "$c" in
	-)		COMPREPLY=(d c); return			;;
 	-?|-??)		COMPREPLY=($c) ; return			;;
	esac
	case "$a" in
	*-?(c)d*)	e='!*.bz2'				;;
	*)		e='*.bz2'				;;
	esac							;;
    bunzip2)		e='!*.bz2'				;;
    gzip)
	case "$c" in
	-)		COMPREPLY=(d c); return			;;
 	-?|-??)		COMPREPLY=($c) ; return			;;
	esac
	case "$a" in
	*-?(c)d*)	e='!*.+(gz|tgz|z|Z)'			;;
	*)		e='*.+(gz|tgz|z|Z)'			;;
	esac							;;
    gunzip)		e='!*.+(gz|tgz|z|Z)'			;;
    uncompress)		e='!*.Z'				;;
    unzip)		e='!*.+(zip|ZIP|jar|exe|EXE)'		;;
    gs|ghostview)	e='!*.+(eps|EPS|ps|PS|pdf|PDF)'		;;
    gv)			e='!*.+(eps|EPS|ps|PS|ps.gz|pdf|PDF)'	;;
    acroread|xpdf)	e='!*.+(pdf|PDF)'			;;
    dvips)		e='!*.+(dvi|DVI)'			;;
    xdvi)		e='!*.+(dvi|dvi.gz|DVI|DVI.gz)'		;;
    tex|latex)		e='!*.+(tex|TEX|texi|latex)'		;;
    *)			e='!*'
    esac

    case "$(complete -p $1)" in
	*-d*) ;;
	*) s="/"  
    esac

    IFS='
'
    case "$c" in
    \$\(*\))		COMPREPLY=(${c}) ;;
    \$\(*)		COMPREPLY=($(compgen -c -P '$(' -S ')'  -- ${c#??}))	;;
    \`*\`)		COMPREPLY=(${c}) ;;
    \`*)		COMPREPLY=($(compgen -c -P '\`' -S '\`' -- ${c#?}))	;;
    \$\{*\})		COMPREPLY=(${c}) ;;
    \$\{*)		COMPREPLY=($(compgen -v -P '${' -S '}'  -- ${c#??}))	;;
    \$*)		COMPREPLY=($(compgen -v -P '$'          -- ${c#?}))	;;
    ~*/*)		COMPREPLY=($(compgen -f -X "$e"         -- ${c}))	;;
    ~*)			COMPREPLY=($(compgen -u ${s:+-S$s} 	-- ${c}))	;;
    *@*)		COMPREPLY=($(compgen -A hostname -P '@' -S ':' -- ${c#*@})) ;;
    *[*?[]*)		COMPREPLY=($(compgen -G "${c}"))			;;
    *[?*+\!@]\(*\)*)
	if test $g -eq 0 ; then
			COMPREPLY=($(compgen -f -X "$e" -- $c))
			return
	fi
			COMPREPLY=($(compgen -G "${c}"))			;;
    *)			COMPREPLY=($(compgen -f -X "$e" -- $c))			;;
    esac
    IFS="$o"

    test $g -eq 0 && shopt -u extglob
}

complete -d -X '.[^./]*' -F _file_	compress \
					bzip2 \
					bunzip2 \
					gzip \
					gunzip \
					uncompress \
					unzip \
					gs ghostview \
					gv \
					acroread xpdf \
					dvips xdvi \
					tex latex
#complete -A directory -F _file_	chown chgrp chmod chattr ln
#complete -A directory -F _file_	more cat less strip grep vi ed

complete -A function -A alias -A command -A builtin type

complete -A function			function
complete -A alias			alias unalias
complete -A variable			unset local readonly
complete -A variable			export
complete -A variable -A export		unset
complete -A shopt			shopt
complete -A setopt			set
complete -A helptopic			help
complete -A user			talk su login sux
complete -A builtin			builtin
complete -A export			printenv
complete -A command			command which nohup exec nice eval 
complete -A command 			ltrace strace gdb
HOSTFILE=""
test -s $HOME/.hosts && HOSTFILE=$HOME/.hosts
complete -A hostname			ping telnet slogin rlogin \
					traceroute nslookup
complete -A hostname -A directory -A file rsh ssh scp
complete -A stopped -P '%'		bg
complete -A job -P '%'			fg jobs disown

_man_ ()
{
    local c=${COMP_WORDS[COMP_CWORD]}
    local o=${COMP_WORDS[COMP_CWORD-1]}
    local os="- f k P S t l"
    local ol="whatis apropos pager sections troff local-file"
    local m s

    if test -n "$MANPATH" ; then
	m=${MANPATH//:/\/man,}
    else
	m="/usr/X11R6/man/man,/usr/openwin/man/man,/usr/share/man/man"
    fi

    case "$c" in
 	 -) COMPREPLY=($os)	;;
	--) COMPREPLY=($ol) 	;;
 	-?) COMPREPLY=($c)	;;
    [1-9n]) COMPREPLY=($c)	;;
	 *)
	case "$o" in
	    -l) COMPREPLY=($(compgen -f -d -X '.*' -- $c)) ;;
	[1-9n]) s=$(eval echo {${m}}$o/)
		if type -p sed &> /dev/null ; then
		    COMPREPLY=(\
			$(ls -1fUA $s 2>/dev/null|\
			  sed -n "/^$c/{s@\.[1-9n].*\.gz@@g;s@.*/:@@g;p;}")\
		    )
		else
		    s=($(ls -1fUA $s 2>/dev/null))
		    s=(${s[@]%%.[1-9n]*})
		    s=(${s[@]#*/:})
		    for m in ${s[@]} ; do
			case "$m" in
			    $c*) COMPREPLY=(${COMPREPLY[@]} $m)
			esac
		    done
		    unset m s
		    COMPREPLY=(${COMPREPLY[@]%%.[1-9n]*})
		    COMPREPLY=(${COMPREPLY[@]#*/:})
		fi					   ;;
	     *) COMPREPLY=($(compgen -c -- $c))		   ;;
	esac
    esac
}

complete -F _man_			man

#
# End of /etc/profile.d/complete.bash
#
