z_ls ()
{
    local IFS=' '
    command \ls $=LS_OPTIONS ${1+"$@"}
}
alias ls=z_ls
