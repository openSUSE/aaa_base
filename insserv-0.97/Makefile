#
# Makefile for compiling insserv tool
#
# Author: Werner Fink,  <werner@suse.de>
#

INITDIR  =	/etc/init.d
#INITDIR  =	/sbin/init.d
INSCONF  =	/etc/insserv.conf
#DEBUG	 =	-DDEBUG=1
#DESTDIR =	/tmp/root
DEBUG	 =
DESTDIR	 =
VERSION	 =	0.97
DATE	 =	$(shell date +'%d%b%y' | tr '[:lower:]' '[:upper:]')

#
# Architecture
#
	   ARCH = $(shell uname -m | sed 's@\(i\)[34567]\(86\)@\13\2@')
#
# egcs used with -O2 includes -fno-force-mem which is/was buggy (1998/10/08)
#
ifdef RPM_OPT_FLAGS
	  COPTS = $(RPM_OPT_FLAGS)
else
ifeq ($(ARCH),i386)
	  COPTS = -O2 -mcpu=i486 -fomit-frame-pointer -fschedule-insns2
else
	  COPTS = -O2 -fomit-frame-pointer -fschedule-insns2
endif
endif
	 CFLAGS = -Wall $(COPTS) $(DEBUG) -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
		  -DSUSE -DINITDIR=\"$(INITDIR)\" -DINSCONF=\"$(INSCONF)\" -pipe
	  CLOOP = -funroll-loops
	     CC = gcc
	     RM = rm -f
	  MKDIR = mkdir -p
	  RMDIR = rm -rf
   INSTBINFLAGS = -s -m 0700
	INSTBIN = install $(INSTBINFLAGS)
   INSTSRPFLAGS = -m 0700
	INSTSRP = install $(INSTSRPFLAGS)
   INSTDOCFLAGS = -c -m 0444
	INSTDOC = install $(INSTDOCFLAGS)
   INSTCONFLAGS = -c -m 0644
	INSTCON = install $(INSTDOCFLAGS)
	   LINK = ln -sf

#
	SDOCDIR = $(DESTDIR)/usr/share/man/man8
	SBINDIR = $(DESTDIR)/sbin
	CONFDIR = $(DESTDIR)/etc
	 LSBDIR = $(DESTDIR)/lib/lsb
      USRLSBDIR = $(DESTDIR)/usr/lib/lsb
#
#
#
TODO	=	insserv

all: $(TODO)

listing.o:	listing.c listing.h
	$(CC) $(CFLAGS) $(CLOOP) -c $<

insserv:	insserv.c listing.o
	$(CC) $(CFLAGS) $(CLOOP) -o $@ $^

clean:
	$(RM) *.o *~ insserv

install:	$(TODO)
	$(MKDIR)   $(SBINDIR)
	$(MKDIR)   $(SDOCDIR)
	$(MKDIR)   $(CONFDIR)
	$(MKDIR)   $(LSBDIR)
	$(MKDIR)   $(DESTDIR)/usr/lib
	$(MKDIR)   $(USRLSBDIR)
	$(INSTBIN) insserv        $(SBINDIR)/
	$(INSTDOC) insserv.8      $(SDOCDIR)/
	$(INSTCON) insserv.conf   $(CONFDIR)/
	$(INSTCON) init-functions $(LSBDIR)/
	$(INSTSRP) install_initd  $(USRLSBDIR)/
	$(INSTSRP) remove_initd   $(USRLSBDIR)/

#
# Make distribution
#
FILES	= README         \
	  COPYING        \
	  Makefile       \
	  listing.c      \
	  listing.h      \
	  insserv.8      \
	  insserv.c      \
	  insserv.conf   \
	  init-functions \
	  remove_initd   \
	  install_initd  \
	  insserv-$(VERSION).lsm

dest:
	$(MKDIR) insserv-$(VERSION)
	@echo -e 'Begin3\n\
Title:		insserv tool for boot scripts\n\
Version:	$(VERSION)\n\
Entered-date:	$(DATE)\n\
Description:	Used for enabling of installed boot scripts\n\
x 		by scanning comment headers which are LSB conform.\n\
Keywords:	boot service control, LSB\n\
Author:		Werner Fink <werner@suse.de>\n\
Maintained-by:	Werner Fink <werner@suse.de>\n\
Primary-site:	sunsite.unc.edu /pub/Linux/system/daemons/init\n\
x		@UNKNOWN insserv-$(VERSION).tar.gz\n\
Alternate-site:	ftp.suse.com /pub/projects/init\n\
Platforms:	Linux with System VR2 or higher boot scheme\n\
Copying-policy:	GPL\n\
End' | sed 's@^ @@g;s@^x@@g' > insserv-$(VERSION).lsm
	cp $(FILES) insserv-$(VERSION)
	tar -c -zf  insserv-$(VERSION).tar.gz insserv-$(VERSION)/
	$(RMDIR)    insserv-$(VERSION)
	set -- `gzip -l insserv-$(VERSION).tar.gz | tail -1` ; \
	sed "s:@UNKNOWN:$$1:" < insserv-$(VERSION).lsm > \
	insserv-$(VERSION).lsm.tmp ; \
	mv insserv-$(VERSION).lsm.tmp insserv-$(VERSION).lsm
