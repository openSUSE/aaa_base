all:

RUN  = /etc/init.d/rc${RUNLEVEL}.d
PRE  = /etc/init.d/rc${PREVLEVEL}.d
BOOT = /etc/init.d/boot.d
include /etc/init.d/.depend.${MODE}

all: ${TARGETS}

${TARGETS}:
ifeq "${MODE}" "start"
	@test ! -e ${PRE}/K??$@ -a -e ${RUN}/S??$@ && ${RUN}/S??$@ start ||:
else
ifeq "${MODE}" "stop"
	@test -e ${PRE}/K??$@ -a ! -e ${RUN}/S??$@ && ${PRE}/K??$@ stop ||:
else
ifeq "${MODE}" "boot"
	@test -e ${BOOT}/S??$@ && ${BOOT}/S??$@ start ||:
endif
endif
endif
.PHONY: all test ${TARGETS}
