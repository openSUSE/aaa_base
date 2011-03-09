CFLAGS=-O2 -g -Wall
CPPFLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=-Wl,-z,relro,-z,now

sbin_PROGRAMS=get_kernel_version

all: $(sbin_PROGRAMS)

install: all
	cp -a files/* $(DESTDIR)/
	install -m755 get_kernel_version $(DESTDIR)/sbin

clean:
	rm -f $(sbin_PROGRAMS)

.PHONY: all install clean
