# /etc/profile.d/complete.bash for SuSE Linux
#
#
# This feature has its own file because some other shells
# do not like the way how the bash assigns arrays
#
# REQUIRES bash 2.0 and higher
#

_def=; _dir=; _file=; _nosp=
if complete -o default _nullcommand &> /dev/null ; then
    _def="-o default"
    _dir="-o dirnames"
    _file="-o filenames"
fi
_minusdd="-d ${_dir}"
_minusdf="-d ${_file}"
if complete -o nospace _nullcommand &> /dev/null ; then
    _nosp="-o nospace"
    _minusdd="${_nosp} ${_dir}"
    _minusdf="${_nosp} ${_dir}"
fi
complete -r _nullcommand &> /dev/null

# Expanding shell function for directories
function _cd_ ()
{
    local c=${COMP_WORDS[COMP_CWORD]}
    local s g=0 x
    local IFS=$'\n'
    local -i o

    shopt -q extglob && g=1
    test $g -eq 0 && shopt -s extglob

    case "$(complete -p ${1##*/} 2> /dev/null)" in
	mkdir)	;;
	*) s="-S/"  
    esac

    case "$c" in
    \$\(*\))	eval COMPREPLY=\(${c}\) ;;
    \$\(*)	COMPREPLY=($(compgen -c -P '$(' -S ')'	-- ${c#??}))	;;
    \`*\`)	eval COMPREPLY=\(${c}\) ;;
    \`*)	COMPREPLY=($(compgen -c -P '\`' -S '\`' -- ${c#?}))	;;
    \$\{*\})	eval COMPREPLY=\(${c}\) ;;
    \$\{*)	COMPREPLY=($(compgen -v -P '${' -S '}'	-- ${c#??}))	;;
    \$*)	COMPREPLY=($(compgen -v -P '$' $s	-- ${c#?}))	;;
    \~*/*)	COMPREPLY=($(compgen -d $s		-- "${c}"))	;;
    \~*)	COMPREPLY=($(compgen -u $s		-- "${c}"))	;;
    *\:*)
                if [[ $COMP_WORDBREAKS =~ : ]] ; then
		    local C=${c%"${c##*[^\\]:}"}
		    COMPREPLY=($(compgen -d $s          -- "${c}"))
		    for ((o=0; o<${#COMPREPLY[@]}; o++)) ; do
			COMPREPLY[o]=${COMPREPLY[o]#"$C"}
		    done
                fi
    esac

    if test "${1##*/}" = "cd" -a ${#COMPREPLY[@]} -gt 0 ; then
	#
	# Handle the CDPATH variable
	#
	x="$(bind -v)"
	local dir=$([[ $x =~ mark-directories+([[:space:]])on ]] && echo on)
	local sym=$([[ $x =~ mark-symlinked-directories+([[:space:]])on ]] && echo on)

	for x in ${CDPATH//:/$'\n'}; do
	    o=${#COMPREPLY[@]}
	    for s in $(compgen -d $x/$c); do
		if [[ (($sym == on && -h $s) || ($dir == on && ! -h $s)) && ! -d ${s#$x/} ]] ; then
		    s="${s}/"
		fi
		COMPREPLY[o++]=${s#$x/}
	    done
	done
    fi

    if test ${#COMPREPLY[@]} -gt 0 ; then
	#
	# Escape spaces and braces in path names with `\'
	#
	s="${COMP_WORDBREAKS// }"
	s="${s//	}"
	s="${s//[\{\}()\[\]]}"
	s="${s} 	(){}[]"
	o=${#s}
    
	while test $((o--)) -gt 0 ; do
	    c="${s:${o}:1}"
	    COMPREPLY=(${COMPREPLY[*]//${c}/\\${c}})
	done
    fi

    #
    # Append a slash on the real result, avoid annoying double tab
    #
    if test "${1##*/}" != "mkdir" -a ${#COMPREPLY[@]} -eq 1 ; then
	x=${COMPREPLY[0]}
	o=$((${#x} - 1))
	if test "$x" = "$c" -a "${x:${o}:1}" != "/"; then
	    COMPREPLY[0]="${x}/"
	fi
    fi

    test $g -eq 0 && shopt -u extglob
}

if shopt -q cdable_vars; then
    complete ${_minusdd} -vF _cd_	cd
else
    complete ${_minusdd} -F  _cd_	cd
fi
complete ${_minusdd} -F _cd_		rmdir pushd chroot chrootx
complete ${_minusdf} -F _cd_		mkdir

# General expanding shell function
_exp_ ()
{
    # bash `complete' is broken because you can not combine
    # -d, -f, and -X pattern without missing directories.
    local c=${COMP_WORDS[COMP_CWORD]}
    local a="${COMP_LINE}"
    local e s g=0 cd dc t=""
    local IFS

    shopt -q extglob && g=1
    test $g -eq 0 && shopt -s extglob
    # Don't be fooled by the bash parser if extglob is off by default
    cd='*-?(c)d*'
    dc='*-d?(c)*'

    case "${1##*/}" in
    compress)		e='*.Z'					;;
    bzip2)
	case "$c" in
	-)		COMPREPLY=(d c)
			test $g -eq 0 && shopt -u extglob
			return					;;
 	-?|-??)		COMPREPLY=($c)
			test $g -eq 0 && shopt -u extglob
			return					;;
	esac
	case "$a" in
	$cd|$dc)	e='!*.bz2'				;;
	*)		e='*.bz2'				;;
	esac							;;
    bunzip2)		e='!*.bz2'				;;
    gzip)
	case "$c" in
	-)		COMPREPLY=(d c)
			test $g -eq 0 && shopt -u extglob
			return					;;
 	-?|-??)		COMPREPLY=($c)
			test $g -eq 0 && shopt -u extglob
			return					;;
	esac
	case "$a" in
	$cd|$dc)	e='!*.+(gz|tgz|z|Z)'			;;
	*)		e='*.+(gz|tgz|z|Z)'			;;
	esac							;;
    gunzip)		e='!*.+(gz|tgz|z|Z)'			;;
    uncompress)		e='!*.Z'				;;
    unzip)		e='!*.+(???)'
			t="@(MS-DOS executable|Zip archive)*"	;;
    gs|ghostview)	e='!*.+(eps|EPS|ps|PS|pdf|PDF)'		;;
    gv|kghostview)	e='!*.+(eps|EPS|ps|PS|ps.gz|pdf|PDF)'	;;
    acroread|[xk]pdf)	e='!*.+(pdf|PDF)'			;;
    dvips)		e='!*.+(dvi|DVI)'			;;
    [xk]dvi)		e='!*.+(dvi|dvi.gz|DVI|DVI.gz)'		;;
    tex|latex|pdflatex)	e='!*.+(tex|TEX|texi|latex)'		;;
    export)
	case "$a" in
	*=*)		c=${c#*=}				;;
	*)		COMPREPLY=($(compgen -v -- ${c}))
			test $g -eq 0 && shopt -u extglob
			return					;;
	esac
	;;
    *)			e='!*'
    esac

    case "$(complete -p ${1##*/} 2> /dev/null)" in
	*-d*)	;;
	*) s="-S/"
    esac

    IFS=$'\n'
    case "$c" in
    \$\(*\))	   eval COMPREPLY=\(${c}\) ;;
    \$\(*)		COMPREPLY=($(compgen -c -P '$(' -S ')'  -- ${c#??}))	;;
    \`*\`)	   eval COMPREPLY=\(${c}\) ;;
    \`*)		COMPREPLY=($(compgen -c -P '\`' -S '\`' -- ${c#?}))	;;
    \$\{*\})	   eval COMPREPLY=\(${c}\) ;;
    \$\{*)		COMPREPLY=($(compgen -v -P '${' -S '}'  -- ${c#??}))	;;
    \$*)		COMPREPLY=($(compgen -v -P '$'          -- ${c#?}))	;;
    \~*/*)		COMPREPLY=($(compgen -f -X "$e"         -- ${c}))	;;
    \~*)		COMPREPLY=($(compgen -u ${s}	 	-- ${c}))	;;
    *@*)		COMPREPLY=($(compgen -A hostname -P '@' -S ':' -- ${c#*@})) ;;
    *[*?[]*)		COMPREPLY=($(compgen -G "${c}"))			;;
    *[?*+\!@]\(*\)*)
	if test $g -eq 0 ; then
			COMPREPLY=($(compgen -f -X "$e" -- $c))
			test $g -eq 0 && shopt -u extglob
			return
	fi
			COMPREPLY=($(compgen -G "${c}"))			;;
    *)
	if test "$c" = ".." ; then
			COMPREPLY=($(compgen -d -X "$e" -S / ${_nosp} -- $c))
	else
			for s in $(compgen -f -X "$e" -- $c) ; do
			    if test -d $s ; then
				COMPREPLY=(${COMPREPLY[@]} $(compgen -f -X "$e" -S / -- $s))
			    elif test -z "$t" ; then
				COMPREPLY=(${COMPREPLY[@]} $s)
			    else
				case "$(file -b $s 2> /dev/null)" in
				$t) COMPREPLY=(${COMPREPLY[@]} $s)		;;
				esac
			    fi
			done
	fi									;;
    esac
    test $g -eq 0 && shopt -u extglob
}

complete -d -X '.[^./]*' -F _exp_ ${_file} \
				 	compress \
					bzip2 \
					bunzip2 \
					gzip \
					gunzip \
					uncompress \
					unzip \
					gs ghostview \
					gv kghostview \
					acroread xpdf kpdf \
					dvips xdvi kdvi \
					tex latex pdflatex
# No clean way to hande variable expansion _and_ file/dir name expansion
# with the same string. So let the default expansion on for that commands.
#complete -d -F _exp_ ${_def}		chown chgrp chmod chattr ln
#complete -d -F _exp_ ${_def}		more cat less strip grep vi ed

complete -A function -A alias -A command -A builtin \
					type
complete -A function			function
complete -A alias			alias unalias
complete -A variable			unset local readonly
complete -F _exp_ ${_def} ${_nosp}	export
complete -A variable -A export		unset
complete -A shopt			shopt
complete -A setopt			set
complete -A helptopic			help
complete -A user			talk su login sux
complete -A builtin			builtin
complete -A export			printenv
complete -A command ${_def}		command which nohup exec nice eval 
complete -A command ${_def}		ltrace strace gdb
HOSTFILE=""
test -s $HOME/.hosts && HOSTFILE=$HOME/.hosts
complete -A hostname			ping telnet slogin rlogin \
					traceroute nslookup
complete -A hostname -A directory -A file \
					rsh ssh scp
complete -A stopped -P '%'		bg
complete -A job -P '%'			fg jobs disown

# Expanding shell function for manual pager
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
	\./*)
	    COMPREPLY=($(compgen -f -d -X '\./.*'  -- $c)) ;;
    [0-9n]|[0-9n]p)
	    COMPREPLY=($c)	;;
	 *)
	case "$o" in
	    -l|--local-file)
		COMPREPLY=($(compgen -f -d -X '.*' -- $c)) ;;
	[0-9n]|[0-9n]p)
		s=$(eval echo {${m}}$o/)
		if type -p sed &> /dev/null ; then
		    COMPREPLY=(\
			$(ls -1fUA $s 2>/dev/null|\
			  sed -n "/^$c/{s@\.[0-9n].*\.gz@@g;s@.*/:@@g;p;}")\
		    )
		else
		    s=($(ls -1fUA $s 2>/dev/null))
		    s=(${s[@]%%.[0-9n]*})
		    s=(${s[@]#*/:})
		    for m in ${s[@]} ; do
			case "$m" in
			    $c*) COMPREPLY=(${COMPREPLY[@]} $m)
			esac
		    done
		    unset m s
		    COMPREPLY=(${COMPREPLY[@]%%.[0-9n]*})
		    COMPREPLY=(${COMPREPLY[@]#*/:})
		fi					   ;;
	     *) COMPREPLY=($(compgen -c -- $c))		   ;;
	esac
    esac
}

complete -F _man_ ${_file}		man

unset _def _dir _file _nosp _minusdd _minusdf

#
# End of /etc/profile.d/complete.bash
#
