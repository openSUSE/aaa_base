CFLAGS=-O2 -g -Wall
CPPFLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=-Wl,-z,relro,-z,now

bin_PROGRAMS=get_kernel_version

all: $(bin_PROGRAMS)

install: all
	cp -a files/* $(DESTDIR)/
	install -m755 get_kernel_version $(DESTDIR)/usr/bin
ifneq ($(filter i%86 armv%,$(RPM_ARCH)),)
	rm -vf $(DESTDIR)/usr/lib/sysctl.d/50-pid-max.conf
endif

clean:
	rm -f $(bin_PROGRAMS)

mimetypes:
	if test -d Apache/apache2; then (cd Apache/apache2 && osc up); else osc co Apache/apache2; fi
	tar --wildcards -Oxjf Apache/apache2/httpd-2*.tar.bz2 '*/docs/conf/mime.types' > mime.types.apache
	./mimetypemerge files/etc/mime.types mime.types.apache > mime.types
	mv mime.types files/etc/mime.types

rpm:
	rpmbuild -bb --build-in-place --noprep aaa_base.spec

.PHONY: all install clean package mimetypes rpm
