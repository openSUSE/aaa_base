# 
# This is for FACTORY only and should be removed for the release.
# - meissner@suse.de
#
# fill new malloc areas with 0x43
export MALLOC_PERTURB_=C
# Abort on any malloc related error.
export MALLOC_CHECK_=2
