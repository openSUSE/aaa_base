#ifndef  LOG_BUFFER_SIZE
# define LOG_BUFFER_SIZE	65536
#endif
#ifndef  BOOT_LOGFILE
# define BOOT_LOGFILE		"/var/log/boot.msg"
#endif
#ifndef  _PATH_BLOG_FIFO
# define _PATH_BLOG_FIFO	"/dev/blog"
#endif
#include <sys/time.h>
#include <sys/types.h> /* Defines the macros major and minor */
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include "libconsole.h"

extern void error (const char *fmt, ...);
extern void warn  (const char *fmt, ...);

/*
 * push and pod direcotry changes
 */
static char pwd[NAME_MAX+1];
void pushd(const char * path)
{
    if (!getcwd(pwd, sizeof(pwd)))
	error ("can not get current directory: %s\n", strerror(errno));

    if (chdir(path) < 0)
	error ("can not change directory: %s\n", strerror(errno));
}

void popd(void)
{
    if (pwd[0])
	if (chdir(pwd) < 0)
	    error ("can not change directory: %s\n", strerror(errno));
    pwd[0] = '\0';
}

/*
 * Follow the link to its full deep, this because
 * to get the real file name stored in lnk[].
 */
static char lnk[NAME_MAX+1];
int rlstat(char ** file, struct stat *st)
{
    int ret = -1;

    if (lstat(*file, st) < 0)
	goto out;

    do {
	size_t cnt;

	if (!S_ISLNK(st->st_mode))
	    break;

	if ((cnt = readlink(*file, lnk, sizeof(lnk))) < 0)
	    goto out;
	lnk[cnt] = '\0';
	*file = lnk;

	if (lstat(*file, st) < 0)
	    goto out;

    } while (S_ISLNK(st->st_mode));
    ret = 0;
    if (*file != (void *)&lnk) {
	strcpy(lnk, *file);
	*file = lnk;
    }
out:
    return ret;
}

/*
 * Arg used: safe out
 */
static void (*vc_reconnect)(int fd) = NULL;
static inline void safeout (int fd, const char *ptr, size_t s)
{
    int saveerr = errno;

    while (s > 0) {
	ssize_t p = write (fd, ptr, s);
	if (p < 0) {
	    if (errno == EPIPE)
		exit (0);
	    if (errno == EINTR || errno == EAGAIN) {
		errno = 0;
		continue;
	    }
	    if (errno == EIO && vc_reconnect) {
		(*vc_reconnect)(fd);
		vc_reconnect = NULL;
		errno = 0;
		continue;
	    }
	    error("Can not write to fd %d: %s\n", fd, strerror(errno));
	}
	ptr += p;
	s -= p;
    }
    errno = saveerr;
}

/*
 * Twice used: safe in
 */
static inline ssize_t safein  (int fd, char *ptr, size_t s)
{
    int saveerr = errno;
    ssize_t r = 0;
    size_t  t;

    if ((ioctl(fd, FIONREAD, (int*)&t) < 0) || (t == 0)) {
	fd_set check;
	struct timeval zero = {0, 0};

	do {
	    FD_ZERO (&check);
	    FD_SET (fd, &check);

	    /* Avoid deadlock: do not read if nothing is in there */
	    if (select(fd + 1, &check, (fd_set*)0, (fd_set*)0, &zero) <= 0)
		break;

	    r = read (fd, ptr, s);

	} while (r < 0 && (errno == EINTR || errno == EAGAIN));

	/* Do not exit on a broken FIFO */
	if (r < 0 && errno != EPIPE)
	    error("Can not read from fd %d: %s\n", fd, strerror(errno));

	goto out;
    }

    while (t > 0) {
	ssize_t p = read (fd, ptr, t);
	if (p < 0) {
	    if (errno == EINTR || errno == EAGAIN) {
		errno = 0;
		continue;
	    }
	    error("Can not read from fd %d: %s\n", fd, strerror(errno));
	}
	ptr += p;
	r += p;
	t -= p;
    }
out:
    errno = saveerr;
    return r;
}

/*
 * Remove Escaped sequences and write out result (see
 * linux/drivers/char/console.c in do_con_trol()).
 */
enum {	ESnormal, ESesc, ESsquare, ESgetpars, ESgotpars, ESfunckey,
	EShash, ESsetG0, ESsetG1, ESpercent, ESignore, ESnonstd,
	ESpalette };
#define NPAR 16
static unsigned int state = ESnormal;
static int npar = 0, nl = 0;

static void parselog(FILE * log, const char *buf, const size_t s)
{
    int c;
    ssize_t r = s;

    while (r > 0) {
	c = (unsigned char)*buf;

	switch(state) {
	case ESnormal:
	default:
	    state = ESnormal;
	    switch (c) {
	    case 0  ...  8:
	    case 16 ... 23:
	    case 25:
	    case 28 ... 31:
		nl = 0;
		fprintf(log, "^%c", c + 64);
		break;
	    case '\n':
		nl = 1;
		fputc(c, log);
		break;
	    case '\r':
	    case 14:
	    case 15:
		/* ^N and ^O used in xterm for rmacs/smacs  *
		 * on console \033[10m and \033[11m is used */
	    case 24:
	    case 26:
		break;
	    case '\033':
		state = ESesc;
		break;
	    case '\t':
	    case  32 ... 126:
	    case 160 ... 255:
		nl = 0;
		fputc(c, log);
		break;
	    case 127:
		nl = 0;
		fprintf(log, "^?");
		break;
	    case 128 ... 128+26:
	    case 128+28 ... 159:
		nl = 0;
		fprintf(log, "\\%03o", c);
		break;
	    case 128+27:
		state = ESsquare;
		break;
	    default:
		nl = 0;
		fprintf(log, "0x%X", c);
		break;
	    }
	    break;
	case ESesc:
	    state = ESnormal;
	    switch((unsigned char)c) {
	    case '[':
		state = ESsquare;
		break;
	    case ']':
		state = ESnonstd;
		break;
	    case '%':
		state = ESpercent;
		break;
	    case 'E':
	    case 'D':
		nl = 1;
		fputc('\n', log);
		break;
	    case '(':
		state = ESsetG0;
		break;
	    case ')':
		state = ESsetG1;
		break;
	    case '#':
		state = EShash;
		break;
	    default:
		break;
	    }
	    break;
	case ESnonstd:
	    if      (c == 'P') {
		npar = 0;
		state = ESpalette;
	    } else if (c == 'R')
		state = ESnormal;
	    else
		state = ESnormal;
	    break;
	case ESpalette:
	    if ((c>='0'&&c<='9') || (c>='A'&&c<='F') || (c>='a'&&c<='f')) {
		npar++;
		if (npar==7)
		    state = ESnormal;
	    } else
		state = ESnormal;
	    break;
	case ESsquare:
	    npar = 0;
	    state = ESgetpars;
	    if (c == '[') {
		state=ESfunckey;
		break;
	    }
	    if (c=='?')
		break;
	case ESgetpars:
	    if (c==';' && npar<NPAR-1) {
		npar++;
		break;
	    } else if (c>='0' && c<='9') {
		break;
	    } else
		state = ESgotpars;
	case ESgotpars:
	    state = ESnormal;
	    break;
	case ESpercent:
	    state = ESnormal;
	    break;
	case ESfunckey:
	case EShash:
	case ESsetG0:
	case ESsetG1:
	    state = ESnormal;
	    break;
	}

	buf++;
	r--;
    }

    fflush(log);
    fdatasync(fileno(log));
}

/*
 * Our ring buffer
 */
static char ring[LOG_BUFFER_SIZE];
static char * end = ring + sizeof(ring);
static char *  in = ring;
static char * out = ring;

/*
 * Signal control for writing on log file
 */
static void (*save_sigio) = SIG_DFL;
static volatile sig_atomic_t nsigio = -1;

static void sigio(int sig)
{
    (void)signal(sig, save_sigio);
    nsigio = sig;
}

/*
 * The stdio file pointer for our log file
 */
static FILE * flog = NULL;
static int fdwrite = -1;
static int fdread  = -1;
static int fdfifo  = -1;

/*
 * Prepare I/O
 */
static void (*rw_connect)(void) = NULL;
static const char *fifo_name = _PATH_BLOG_FIFO;

void prepareIO(void (*rfunc)(int), void (*cfunc)(void), const int in, const int out)
{
    vc_reconnect = rfunc;
    rw_connect   = cfunc;
    fdread  = in;
    fdwrite = out;

    if (fifo_name && fdfifo < 0) {
	struct stat st;
	if (!stat(fifo_name, &st) && S_ISFIFO(st.st_mode)) {
	    if ((fdfifo = open(fifo_name, O_RDWR)) < 0)
		warn("can not open named fifo %s: %s\n", fifo_name, strerror(errno));
	}
    }
}

/*
 * Seek for input, more input ...
 */
static void more_input (struct timeval *timeout)
{
    fd_set watch;
    int nfds, wfds;

    FD_ZERO (&watch);
    FD_SET (fdread, &watch);

    if (fdfifo > 0) {
	FD_SET (fdfifo, &watch);
	wfds = (fdread > fdfifo ? fdread : fdfifo) + 1;
    } else
	wfds = fdread + 1;

    nfds = select(wfds, &watch, (fd_set*)0, (fd_set*)0, timeout);

    if (nfds < 0) {
	if (errno != EINTR)
	    error ("select(): %s\n", strerror(errno));
	goto out;
    }

    if (!nfds)
	goto out;

    if (FD_ISSET(fdread, &watch)) {
	ssize_t cnt = safein(fdread, in, end - in);
	char * tmp = in;

	safeout(fdwrite, in, cnt);	/* Write copy of input to real tty */
	tcdrain(fdwrite);
	in += cnt;

	if (tmp < out && in > out)
	    out = in;
	if (in  >= end)
	    in  = ring;
	if (out >= end)
	    out = ring;
    }

    if (fdfifo > 0 && FD_ISSET(fdfifo, &watch)) {
	ssize_t cnt = safein(fdfifo, in, PIPE_BUF);
	char * tmp = in;

	in += cnt;			/* NO copy of input from fifo to tty */

	if (tmp < out && in > out)
	    out = in;
	if (in  >= end)
	    in  = ring;
	if (out >= end)
	    out = ring;
    }
    errno = 0;
out:
}

/*
 *  The main routine for blogd.
 */
void safeIO (void)
{
    struct timeval timeout;
    ssize_t todo;
    static int log = -1;

    timeout.tv_sec  = 5;
    timeout.tv_usec = 0;
    more_input(&timeout);

    if (!nsigio) /* signal handler set but no signal recieved */
	goto out;

    if (log < 0) {
	if (access(BOOT_LOGFILE, W_OK) < 0) {
	    if (errno != ENOENT && errno != EROFS)
		error("Can not write to %s: %s\n", BOOT_LOGFILE, strerror(errno));
	    goto out;
	}
	if ((log = open(BOOT_LOGFILE, O_WRONLY|O_NOCTTY|O_APPEND)) < 0) {
	    if (errno != ENOENT && errno != EROFS)
		error("Can not open %s: %s\n", BOOT_LOGFILE, strerror(errno));
	    goto out;
	}
	if ((flog = fdopen (log, "a")) == NULL)
		error("Can not open %s: %s\n", BOOT_LOGFILE, strerror(errno));

	if (rw_connect)
	    (*rw_connect)();

	nsigio = SIGIO; /* We do not need a signal handler */
    }

    if (in >= out)
	todo = in  - out;
    else
	todo = end - out;

    if (todo) {
	parselog(flog, out, todo);
	out += todo;
	if (out >= end)
	    out = ring;
    }
out:
    if (nsigio < 0) { /* signal handler not set, so do it */
	save_sigio = signal(SIGIO, sigio);
	nsigio = 0;
    }
}

/*
 *
 */
void closeIO(void)
{
    struct timeval timeout;
    ssize_t todo;

    /* Maybe we've catched a signal, therefore */
    fflush(flog);			/* Clear out stdio buffers   */
    fdatasync(fileno(flog));		/* and throw it out	     */
    (void)tcdrain(fdwrite);		/* Hold in sync with console */

    timeout.tv_sec  = 0;
    timeout.tv_usec = 5*100*1000;	/* A half second */
    more_input(&timeout);

    if (!flog)
	goto out;

    if (in >= out)
	todo = in  - out;
    else
	todo = end - out;

    if (todo) {
	parselog(flog, out, todo);
	out += todo;
	if (out >= end)
	    out = ring;
    }

    if (!nl)
	fputc('\n', flog);

    (void)fclose(flog);
    (void)tcdrain(fdwrite);
out:
}

/*
 * Fetch our real underlying terminal character device,
 * returned name should be freed if not used anymore.
 */
static void ctty(pid_t pid, int * tty, int * ttypgrp)
{
    char fetched[NAME_MAX+1];
    int fd;

    sprintf(fetched, "/proc/%d/stat", (int)pid);
    if ((fd = open(fetched, O_RDONLY)) < 0)
	error("can not open(%s): %s\n", fetched, strerror(errno));
    safein(fd, fetched, sizeof(fetched));
    close(fd);

    /* format pid comm state ppid pgrp session tty tpgid */
    if (sscanf(fetched,"%*d %*s %*c %*d %*d %*d %d %d %*u", tty, ttypgrp) != 2)
	error("can not sscanf for my tty: %s\n", strerror(errno));
}

/* fall back routine to fetch tty */
static int fallback(const pid_t pid, const pid_t ppid)
{
    int tty = 0;
    pid_t ttypgrp = -1;
    pid_t  pgrp = getpgid(pid);
    pid_t ppgrp = getpgid(ppid);

    ctty(pid, &tty, &ttypgrp);

    if (pgrp != ttypgrp && ppgrp != ttypgrp) {
	int fdfrom[2];
	pid_t pid = -1;  /* Inner pid */

	if (pipe(fdfrom) < 0)
	    error("can not create a pipe: %s\n", strerror(errno));

	switch ((pid = fork())) {
	case 0:
	    {   void (*save_sighup);

		dup2( fdfrom[1], 1);
		close(fdfrom[1]);
		close(fdfrom[0]);

		pid = getpid();	/* our pid is not zero */

		if (pid != getsid(pid)) {
		    if (pid == getpgid(pid))
			setpgid(0, ppgrp);
		    setsid();
		}

		/* Remove us from any controlling tty */
		save_sighup = signal(SIGHUP, SIG_IGN);
		if (ttypgrp > 0)
		    ioctl(0, TIOCNOTTY, (void *)1);
		(void)signal(SIGHUP, save_sighup);

		/* Take stdin as our controlling tt< */
		if (ioctl(0, TIOCSCTTY, (void *)1) < 0)
		    warn("can not set controlling tty: %s\n", strerror(errno));

		ctty(pid, &tty, &ttypgrp);

		/* Never hold this controlling tty */
		save_sighup = signal(SIGHUP, SIG_IGN);
		if (ttypgrp > 0)
		    ioctl(0, TIOCNOTTY, (void *)1);
		(void)signal(SIGHUP, save_sighup);

		printf("|%d|%d|", tty, ttypgrp);  /* stdout to pipe synchronize ... */

		exit(0);
	    } break;
	case -1:
	    error("can not execute: %s\n", strerror(errno));
	    break;
	default:
	    {   int fd = dup(0);
		dup2( fdfrom[0], 0);
		close(fdfrom[0]);
		close(fdfrom[1]);

		scanf("|%d|%d|", &tty, &ttypgrp); /* ... with stdin from pipe here  */

		dup2(fd, 0);
		close(fd);
	    } break;
	}
    }

    return tty;
}

/* main routine to fetch tty */
char * fetchtty(const pid_t pid, const pid_t ppid)
{
    int tty = 0, found = 0;
    char * name;
    DIR * dev;
    struct dirent * d;
    struct stat st;

#ifdef TIOCGDEV
    if (ioctl (0, TIOCGDEV, &tty) < 0) {
#endif

	tty = fallback(pid, ppid);

#ifdef TIOCGDEV
    }
#endif

    if (!(dev = opendir("/dev")))
	error("can not opendir(/dev): %s\n", strerror(errno));

    pushd("/dev");
    while ((d = readdir(dev))) {
	name = d->d_name;

	if (*name == '.')
	    continue;

	if (rlstat(&name, &st) < 0) {
	    if (errno != ENOENT)
		warn("can not follow %s: %s\n", name, strerror(errno));
	    continue;
	}

	if (!S_ISCHR(st.st_mode))
	    continue;

	if ((dev_t)tty != st.st_rdev)
	    continue;
	found++;
	break;
    }
    popd();
    closedir(dev);

    if (!found)
	*name = '\0';

    /*
     * rlstat() uses lnk[] as buffer to which points
     * the pointer name after rlstat().
     */
    if (*name && *name != '/') {
	char * ptr = strdupa(name);
	strcpy(name, "/dev/");
	strcat(name, ptr);
    }
    return name;
}
