#
# For interactive debugging of the startup process. If set
# to "yes" the system will ask whether to confirm every
# step of the boot process.
#
PROMPT_FOR_CONFIRM="no"

#
# For interactive debugging of the startup process. How long
# wait before the default answer is assumed.
#
CONFIRM_PROMPT_TIMEOUT="5"

#
# If set to "yes" this enables to stop the boot process by
# pressing Ctrl-S and continue with Ctrl-Q (xon/xoff
# flow control).
#
FLOW_CONTROL="no"

#
# This  variable will limits the maximum number of file system
# checkers that can be running at one time.  This allows configurations
# which have a large number of disks to avoid fsck starting too many file
# system checkers at once, which might overload CPU and memory resources
# available on the system.
#
FSCK_MAX_INST=10
