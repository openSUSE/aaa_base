CFLAGS=-O2 -g -Wall
CPPFLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=-Wl,-z,relro,-z,now

bin_PROGRAMS=get_kernel_version

all: $(bin_PROGRAMS)

install: all
	cp -a files/* $(DESTDIR)/
	install -m755 get_kernel_version $(DESTDIR)/usr/bin

clean:
	rm -f $(sbin_PROGRAMS)

package:
	obs/mkpackage

.PHONY: all install clean package
