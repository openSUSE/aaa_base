#include <sys/types.h> /* Defines the macros major and minor */
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "libconsole.h"

/*
 * Internal logger
 */
static char *myname = NULL;
static void _logger (const char *fmt, va_list ap)
{
    char buf[strlen(myname)+2+strlen(fmt)+1];
    strcat(strcat(strcpy(buf, myname), ": "), fmt);
    vfprintf(stderr, buf, ap);
    return;
}

/*
 * Cry and exit.
 */
void error (const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    _logger(fmt, ap);
    va_end(ap);
    exit (1);
}

/*
 * Warn the user.
 */
void warn (const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    _logger(fmt, ap);
    va_end(ap);
    return;
}

/*
 * Now do the job
 */
int main(int argc, char *argv[])
{
    char * tty;
    myname = basename(*argv);

    if (!strcmp(myname, "setconsole")) {
	int fdc;

	if (argc != 2)
	    error("Need a tty device name as argument.\n");

	if ((fdc = open(argv[1], O_WRONLY|O_NONBLOCK)) < 0)
	    error("can not open %s: %s\n", argv[1], strerror(errno));

	if (!isatty(fdc))
	    error("%s is not a tty.\n", argv[1]);

	(void)ioctl(0, TIOCCONS, NULL);	/* Undo any current map if any */
	if (ioctl(fdc, TIOCCONS, NULL) < 0)
	    error("can not set console device: %s\n", strerror(errno));

	close(fdc);
	goto out;
    }
    tty = fetchtty(getpid(), getppid());
    printf("%s\n", tty);
out:
    return 0;
}
