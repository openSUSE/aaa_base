CFLAGS=-O2 -g -Wall
CPPFLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=-Wl,-z,relro,-z,now

bin_PROGRAMS=get_kernel_version locale-check

all: $(bin_PROGRAMS)

install: all
	cp -a files/* $(DESTDIR)/
	install -m755 get_kernel_version $(DESTDIR)/usr/bin
	install -m755 locale-check $(DESTDIR)/usr/bin

clean:
	rm -f $(bin_PROGRAMS)

mimetypes:
	if test -d Apache/apache2; then (cd Apache/apache2 && osc up); else osc co Apache/apache2; fi
	tar --wildcards -Oxjf Apache/apache2/httpd-*.tar.bz2 '*/docs/conf/mime.types' > mime.types.apache
	./mimetypemerge files/etc/mime.types mime.types.apache > mime.types
	mv mime.types files/etc/mime.types

.PHONY: all install clean package mimetypes
