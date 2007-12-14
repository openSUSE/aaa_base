" ~/.exrc	Configuration for several
"		vi's derivates in Ex mode
" Autor: Klaus Franken <http://www.suse.de/feedback>
" Version: 06.06.97

" Do not use the indent from the current
" line when starting a new line
set noautoindent

" Ignore case in search patterns
" set ignorecase

" Show the line and column number of the
" current cursor position
set ruler

" Shell to start with !
" set shell=sh

" Show matching bracket if seen on screen
set showmatch

" Show the current editing mode which is
" INSERT or REPLACE
set showmode

" Keyboard mapping for several vi derivates
" Autor: Werner Fink   <werner@suse.de> 
" Version: 20.05.1997

" keys in display mode
map OA  k
map [A  k
map OB  j
map [B  j
map OD  h
map [D  h
map     h
map     h
map OC  l
map [C  l
map [2~ i
map [3~ x
map [1~ 0
map OH  0
map [H  0
map [4~ $
map OF  $
map [F  $
map [5~ 
map [6~ 
map [E  ""
map [G  ""
map OE  ""
map Oo  :
map Oj  *
map Om  -
map Ok  +
map Ol  +
map OM  
map Ow  7
map Ox  8
map Oy  9
map Ot  4
map Ou  5
map Ov  6
map Oq  1
map Or  2
map Os  3
map Op  0
map On  .

" keys in insert mode
map! Oo  :
map! Oj  *
map! Om  -
map! Ok  +
map! Ol  +
map! OM  
map! Ow  7
map! Ox  8
map! Oy  9
map! Ot  4
map! Ou  5
map! Ov  6
map! Oq  1
map! Or  2
map! Os  3
map! Op  0
map! On  .
