# 
# This is for FACTORY only and should be removed for the release.
# - meissner@suse.de
#
# fill new malloc areas with 0x44
export MALLOC_PERTURB_=D
# Abort on any malloc related error.
export MALLOC_CHECK_=2
