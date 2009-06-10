# malloc debugging helpers (see man malloc), remove again for RC1 - meissner@suse.de
if test ! -d /.buildenv ; then
  # disable for now in build environments - too many unknown problems (bnc#509398)
  export MALLOC_CHECK_=3
fi
export MALLOC_PERTURB_=E
