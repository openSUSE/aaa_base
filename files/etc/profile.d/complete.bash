# /etc/profile.d/complete.bash for SuSE Linux
#
#
# This feature has its own file because some other shells
# do not like the way how the bash assigns arrays
#
# REQUIRES bash 2.0 and higher
#

if complete -o default _nullcommand &> /dev/null ; then
    _def="-o default"
    _dir="-o dirnames"
   _file="-o filenames"
else
    _def=""
    _dir=""
   _file=""
fi
if complete -o nospace _nullcommand &> /dev/null ; then
    _nosp="-o nospace"
 _minusdd="${_def} ${_nosp} ${_dir}"
 _minusdf="${_def} ${_nosp} ${_dir}"
else
    _nosp=""
 _minusdd="-d ${_dir}"
 _minusdf="-d ${_file}"
fi
complete -r _nullcommand &> /dev/null

# Escape file and directory names, add slash to directories if needed.
# Escaping could be done by the option 'filenames' but this fails
# e.g. on variable expansion like $HO<TAB>
_compreply_ ()
{
    local IFS=$'\n'
    local s x
    local -i o

    test ${#COMPREPLY[@]} -eq 0 && return 0

    #
    # Escape spaces and braces in path names with `\'
    #
    s="${COMP_WORDBREAKS// }"
    s="${s//	}"
    s="${s//[\{\}()\[\]]}"
    s="${s} 	(){}[]"
    o=${#s}

    while test $((o--)) -gt 0 ; do
	x="${s:${o}:1}"
	case "$x" in
	\() COMPREPLY=($(echo "${COMPREPLY[*]}"|command sed -r 's/\(/\\\(/g')) ;;
	*)  COMPREPLY=(${COMPREPLY[*]//${x}/\\${x}}) ;;
	esac
    done

    #
    # Append a slash on the real result, avoid annoying double tab
    #
    for ((o=0; o < ${#COMPREPLY[*]}; o++)) ; do
	test -d "${COMPREPLY[$o]}"     || continue
	test -z "${COMPREPLY[$o]##*/}" || continue
	COMPREPLY[$o]="${COMPREPLY[$o]}/"
    done
}

# Expanding shell function for directories
_cd_ ()
{
    local c=${COMP_WORDS[COMP_CWORD]}
    local s g=0 x
    local IFS=$'\n'
    local -i o

    shopt -q extglob && g=1
    test $g -eq 0 && shopt -s extglob

    case "$(complete -p ${1##*/} 2> /dev/null)" in
    mkdir)  ;;
    *)	    s="-S/"  
    esac

    case "$c" in
    *\**)	COMPREPLY=($(for x in $c; do
		    test -d $x && echo $x/
		done)) ;;
    \$\(*\))	eval COMPREPLY=\(${c}\) ;;
    \$\(*)	COMPREPLY=($(compgen -c -P '$(' -S ')'	-- ${c#??}))	;;
    \`*\`)	eval COMPREPLY=\(${c}\) ;;
    \`*)	COMPREPLY=($(compgen -c -P '\`' -S '\`' -- ${c#?}))	;;
    \$\{*\})	eval COMPREPLY=\(${c}\) ;;
    \$\{*)	COMPREPLY=($(compgen -v -P '${' -S '}'	-- ${c#??}))	;;
    \$*)	COMPREPLY=($(compgen -v -P '$' $s	-- ${c#?}))	;;
    \~*/*)	COMPREPLY=($(compgen -d $s 		-- "${c}"))	;;
    \~*)	COMPREPLY=($(compgen -u $s 		-- "${c}"))	;;
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

    _compreply_

    test $g -eq 0 && shopt -u extglob
    return 0
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
    local -i o
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
			return 0				;;
 	-?|-??)		COMPREPLY=($c)
			test $g -eq 0 && shopt -u extglob
			return 0				;;
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
			return 0				;;
 	-?|-??)		COMPREPLY=($c)
			test $g -eq 0 && shopt -u extglob
			return 0				;;
	esac
	case "$a" in
	$cd|$dc)	e='!*.+(gz|tgz|z|Z)'			;;
	*)		e='*.+(gz|tgz|z|Z)'			;;
	esac							;;
    gunzip)		e='!*.+(gz|tgz|z|Z)'			;;
    lzma)
	case "$c" in
	-)		COMPREPLY=(d c)
			test $g -eq 0 && shopt -u extglob
			return 0				;;
 	-?|-??)		COMPREPLY=($c)
			test $g -eq 0 && shopt -u extglob
			return 0				;;
	esac
	case "$a" in
	$cd|$dc)	e='!*.+(lzma)'				;;
	*)		e='*.+(lzma)'				;;
	esac							;;
    unlzma)		e='!*.+(lzma)'				;;
    xz)
	case "$c" in
	-)		COMPREPLY=(d c)
			test $g -eq 0 && shopt -u extglob
			return 0				;;
 	-?|-??)		COMPREPLY=($c)
			test $g -eq 0 && shopt -u extglob
			return 0				;;
	esac
	case "$a" in
	$cd|$dc)	e='!*.+(xz)'				;;
	*)		e='*.+(xz)'				;;
	esac							;;
    unxz)		e='!*.+(xz)'				;;
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
			return 0				;;
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
			return 0
	fi
			COMPREPLY=($(compgen -G "${c}"))			;;
    *)
	if test "$c" = ".." ; then
			COMPREPLY=($(compgen -d -X "$e" ${_nosp} -- $c))
	else
			if test -n "$t" ; then
			    let o=0
			    COMPREPLY=()
			    for s in $(compgen -f -X "$e" -- $c) ; do
				case "$(file -b "$s" 2> /dev/null)" in
				directory) COMPREPLY[$((o++))]="$s" ;;
				$t)	   COMPREPLY[$((o++))]="$s" ;;
				esac
			    done
			else
			    COMPREPLY=($(compgen -f -X "$e" -- $c))
			fi
	fi
    esac

    test $g -eq 0 && shopt -u extglob
    return 0
}

_gdb_ ()
{
    local c=${COMP_WORDS[COMP_CWORD]}
    local x
    local -i o

    if test $COMP_CWORD -eq 1 ; then
	case "$c" in
 	-*) COMPREPLY=($(compgen -W '-args -tty -s -e -se -c -x -d' -- "$c")) ;;
	*)  COMPREPLY=($(compgen -c -- "$c"))
	esac
	return 0
    fi
    local p=${COMP_WORDS[COMP_CWORD-1]}
    case "$p" in
    -args)	COMPREPLY=($(compgen -c -- "$c")) ;;
    -tty)	COMPREPLY=(/dev/tty* /dev/pts/*)
		COMPREPLY=($(compgen -W "${COMPREPLY[*]}" -- "$c")) ;;
    -s|e|-se)	COMPREPLY=($(compgen -f -- "$c")) ;;
    -c|-x)	COMPREPLY=($(compgen -f -- "$c")) ;;
    -d)		COMPREPLY=($(compgen -d ${_nosp} -- "$c")) ;;
    *)
		if test -z "$c"; then
		    COMPREPLY=($(command ps axho comm,pid |\
				 command sed -rn "\@^${p##*/}@{ s@.*[[:blank:]]+@@p; }"))
		else
		    COMPREPLY=()
		fi
		let o=${#COMPREPLY[*]}
		for s in $(compgen -f -- "$c") ; do
		    case "$(file -b "$s" 2> /dev/null)" in
		    directory)  COMPREPLY[$((o++))]="$s" ;;
		    *)		COMPREPLY[$((o++))]="$s" ;;
		    esac
		done
    esac 

   return 0
}

complete -d -X '.[^./]*' -F _exp_ ${_file} ${_def} \
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

complete -d -F _exp_ ${_file} ${_def}	chown chgrp chmod chattr ln
complete -d -F _exp_ ${_file} ${_def}	more cat less strip grep vi ed

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
complete -A command ${_def}		ltrace strace
complete -F _gdb_ ${_file} ${_def} 	gdb
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
			$(command ls -1UA $s 2>/dev/null|\
			  command sed -rn "/^$c/{s@\.[0-9n].*\.gz@@g;s@.*/:@@g;p;}")\
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
