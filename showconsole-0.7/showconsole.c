#include <sys/types.h> /* Defines the macros major and minor */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
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

    tty = fetchtty(getpid(), getppid());
    printf("%s\n", tty);

    return 0;
}
