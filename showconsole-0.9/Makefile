#
# Makefile for compiling console tools
#
# Author: Werner Fink,  <werner@suse.de>
#

LOG_BUFFER_SIZE	= 65536
BOOT_LOGFILE	= /var/log/boot.msg
BOOT_FIFO	= /dev/blog

#DEBUG	 =	-DDEBUG=1
#DESTDIR =	/tmp/root
DEBUG	 =
DESTDIR	 =
VERSION	 =	0.9
DATE	 =	$(shell date +'%d%b%y' | tr '[:lower:]' '[:upper:]')

#
# Architecture
#
	   ARCH = $(shell uname -m | sed 's@\(i\)[34567]\(86\)@\13\2@')
#
# egcs used with -O2 includes -fno-force-mem which is/was buggy (1998/10/08)
#
ifeq ($(ARCH),i386)
	  COPTS = -mcpu=i486 -fomit-frame-pointer -fschedule-insns2
else
	  COPTS = -fomit-frame-pointer -fschedule-insns2
endif
	 CFLAGS = -O2 $(COPTS) $(DEBUG) -D_GNU_SOURCE \
		  -DLOG_BUFFER_SIZE=$(LOG_BUFFER_SIZE) \
		  -DBOOT_LOGFILE=\"$(BOOT_LOGFILE)\" \
		  -D_PATH_BLOG_FIFO=\"$(BOOT_FIFO)\" -Wall -pipe
	  CLOOP = -funroll-loops
ifeq ($(BLOGGER),1)
	 CFLAGS := $(CFLAGS) -DBLOGGER
endif
	     CC = gcc
	     RM = rm -f
	  MKDIR = mkdir -p
	  RMDIR = rm -rf
   INSTBINFLAGS = -s -m 0700
	INSTBIN = install $(INSTBINFLAGS)
   INSTDOCFLAGS = -c -m 0444
	INSTDOC = install $(INSTDOCFLAGS)
   INSTCONFLAGS = -c -m 0644
	INSTCON = install $(INSTDOCFLAGS)
	   LINK = ln -sf
	     AR = ar rusv
	     SO = echo .so man8/

#
	SDOCDIR = $(DESTDIR)/usr/share/man/man8
	SBINDIR = $(DESTDIR)/sbin
	CONFDIR = $(DESTDIR)/etc
	 LSBDIR = $(DESTDIR)/usr/lib/lsb
	 LIBDIR = $(DESTDIR)/usr/lib
	 INCDIR = $(DESTDIR)/usr/include
#
#
#
TODO	=	showconsole blogd blogger

all: $(TODO)

libconsole.o:	libconsole.c libconsole.h
	$(CC) $(CFLAGS) $(CLOOP) -c $<

libblogger.o:	libblogger.c libblogger.h
	$(CC) $(CFLAGS) $(CLOOP) -c $<

libblogger.a:	libblogger.o
	$(AR) $@ $^

showconsole:	showconsole.c libconsole.o
	$(CC) $(CFLAGS) $(CLOOP) -o $@ $^

blogd:	blogd.c libconsole.o
	$(CC) $(CFLAGS) $(CLOOP) -o $@ $^ -lutil

blogger:	blogger.c libblogger.a
	$(CC) $(CFLAGS) $(CLOOP) -o $@ $^

clean:
	$(RM) *.o *.a *~ showconsole blogd blogger

install:	$(TODO)
	$(MKDIR)   $(SBINDIR)
	$(MKDIR)   $(SDOCDIR)
	$(MKDIR)   $(CONFDIR)
	$(MKDIR)   $(LSBDIR)
	$(MKDIR)   $(LIBDIR)
	$(MKDIR)   $(INCDIR)
	$(INSTBIN) showconsole   $(SBINDIR)/
	$(LINK)    showconsole   $(SBINDIR)/setconsole
	$(INSTBIN) blogger       $(SBINDIR)/
	$(INSTDOC) blogger.8     $(SDOCDIR)/
	$(INSTDOC) showconsole.8 $(SDOCDIR)/
	$(SO)showconsole.8 >     $(SDOCDIR)/setconsole.8
	$(INSTBIN) blogd         $(SBINDIR)/
	$(INSTDOC) blogd.8       $(SDOCDIR)/
	$(INSTCON) libblogger.a  $(LIBDIR)/
	$(INSTCON) libblogger.h  $(INCDIR)/

#
# Make distribution
#
FILES	= README         \
	  COPYING        \
	  OTHERS         \
	  Makefile       \
	  libblogger.c   \
	  libblogger.h   \
	  libconsole.c   \
	  libconsole.h   \
	  showconsole.8  \
	  showconsole.c  \
	  blogd.c        \
	  blogd.8        \
	  blogger.c      \
	  blogger.8      \
	  showconsole-$(VERSION).lsm

dest:
	$(MKDIR) showconsole-$(VERSION)
	@echo -e 'Begin3\n\
Title:		console tools for boot scripts\n\
Version:	$(VERSION)\n\
Entered-date:	$(DATE)\n\
Description:	Used for fetch the real device in boot scripts\n\
x 		running on /dev/console.\n\
Keywords:	boot control\n\
Author:		Werner Fink <werner@suse.de>\n\
Maintained-by:	Werner Fink <werner@suse.de>\n\
Primary-site:	sunsite.unc.edu /pub/Linux/system/daemons/init\n\
x		@UNKNOWN showconsole-$(VERSION).tar.gz\n\
Alternate-site:	ftp.suse.com /pub/projects/init\n\
Platforms:	Linux with System VR2 or higher boot scheme\n\
Copying-policy:	GPL\n\
End' | sed 's@^ @@g;s@^x@@g' > showconsole-$(VERSION).lsm
	cp $(FILES) showconsole-$(VERSION)
	tar -c -zf  showconsole-$(VERSION).tar.gz showconsole-$(VERSION)/
	$(RMDIR)    showconsole-$(VERSION)
	set -- `gzip -l showconsole-$(VERSION).tar.gz | tail -1` ; \
	sed "s:@UNKNOWN:$$1:" < showconsole-$(VERSION).lsm > \
	showconsole-$(VERSION).lsm.tmp ; \
	mv showconsole-$(VERSION).lsm.tmp showconsole-$(VERSION).lsm
