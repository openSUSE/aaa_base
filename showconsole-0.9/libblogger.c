#ifndef  _PATH_BLOG_FIFO
# define _PATH_BLOG_FIFO	"/dev/blog"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <paths.h>
#include <errno.h>
#include "libblogger.h"

/*
 * Use Esacape sequences which are handled by console
 * driver of linux kernel but not used.  We transport
 * with this our informations to the parser of blogd
 * whereas the linux console throw them away.
 * Problem: Does not work on serial console.
 */

#define ESNN	"<notice>"	/* Notice  */
#define ESND	"<done>"	/* Done    */
#define ESNF	"<failed>"	/* Failed  */
#define ESNS	"<skipped>"	/* Skipped */
#define ESNU	"<unused>"	/* Unused  */

static int fdfifo = -1;
static char * fifo_name = _PATH_BLOG_FIFO;

static int bootlog_init(const int lvl)
{
    int ret = -1;
    struct stat st;

    if (stat(fifo_name, &st))
	goto out;

    if (!S_ISFIFO(st.st_mode))
	goto out;

    if ((fdfifo = open(fifo_name, O_WRONLY|O_NONBLOCK)) < 0)
	goto out;

    ret = 0;
out:
    return ret;
}

int bootlog(const int lvl, const char *fmt, ...)
{
    va_list ap;
    int ret = -1;
    char * head = ESNN;
    char buf[4096];

    if (fdfifo < 0 && bootlog_init(lvl) < 0)
	goto out;

    ret = 0;
    switch (lvl) {
	case -1:
	    head = NULL;
	    break;
	case B_NOTICE:
	    head = ESNN;
	    break;
	case B_DONE:
	    head = ESND;
	    break;
	case B_FAILED:
	    head = ESNF;
	    break;
	case B_SKIPPED:
	    head = ESNS;
	    break;
	case B_UNUSED:
	    head = ESNU;
	    break;
	default:
	    head = ESNN;
	    break;
    }

    if (head)
	write(fdfifo, head, strlen(head));
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    write(fdfifo, buf, strlen(buf));
out:
    return ret;
}
