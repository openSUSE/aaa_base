#
# (c) System csh.cshrc for tcsh, Werner Fink '93
#                       and  J"org Stadler '94
# Zusamengetragen, modifiziert, ergaenzt ...
# u.a. aus den Sourcen der tcsh und der man page und ...
#
onintr -
#
# Usefull
set echo_style=both
#
setenv OPENWINHOME /usr/openwin
setenv HELPPATH $OPENWINHOME/lib/help
setenv MINICOM "-c on"
set tmp=/usr/man
foreach man ( /usr/local/man \
              /usr/X11R6/man \
              /usr/openwin/man )
	if ( -d $man ) then
	   set tmp=${tmp}:${man}
	endif
end
setenv MANPATH $tmp
unset  tmp man
setenv MINICOM "-c on"
setenv HOSTNAME "`hostname -f`"
setenv HOST     "`hostname -s`"
if ( -f /etc/organization ) then
   setenv ORGANIZATION "`cat /etc/organization`"
endif
setenv MACHTYPE `uname -m`
setenv LESSCHARSET latin1
setenv LESS -sM
setenv LESSOPEN "|lesspipe.sh %s"
if ( -f /etc/lesskey.bin ) then
   setenv LESSKEY /etc/lesskey.bin
endif
setenv MORE -sl
setenv PAGER '/usr/bin/less -sM'
setenv GZIP -9
setenv CSHEDIT emacs
setenv INFODIR /usr/info:/usr/local/info
setenv INFOPATH $INFODIR
if ( -s /etc/nntpserver ) then
   setenv NNTPSERVER `cat /etc/nntpserver`
else
   setenv NNTPSERVER news
endif
#
set XLIBDIR=/usr/X11R6/lib/X11/
if ( -d /usr/lib/X11/app-defaults/ ) then
    set XLIBDIR=/usr/lib/X11/
endif
set path_xr5=${XLIBDIR}%L/%T/%N%C:${XLIBDIR}%l/%T/%N%C:${XLIBDIR}%T/%N%C
set path_xr6=${XLIBDIR}%L/%T/%N:${XLIBDIR}%l/%T/%N:${XLIBDIR}%T/%N
set path_var=/var/X11R6/%T/%N%C:/var/X11R6/%T/%N
setenv XFILESEARCHPATH "${path_xr5}:${path_xr6}:${path_var}"
if ( -d ${HOME}/.app-defaults/ ) then
    setenv XAPPLRESDIR  ${HOME}/.app-defaults/
endif
unset XLIBDIR path_xr5 path_xr6 path_var
#
if ( -f /usr/lib/teTeX/texmf.cnf ) then
   setenv TETEXDIR /usr/lib/teTeX
   if ( -d /usr/bin/TeX ) set path=( /usr/bin/TeX $path )
endif
#
# Which C-lib or locale db takes what?
# setenv LC_CTYPE iso_8859_1
# setenv LC_CTYPE ISO8859-1
# setenv LC_CTYPE ISO-8859-1
# setenv LC_CTYPE de_DE
# setenv LC_CTYPE en_GB
# setenv LANG de_DE
setenv INPUTRC /etc/inputrc
setenv COLORTERM 1
#
# SuSEconfig stuff
#
if ( -r /etc/SuSEconfig/csh.cshrc ) then
    source /etc/SuSEconfig/csh.cshrc
endif
#
if (! ${?prompt}) goto end
#
# Interactive session
#
set autocorrect=1
set autolist=ambiguous
set listmaxrows=23
# set cdpath = ( /var/spool )
# set complete=enhance
# set correct=all
set correct=cmd
set fignore=(.o \~)
# set histdup=prev
set histdup=erase
set history=100
set listjobs=long
set notify=1
set nostat=( /afs )
set rmstar=1
set savehist=( $history merge )
set showdots=1
set symlinks=expand
#
unset autologout
unset ignoreeof
#
set noglob
if ( -x /usr/bin/dircolors ) then
    if ( -r $HOME/.dir_colors ) then
	eval `dircolors -c $HOME/.dir_colors`
    else if ( -r /etc/DIR_COLORS ) then
	eval `dircolors -c /etc/DIR_COLORS`
    endif
endif
unalias ls
if ( "$uid" == "0" ) then
    setenv LS_OPTIONS '-a -N -T 0';
else
    setenv LS_OPTIONS '-N -T 0';
endif
alias ls 'ls $LS_OPTIONS --color=tty'
alias la 'ls -AF --color=none'
alias ll 'ls -l  --color=none'
alias l  'll'
alias dir  'ls --format=vertical'
alias vdir 'ls --format=long'
alias d dir;
alias v vdir;
# Handle emacs
if ($?EMACS) then
  setenv LS_OPTIONS '-N --color=none -T 0';
  tset -I -Q
  stty cooked pass8 dec nl -echo
  if ($?tcsh) unset edit
endif
unset noglob
#
# Hallo Bodo :-)
#
alias nocomplete 'uncomplete *'
#
# Prompting and Xterm title
#
set prompt="%B%m%b %C2%# "
alias cwdcmd 'echo "Directory: $cwd"'
if ( -o /dev/$tty && -x /usr/bin/biff ) /usr/bin/biff y
#
if ( ${?WINDOWID} && ${?EMACS} == 0 ) then
  alias cwdcmd '(echo -n "\033]2;$USER on ${HOST}: $cwd\007\033]1;$HOST\007" > /dev/tty)'
  cd .
endif
#
if ( -o /dev/$tty && ${?DISPLAY} ) then
  if ( -x /usr/bin/biff ) /usr/bin/biff n
  set prompt="%C2%# "
endif
#
# Are we a tcsh?
#
if ($?tcsh) then
    if ($tcsh != 1) then
        set _rev=$tcsh:r
        set _rel=$_rev:e
        set _pat=$tcsh:e
        set _rev=$_rev:r
    endif
    if ($_rev > 5 && $_rel > 1) then
        set _complete=1
    endif
    unset _rev _rel _pat
endif
#
if ($?_complete) then
    set noglob
#
    set hosts
    foreach _f ($HOME/.hosts /etc/csh.hosts $HOME/.rhosts /etc/hosts.equiv)
	if ( -r $_f ) then
	    set hosts=($hosts `grep -E -shv '^#|\+' $_f | cut -d " " -f 1`)
	endif
    end
    if ( -r $HOME/.netrc ) then
	set _f=`awk '/machine/ { print $2 }' < $HOME/.netrc` >& /dev/null
	set hosts=($hosts $_f)
    endif
    set hosts=(`echo $hosts localhost $HOSTNAME|tr ' ' '\n'|sort -t '.'|uniq`)
    unset _f
    set _maildir = /var/spool/mail
    set _ypdir  =  /var/yp
    set _domain =  "`domainname`"
    set _manpath="/usr{{/X11/man,/openwin/man,/lib/teTeX/man}/{man,cat},{/man/{man,cat},/man/preformat/cat}}"


    complete ispell	c/-/"(a A b B C d D e ee f L m M p s S T v vv w W)"/ \
			n/-d/"(english deutsch)"/ \
			n/-T/"(tex plaintex nroff latin1 ascii atari)"/ \
			n@-p@'`ls -1 $HOME/.ispell_*`'@ \
			n/-W/"(1 2 3 4 5)"/ \
			n/-L/x:'ispell -L <number>'/ \
			n/-f/t/ n/*/f:^*.{dvi,ps,a,o,gz,z,Z}/
    complete {r,s}sh 	c/-/"(l n)"/ n/-l/u/ N/-l/c/ n/-/c/ p/1/\$hosts/ p/2/c/ p/*/f/
    complete xrsh 	c/-/"(l n)"/ n/-l/u/ N/-l/c/ n/-/c/ p/1/\$hosts/ p/2/c/ p/*/f/
    complete {r,s}login 	c/-/"(l 8 e)"/ n/-l/u/ p/1/\$hosts/ 
    complete xlogin 	n/*/\$hosts/
    complete telnet 	p/1/\$hosts/ p/2/x:'<port>'/ n/*/n/
    complete xtelnet 	n/*/\$hosts/ 
    complete ywho	n/*/\$hosts/
    # argument from list in $hosts
    complete cd  	p/1/d/		# Directories only
    complete chdir 	p/1/d/
    complete pushd 	p/1/d/
    complete popd 	p/1/d/
    complete pu 	p/1/d/
    complete po 	p/1/d/
    complete mkdir	c/--/"(parents help version mode)"/ c/-/"(p m -)"/ \
			n/{-m,--mode}/x:'<mode>'/ n/*/d/
    complete rmdir	c/--/"(parents help version)"/ c/-/"(p -)"/ n/*/d/
    complete complete	p/1/X/
    complete uncomplete p/*/X/
    complete exec 	p/1/c/
    complete trace 	p/1/c/
    complete strace 	p/1/c/
    complete which	n/*/c/
    complete where	n/*/c/
    complete skill 	p/1/c/
    complete dde	p/1/c/ 
    complete adb	c/-I/d/ n/-/c/ N/-/"(core)"/ p/1/c/ p/2/"(core)"/
    complete sdb	p/1/c/
    complete dbx	c/-I/d/ n/-/c/ N/-/"(core)"/ p/1/c/ p/2/"(core)"/
    complete xdb	p/1/c/
    complete gdb	n/-d/d/ n/*/c/
    complete ups	p/1/c/
    complete set	'c/*=/f/' 'p/1/s/=' 'n/=/f/'
    complete unset	n/*/s/
    complete unsetenv 	n/*/e/
    complete alias 	p/1/a/		# only aliases are valid
    complete unalias 	n/*/a/
    complete xdvi 	n/*/f:*.dvi/	# Only files that match *.dvi
    complete laser	n/*/f:*.dvi/
    complete dvips 	'c/-P/$printers/' 'n/-o/f:*.{ps,PS}/' n/*/f:*.dvi/
    complete tex 	n/*/f:*.tex/
    complete latex 	n/*/f:*.{tex,ltx}/
    complete slitex 	n/*/f:*.tex/
    complete su		c/--/"(login fast preserve-environment command shell \
			help version)"/ c/-/"(f l m p c s -)"/ \
			n/{-c,--command}/c/ \
			n@{-s,--shell}@'`cat /etc/shells`'@ n/*/u/
    complete cc 	c/-[IL]/d/ \
              c@-l@'`\ls -1 /usr/lib/lib*.a | sed s%^.\*/lib%%\;s%\\.a\$%%`'@ \
			c/-/"(o l c g L I D U)"/ n/*/f:*.[coa]/
    complete acc 	c/-[IL]/d/ \
	      c@-l@'`\ls -1 /usr/lang/SC1.0/lib*.a | sed s%^.\*/lib%%\;s%\\.a\$%%`'@ \
			c/-/"(o l c g L I D U)"/ n/*/f:*.[coasi]/
    complete gcc 	c/-[IL]/d/ \
		 	c/-f/"(caller-saves cse-follow-jumps delayed-branch \
		               elide-constructors expensive-optimizations \
			       float-store force-addr force-mem inline \
			       inline-functions keep-inline-functions \
			       memoize-lookups no-default-inline \
			       no-defer-pop no-function-cse omit-frame-pointer \
			       rerun-cse-after-loop schedule-insns \
			       schedule-insns2 strength-reduce \
			       thread-jumps unroll-all-loops \
			       unroll-loops syntax-only all-virtual \
			       cond-mismatch dollars-in-identifiers \
			       enum-int-equiv no-asm no-builtin \
			       no-strict-prototype signed-bitfields \
			       signed-char this-is-variable unsigned-bitfields \
			       unsigned-char writable-strings call-saved-reg \
			       call-used-reg fixed-reg no-common \
			       no-gnu-binutils nonnull-objects \
			       pcc-struct-return pic PIC shared-data \
			       short-enums short-double volatile)"/ \
		 	c/-W/"(all aggregate-return cast-align cast-qual \
		      	       comment conversion enum-clash error format \
		      	       id-clash-len implicit missing-prototypes \
		      	       no-parentheses pointer-arith return-type shadow \
		      	       strict-prototypes switch uninitialized unused \
		      	       write-strings)"/ \
		 	c/-m/"(68000 68020 68881 bitfield fpa nobitfield rtd \
			       short c68000 c68020 soft-float g gnu unix fpu \
			       no-epilogue)"/ \
		 	c/-d/"(D M N)"/ \
		 	c/-/"(f W vspec v vpath ansi traditional \
			      traditional-cpp trigraphs pedantic x o l c g L \
			      I D U O O2 C E H B b V M MD MM i dynamic \
			      nodtdlib static nostdinc undef)"/ \
		 	c/-l/f:*.a/ \
		 	n/*/f:*.{c,C,cc,o,a,s,i}/
    complete g++ 	n/*/f:*.{C,cc,o,s,i}/
    complete CC 	n/*/f:*.{C,cc,o,s,i}/
    complete rm 	c/--/"(directory force interactive verbose \
			recursive help version)"/ c/-/"(d f i v r R -)"/ \
			n/*/f:^*.{c,cc,C,h,in}/	# Protect precious files
    complete {vi,more,less} 	n/*/f:^*.{o,a,dvi,gz,z,Z}/
    complete bindkey    N/-a/b/ N/-c/c/ n/-[ascr]/'x:<key-sequence>'/ \
			n/-[svedlr]/n/ c/-[vedl]/n/ c/-/"(a s k c v e d l r)"/ \
			n/-k/"(left right up down)"/ p/2-/b/ \
			p/1/'x:<key-sequence or option>'/

    complete find 	n/-fstype/"(nfs 4.2)"/ n/-name/f/ \
		  	n/-type/"(c b d f p l s)"/ n/-user/u/ n/-group/g/ \
		  	n/-exec/c/ n/-ok/c/ n/-cpio/f/ n/-ncpio/f/ n/-newer/f/ \
		  	c/-/"(follow fstype name perm prune type user nouser \
		  	     group nogroup size inum atime mtime ctime exec \
			     ok print ls cpio ncpio newer xdev depth \
			     daystart follow maxdepth mindepth noleaf version \
			     anewer cnewer amin cmin mmin true false uid gid \
			     ilname iname ipath iregex links lname empty path \
			     regex used xtype fprint fprint0 fprintf \
			     print0 printf not a and o or)"/ \
			n/*/d/

    complete kill	c/-/S/ c/%/j/ \
			'n/*/`ps -h $LOGNAME | awk '"'"'{print $1}'"'"'`/'
    complete -%*	c/%/j/			# fill in the jobs builtin
    complete {fg,bg,stop}	c/%/j/ p/1/"(%)"//

    complete limit	c/-/"(h)"/ n/*/l/
    complete unlimit	c/-/"(h)"/ n/*/l/

    complete -co*	p/0/"(compress)"/	# make compress completion
						# not ambiguous
    complete nm		n/*/f:^*.{h,C,c,cc}/

    complete finger	c/*@/\$hosts/ n/*/u/@ 
    complete ping	p/1/\$hosts/
    complete traceroute	p/1/\$hosts/

    complete {talk,ntalk,phone,otalk}	p/1/'`users | tr " " "\012" | uniq`'/ \
		n/*/\`who\ \|\ grep\ \$:1\ \|\ awk\ \'\{\ print\ \$2\ \}\'\`/

    complete ftp	c/-/"(d i g n v)"/ n/-/\$hosts/ p/1/\$hosts/ n/*/n/
    complete ncftp	c/-/"(a I N)"/     n/-/\$hosts/ p/1/\$hosts/ n/*/n/

    # this one is simple...
    #complete rcp c/*:/f/ C@[./]*@f@ n/*/\$hosts/:
    # From Michael Schroeder: 
    # This one will rsh to the file to fetch the list of files!
    complete {r,s}cp 'c%*@*:%`set q=$:-0;set q="$q:s/@/ /";set q="$q:s/:/ /";set q=($q " ");rsh $q[2] -l $q[1] ls -dp $q[3]\*`%' 'c%*:%`set q=$:-0;set q="$q:s/:/ /";set q=($q " ");rsh $q[1] ls -dp $q[2]\*`%' 'c%*@%$hosts%:' 'C@[./$~]*@f@'  'n/*/$hosts/:'


    complete dd c/--/"(help version)"/ c/[io]f=/f/ \
		c/conv=*,/"(ascii ebcdic ibm block unblock \
			    lcase ucase swap noerror sync)"/,\
		c/conv=/"(ascii ebcdic ibm block unblock \
			  lcase ucase swap noerror sync)"/,\
	        c/*=/x:'<number>'/ \
		n/*/"(if of conv ibs obs bs cbs files skip file seek count)"/=

    complete nslookup   p/1/x:'<host>'/ p/2/\$hosts/

    complete ar c/[dmpqrtx]/"(c l o u v a b i)"/ p/1/"(d m p q r t x)"// \
		p/2/f:*.a/ p/*/f:*.o/

    complete {refile,sprev,snext,scan,pick,rmm,inc,folder,show} \
		c@+@F:$HOME/Mail/@

    # these and interrupt handling from Jaap Vermeulen <jaap@sequent.com>
    complete {rexec,rxexec,rxterm,rmterm} \
			'p/1/$hosts/' 'c/-/(l L E)/' 'n/-l/u/' 'n/-L/f/' \
			'n/-E/e/' 'n/*/c/'

    # these from Marc Horowitz <marc@cam.ov.com>
    complete attach 'n/-mountpoint/d/' 'n/-m/d/' 'n/-type/(afs nfs rvd ufs)/' \
		    'n/-t/(afs nfs rvd ufs)/' 'n/-user/u/' 'n/-U/u/' \
		    'c/-/(verbose quiet force printpath lookup debug map \
			  nomap remap zephyr nozephyr readonly write \
			  mountpoint noexplicit explicit type mountoptions \
			  nosetuid setuid override skipfsck lock user host)/' \
		    'n/-e/f/' 'n/*/()/'
    complete hesinfo	'p/1/u/' \
			'p/2/(passwd group uid grplist pcap pobox cluster \
			      filsys sloc service)/'

    # these from E. Jay Berkenbilt <ejb@ERA.COM>
    complete ./configure 'c/--*=/f/' 'c/--{cache-file,prefix,srcdir}/(=)//' \
			 'c/--/(cache-file verbose prefix srcdir)//'
    complete gs 'c/-sDEVICE=/(x11 cdjmono cdj550 epson eps9high epsonc \
			      dfaxhigh dfaxlow laserjet ljet4 sparc pbm \
			      pbmraw pgm pgmraw ppm ppmraw bit)/' \
		'c/-sOutputFile=/f/' 'c/-s/(DEVICE OutputFile)/=' \
		'c/-d/(NODISPLAY NOPLATFONTS NOPAUSE)/' 'n/*/f/'
    complete perl	'n/-S/c/'
    complete printenv	'n/*/e/'
    complete sccs	p/1/"(admin cdc check clean comb deledit delget \
			delta diffs edit enter fix get help info \
			print prs prt rmdel sccsdiff tell unedit \
			unget val what)"/

    # More completions from waz@quahog.nl.nuwc.navy.mil (Tom Warzeka)
    # this one works but is slow and doesn't descend into subdirectories
    # complete	cd	C@[./]*@d@ \
    #			p@1@'`\ls -1F . $cdpath | grep /\$ | sort -u`'@ n@*@n@

    if ( -r /etc/shells ) then
        complete setenv	p@1@e@ n@DISPLAY@\$hosts@: n@SHELL@'`cat /etc/shells`'@ 'c/*:/f/'
    else
	complete setenv	p@1@e@ n@DISPLAY@\$hosts@: 'c/*:/f/'
    endif

    # these conform to the latest GNU versions available at press time ...

    complete emacs	c/-/"(batch d f funcall i insert kill l load \
			no-init-file nw q t u user)"/ c/+/x:'<line_number>'/ \
			n/-d/x:'<display>'/ n/-f/x:'<lisp_function>'/ n/-i/f/ \
			n@-l@F:/usr/share/emacs/@ \
			n/-t/x:'<terminal>'/ \
			n/-u/u/ n/*/f:^*{[\#~],.dvi,.o,.gz,.Z,.z,.zip}/

    if (-r $HOME/.mailrc) then
	complete mail	c/-/"(e i f n s u v)"/ c/*@/\$hosts/ \
			c@+@F:$HOME/Mail@ C@[./\$~]@f@ n/-s/x:'<subject>'/ \
			n@-u@T:$_maildir@ n/-f/f/ \
			n@*@'`sed -n s/alias//p $HOME/.mailrc | tr -s " " "     " | cut -f 2`'@
    else
	complete mail	c/-/"(e i f n s u v)"/ c/*@/\$hosts/ \
			c@+@F:$HOME/Mail@ C@[./\$~]@f@ n/-s/x:'<subject>'/ \
			n@-u@T:$_maildir@ n/-f/f/ n/*/u/
    endif


    complete man	n@1@'`\ls -1AU ${_manpath}1/|& grep -v ':'|sed s%\\.1.\*\$%%|sort|uniq`'@ \
			n@2@'`\ls -1AU ${_manpath}2/|& grep -v ':'|sed s%\\.2.\*\$%%|sort|uniq`'@ \
			n@3@'`\ls -1AU ${_manpath}3/|& grep -v ':'|sed s%\\.3.\*\$%%|sort|uniq`'@ \
			n@4@'`\ls -1AU ${_manpath}4/|& grep -v ':'|sed s%\\.4.\*\$%%|sort|uniq`'@ \
			n@5@'`\ls -1AU ${_manpath}5/|& grep -v ':'|sed s%\\.5.\*\$%%|sort|uniq`'@ \
			n@6@'`\ls -1AU ${_manpath}6/|& grep -v ':'|sed s%\\.6.\*\$%%|sort|uniq`'@ \
			n@7@'`\ls -1AU ${_manpath}7/|& grep -v ':'|sed s%\\.7.\*\$%%|sort|uniq`'@ \
			n@8@'`\ls -1AU ${_manpath}8/|& grep -v ':'|sed s%\\.8.\*\$%%|sort|uniq`'@ \
			n@8@'`\ls -1AU ${_manpath}9/|& grep -v ':'|sed s%\\.8.\*\$%%|sort|uniq`'@ \
			n@n@'`\ls -1AU ${_manpath}n/|& grep -v ':'|sed s%\\.n.\*\$%%|sort|uniq`'@ \
	c/-/"(f k P s t)"/ n/-f/c/ n/-k/x:'<keyword>'/ n/-P/d/ n/*/c/

    complete xhost	c/[+-]/\$hosts/ n/*/\$hosts/

    complete zcat	c/--/"(force help license quiet version)"/ \
			c/-/"(f h L q V -)"/ n/*/f:*.{gz,Z,z,zip}/
    complete gzip	c/--/"(stdout to-stdout decompress uncompress \
			force help list license no-name quiet recurse \
			suffix test verbose version fast best)"/ \
			c/-/"(c d f h l L n q r S t v V 1 2 3 4 5 6 7 8 9 -)"/ \
			n/{-S,--suffix}/x:'<file_name_suffix>'/ \
			n/{-d,--{de,un}compress}/f:*.{gz,Z,z,zip,taz,tgz}/ \
			N/{-d,--{de,un}compress}/f:*.{gz,Z,z,zip,taz,tgz}/ \
			n/*/f:^*.{gz,Z,z,zip,taz,tgz}/
    complete {gunzip,ungzip} c/--/"(stdout to-stdout force help list license \
			no-name quiet recurse suffix test verbose version)"/ \
			c/-/"(c f h l L n q r S t v V -)"/ \
			n/{-S,--suffix}/x:'<file_name_suffix>'/ \
			n/*/f:*.{gz,Z,z,zip,taz,tgz,tar.gz}/
    complete zgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<limited_regular_expression>'/ \
			n/-*e/x:'<limited_regular_expression>'/ n/-*f/f/ n/*/f:*.gz/
    complete zegrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<full_regular_expression>'/ \
			n/-*e/x:'<full_regular_expression>'/ n/-*f/f/ n/*/f:*.gz/
    complete zfgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<fixed_string>'/ \
			n/-*e/x:'<fixed_string>'/ n/-*f/f/ n/*/f:*.gz/

    complete znew	c/-/"(f t v 9 P K)"/ n/*/f:*.Z/
    complete zmore	n/*/f:*.{gz,Z,z,zip}/
    complete zfile	n/*/f:*.{gz,Z,z,zip,taz,tgz,tar.gz}/
    complete ztouch	n/*/f:*.{gz,Z,z,zip,taz,tgz,tar.gz}/
    complete zforce	n/*/f:^*.{gz,taz,tgz,tar.gz}/

    complete grep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<limited_regular_expression>'/ \
			n/-*e/x:'<limited_regular_expression>'/ n/-*f/f/ n/*/f/
    complete egrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<full_regular_expression>'/ \
			n/-*e/x:'<full_regular_expression>'/ n/-*f/f/ n/*/f/
    complete fgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<fixed_string>'/ \
			n/-*e/x:'<fixed_string>'/ n/-*f/f/ n/*/f/
    complete users	c/--/"(help version)"/ p/1/x:'<accounting_file>'/

    complete who	c/--/"(heading mesg idle count help message version \
			writable)"/ c/-/"(H T w i u m q s -)"/ \
			p/1/x:'<accounting_file>'/ n/am/"(i)"/ n/are/"(you)"/

    complete chown	c/--/"(changes silent quiet verbose recursive help \
			version)"/ c/-/"(c f v R -)"/ C@[./\$~]@f@ c/*[.:]/g/ \
			n/-/u/. p/1/u/. n/*/f/

    complete chgrp	c/--/"(changes silent quiet verbose recursive help \
			version)"/ c/-/"(c f v R -)"/ n/-/g/ p/1/g/ n/*/f/

    complete cat	c/--/"(number-nonblank number squeeze-blank show-all \
			show-nonprinting show-ends show-tabs help version)"/ \
			c/-/"(b e n s t u v A E T -)"/ n/*/f/
    complete mv		c/--/"(backup force interactive update verbose suffix \
			version-control help version)"/ \
			c/-/"(b f i u v S V -)"/ \
			n/{-S,--suffix}/x:'<suffix>'/ \
			n/{-V,--version-control}/"(t numbered nil existing \
			never simple)"/ n/-/f/ N/-/d/ p/1/f/ p/2/d/ n/*/f/
    complete cp		c/--/"(archive backup no-dereference force interactive \
			link preserve symbolic-link update verbose parents \
			one-file-system recursive suffix version-control help \
			version)"/ c/-/"(a b d f i l p r s u v x P R S V -)"/ \
			n/-*r/d/ n/{-S,--suffix}/x:'<suffix>'/ \
			n/{-V,--version-control}/"(t numbered nil existing \
			never simple)"/ n/-/f/ N/-/d/ p/1/f/ p/2/d/ n/*/f/
    complete ln		c/--/"(backup directory force interactive symbolic \
			verbose suffix version-control help version)"/ \
			c/-/"(b d F f i s v S V -)"/ \
			n/{-S,--suffix}/x:'<suffix>'/ \
			n/{-V,--version-control}/"(t numbered nil existing \
			never simple)"/ n/-/f/ N/-/x:'<link_name>'/ \
			p/1/f/ p/2/x:'<link_name>'/
    complete touch	c/--/"(date file help time version)"/ \
			c/-/"(a c d f m r t -)"/ \
			n/{-d,--date}/x:'<date_string>'/ \
			c/--time/"(access atime mtime modify use)"/ \
			n/{-r,--file}/f/ n/-t/x:'<time_stamp>'/ n/*/f/

    complete tar	c/-[Acru]*/"(b B C f F g G h i l L M N o P \
			R S T v V w W X z Z)"/ \
			c/-[dtx]*/"( B C f F g G i k K m M O p P \
			R s S T v w x X z Z)"/ \
			p/1/"(A c d r t u x -A -c -d -r -t -u -x \
			--catenate --concatenate --create --diff --compare \
			--delete --append --list --update --extract --get)"/ \
			c/--/"(catenate concatenate create diff compare \
			delete append list update extract get atime-preserve \
			block-size read-full-blocks directory checkpoint file \
			force-local info-script new-volume-script incremental \
			listed-incremental dereference ignore-zeros \
			ignore-failed-read keep-old-files starting-file \
			one-file-system tape-length modification-time \
			multi-volume after-date newer old-archive portability \
			to-stdout same-permissions preserve-permissions \
			absolute-paths preserve record-number remove-files \
			same-order preserve-order same-owner sparse \
			files-from null totals verbose label version \
			interactive confirmation verify exclude exclude-from \
			compress uncompress gzip ungzip use-compress-program \
			block-compress)"/ \
			c/-/"(b B C f F g G h i k K l L m M N o O p P R s S \
			T v V w W X z Z 0 1 2 3 4 5 6 7 -)"/ \
			n/-c*{zf,fz}/x:'<new_gziped_tar_file, device_file, or "-">'/ \
			n/-c*f/x:'<new_tar_file, device_file, or "-">'/ \
			n/-[Adrtuxv]*{zf,fz}/f:*.{tar.gz,tgz}/ \
			n/{-[Adrtuxv]*f,--file}/f:*.tar/ \
			N/-x*{zf,fz}/'`tar -tzf $:-1`'/ \
			N/{-x*f,--file}/'`tar -tf $:-1`'/ \
			n/--use-compress-program/c/ \
			n/{-b,--block-size}/x:'<block_size>'/ \
			n/{-V,--label}/x:'<volume_label>'/ \
			n/{-N,--{after-date,newer}}/x:'<date>'/ \
			n/{-L,--tape-length}/x:'<tape_length_in_kB>'/ \
			n/{-C,--directory}/d/ \
			N/{-C,--directory}/'`\ls $:-1`'/ \
			n/-[0-7]/"(l m h)"/

    complete  mount	c/-/"(a n v t r w)"/ n/-t/"(minix iso9660 msdos vfat ext2 nfs proc)"/ \
    			'C@/de@F@' 'C@/*@F@@' 'n@*@`grep -E -v \(^#\|^\$\) /etc/fstab|awk \{\ print\ \$2\ \}`@'
    complete umount	c/-/"(a n t)"/   n/-t/"(minix iso9660 msdos ext2 nfs proc)"/ \
    			n/*/'`mount | cut -d " " -f 3`'/

    # these deal with NIS (formerly YP); if it's not running you don't need 'em
    complete domainname	p@1@D:$_ypdir@" " n@*@n@
    complete ypcat	c@-@"(d k t x)"@ n@-x@n@ n@-d@D:$_ypdir@" " \
	    N@-d@\`\\ls\ -1\ $_ypdir/\$:-1\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@ \
	  p/1/"(passwd group hosts networks)"/
    complete ypmatch	c@-@"(d k t x)"@ n@-x@n@ n@-d@D:$_ypdir@" " \
	            N@-d@x:'<key ...>'@ n@-@x:'<key ...>'@ p@1@x:'<key ...>'@ \
	  n@*@\`\\ls\ -1\ $_ypdir/$_domain\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@
    complete ypwhich	c@-@"(d m t x V1 V2)"@ n@-x@n@ n@-d@D:$_ypdir@" " \
	 n@-m@\`\\ls\ -1\ $_ypdir/$_domain\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@ \
			N@-m@n@ n@*@\$hosts@

    # there's no need to clutter the user's shell with these
    unset _maildir _ypdir _domain

    complete make \
	'n/-f/f/' \
      	'c/*=/f/' \
	'n@*@`cat -s GNUmakefile Makefile makefile |& sed -n -e "/No such file/d" -e "/^[^     #].*:/s/:.*//p"`@'

    if ( -f /etc/printcap ) then
	set printers=(`sed -n -e "/^[^     #].*:/s/:.*//p" /etc/printcap`)

	complete lpr    'c/-P/$printers/'
	complete lpq    'c/-P/$printers/'
	complete lprm   'c/-P/$printers/'
	complete lpquota        'p/1/(-Qprlogger)/' 'c/-P/$printers/'
    endif

    complete compress	c/-/"(c f v b)"/ n/-b/x:'<max_bits>'/ n/*/f:^*.Z/
    complete uncompress	c/-/"(c f v)"/                        n/*/f:*.Z/
    complete psompress	c/-/"(d c f)"/                        n/*/f:^*.Z/

    unset noglob
    unset _complete
#
    #
    # VI line editing
    #
    if ( ${?CSHEDIT} ) setenv CSHEDIT emacs
    if ( "$CSHEDIT" == "vi" ) then
	bindkey    -v
    else
	bindkey    "^[ "	magic-space
	bindkey    "^[!"	expand-history
    endif
    #
    # Common standard keypad and cursor
    #
    bindkey    "^[[2~"		yank
    bindkey    "^[[3~"		delete-char
    bindkey    "^[[5~"		up-history
    bindkey    "^[[6~"		down-history
    bindkey    "^[[C"		forward-char
    bindkey    "^[[D"		backward-char
    bindkey    "^[[A"		up-history
    bindkey    "^[[B"		down-history
    if ( ! ${?TERM} ) setenv TERM linux
    if ( "$TERM" == "xterm" ) then
	bindkey -c "^[[E"	"source /etc/csh.cshrc"
    else
	bindkey -c "^[[G"	"source /etc/csh.cshrc"
    endif
    #
    # Avoid network problems
    #   ... \177 (ASCII-DEL) and \010 (ASCII-BS)
    #       do `backward-delete-char'
    # Note: `delete-char' is maped to \033[3~
    #       Therefore xterm's responce on pressing
    #       key Delete or KP-Delete should be
    #       \033[3~ ... NOT \177
    #
    bindkey    "^?"		backward-delete-char
    bindkey    "^H"		backward-delete-char
    #
    # Home and End
    #
    if ( "$TERM" == "xterm" ) then
	#
	# Normal keypad and cursor of xterm
	#
	bindkey    "^[[1~"	history-search-backward
	bindkey    "^[[4~"	set-mark-command
	bindkey    "^[[H"	beginning-of-line
	bindkey    "^[[F"	end-of-line
	# Home and End of application keypad and cursor of xterm
	bindkey    "^[OH"	beginning-of-line
	bindkey    "^[OF"	end-of-line
    else
	#
	# TERM=linux or console
	#
	bindkey    "^[[1~"	beginning-of-line
	bindkey    "^[[4~"	end-of-line
    endif
    #
    # Application keypad and cursor of xterm
    #
    if ( "$TERM" == "xterm" ) then
	bindkey    "^[OD"	backward-char
	bindkey    "^[OC"	forward-char
	bindkey    "^[OA"	up-history
	bindkey    "^[OB"	down-history
	bindkey -c "^[OE"	"source /etc/csh.cshrc"
	# DEC keyboard KP_F1 - KP_F4
	bindkey -s "^[OP"	"^["
	bindkey -s "^[OQ"	"^[OA^[OB"
	bindkey    "^[OR"	undefined-key  
	bindkey    "^[OS"	kill-line
    endif
    #
    # Function keys F1 - F12
    #
    if ( "$TERM" == "linux" ) then
	#
	# On console the first five function keys
	#
	bindkey    "^[[[A"	undefined-key
	bindkey    "^[[[B"	undefined-key
	bindkey    "^[[[C"	undefined-key
	bindkey    "^[[[D"	undefined-key
	bindkey    "^[[[E"	undefined-key
    else
	#
	# The first five standard function keys
	#
	bindkey    "^[[11~"	undefined-key
	bindkey    "^[[12~"	undefined-key
	bindkey    "^[[13~"	undefined-key
	bindkey    "^[[14~"	undefined-key
	bindkey    "^[[15~"	undefined-key
    endif
    bindkey    "^[[17~"		undefined-key
    bindkey    "^[[18~"		undefined-key
    bindkey    "^[[19~"		undefined-key
    bindkey    "^[[20~"		undefined-key
    bindkey    "^[[21~"		undefined-key
    # Note: F11, F12 are identical with Shift_F1 and Shift_F2
    bindkey    "^[[23~"		undefined-key
    bindkey    "^[[24~"		undefined-key
    #
    # Shift Function keys F1  - F12
    #      identical with F11 - F22
    #
    # bindkey   "^[[23~"	undefined-key
    # bindkey   "^[[24~"	undefined-key
    bindkey    "^[[25~"		undefined-key
    bindkey    "^[[26~"		undefined-key
    # DEC keyboard: F15=^[[28~ is Help
    bindkey    "^[[28~"		undefined-key
    # DEC keyboard: F16=^[[29~ is Menu
    bindkey    "^[[29~"		undefined-key
    bindkey    "^[[31~"		undefined-key
    bindkey    "^[[32~"		undefined-key
    bindkey    "^[[33~"		undefined-key
    bindkey    "^[[34~"		undefined-key
    if ( "$TERM" == "xterm" ) then
	# Not common
	bindkey    "^[[35~"	undefined-key
	bindkey    "^[[36~"	undefined-key
    endif
    #
    if ( "$TERM" == "xterm" ) then
	#
	# Application keypad and cursor of xterm
	# with NumLock ON
	#
	# Operators
	bindkey -s "^[Oo"	"/"
	bindkey -s "^[Oj"	"*"
	bindkey -s "^[Om"	"-"
	bindkey -s "^[Ok"	"+"
	# Change in xterm X11R6.3
	bindkey -s "^[Ol"	"+"
	bindkey    "^[OM"	newline
	# Colon and dot
	# bindkey -s "^[Ol"	","
	bindkey -s "^[On"	"."
	# Numbers
	bindkey -s "^[Op"	"0"
	bindkey -s "^[Oq"	"1"
	bindkey -s "^[Or"	"2"
	bindkey -s "^[Os"	"3"
	bindkey -s "^[Ot"	"4"
	bindkey -s "^[Ou"	"5"
	bindkey -s "^[Ov"	"6"
	bindkey -s "^[Ow"	"7"
	bindkey -s "^[Ox"	"8"
	bindkey -s "^[Oy"	"9"
    endif
    #
    #  EMACS line editing
    #
    if ( "$CSHEDIT" == "emacs" ) then 
	#
	# ... xterm application cursor
	#
    	if ( "$TERM" == "xterm" ) then
	     bindkey    "^[^[OD"	backward-word
	     bindkey    "^[^[OC"	forward-word
	     bindkey    "^[^[OA"	up-history
	     bindkey    "^[^[OB"	down-history
	     bindkey    "^^[OD"		backward-char
	     bindkey    "^^[OC"		forward-char
	     bindkey    "^^[OA"		up-history
	     bindkey    "^^[OB"		down-history
    	endif
	#
	# Standard cursor
	#
	bindkey    "^[^[[D"	backward-word
	bindkey    "^[^[[C"	forward-word
	bindkey    "^[^[[A"	up-history
	bindkey    "^[^[[B"	down-history
	bindkey    "^^[[D"	backward-char
	bindkey    "^^[[C"	forward-char
	bindkey    "^^[[A"	up-history
	bindkey    "^^[[B"	down-history
    endif
    #
    # end key binding
    #
#
#
endif # if ($?_complete)
end:
    onintr
##
