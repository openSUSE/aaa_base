# malloc debugging helpers (see man malloc), remove again for RC1 - meissner@suse.de
# disable for now in build environments - too many unknown problems (bnc#509398)
#setenv MALLOC_CHECK_ 3
setenv MALLOC_PERTURB_ E
