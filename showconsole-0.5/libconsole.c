#ifndef  LOG_BUFFER_SIZE
# define LOG_BUFFER_SIZE	65536
#endif
#ifndef  BOOT_LOGFILE
# define BOOT_LOGFILE		"/var/log/boot.msg"
#endif
#include <sys/time.h>
#include <sys/types.h> /* Defines the macros major and minor */
#include <sys/stat.h>
#include <sys/ioctl.h>
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
    while (s > 0) {
	ssize_t p = write (fd, ptr, s);
	if (p < 0) {
	    if (errno == EPIPE)
		exit (0);
	    if (errno == EINTR || errno == EAGAIN)
		continue;
	    if (errno == EIO && vc_reconnect) {
		(*vc_reconnect)(fd);
		vc_reconnect = NULL;
		continue;
	    }
	    error("Can not write to fd %d: %s\n", fd, strerror(errno));
	}
	ptr += p;
	s -= p;
    }
}

/*
 * Once used: safe in
 */
static inline ssize_t safein  (int fd, char *ptr, size_t s)
{
    ssize_t r = 0;
    do {
	r = read (fd, ptr, s);
    } while (r < 0 && (errno == EINTR || errno == EAGAIN));

    if (r < 0)
	error("Can not from fd %d: %s\n", fd, strerror(errno));

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

#ifdef BLOGGER
/*
 * If 1 use sequences handled but not used by console.c
 */
static int blogger = 0;
#endif
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
#ifdef BLOGGER
	    if (c>='0'&&c<='9') {
		blogger = 1;
		if (!nl)
		    fputc('\n', log);
		switch (c) {
		case 1:
		default:
		    fprintf(log, "<notice>");
		    break;
		case 2:
		    fprintf(log, "<done>");
		    break;
		case 3:
		    fprintf(log, "<failed>");
		    break;
		case 4:
		    fprintf(log, "<skipped>");
		    break;
		case 5:
		    fprintf(log, "<unused>");
		    break;
		}
	    } else {
		if (blogger)
		    fputc('\n', log);
		blogger = 0;
	    }
#endif
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
#ifdef BLOGGER
	    if (blogger)
		fputc(c, log);
#endif
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
static int nsigio = -1;
static void sigio(int sig)
{
    (void)signal(sig, save_sigio);
    nsigio = sig;
}

/*
 * The stdio file pointer for our log file
 */
static FILE * flog = NULL;

#ifdef NEWBLOGGER
static sigset_t ttin;
static void sigttin(int sig)
{
    sigprocmask (SIG_BLOCK, &ttin, NULL);
    parselog(flog, "Signal TTIN\n", sizeof(char)*strlen("Signal TTIN\n"));
    sigprocmask (SIG_UNBLOCK, &ttin, NULL);
}
#endif

/*
 * Prepare I/O
 */
static void (*rw_connect)(void) = NULL;
void prepareIO(void (*rfunc)(int), void (*cfunc)(void))
{
#ifdef NEWBLOGGER
    struct sigaction act;
#endif

    vc_reconnect = rfunc;
    rw_connect   = cfunc;

#ifdef NEWBLOGGER
    act.sa_handler = sigttin;
    act.sa_flags   = SA_RESTART;
    if (sigemptyset (&act.sa_mask))
	error("can not set empty signal set: %s\n", strerror(errno));
    if (sigaddset   (&act.sa_mask, SIGTTIN))
	error("can not set add TTIN to signal set: %s\n", strerror(errno));
    if (sigaction (SIGTTIN, &act, NULL))
	error("can not set signal action: %s\n", strerror(errno));
    ttin = act.sa_mask;
#endif
}

/*
 *  The main routine for blogd.
 */
void safeIO (const int fdread, const int fdwrite)
{
    fd_set watch;
    struct timeval timeout;
    ssize_t todo;
    static int log = -1;

    FD_ZERO (&watch);
    FD_SET (fdread, &watch);

    timeout.tv_sec  = 5;
    timeout.tv_usec = 0;

    if (select(fdread + 1, &watch, (fd_set*)0, (fd_set*)0, &timeout) == 1) {
	ssize_t cnt;
	if ((cnt = read(fdread, in, end - in)) >= 0) {
	    char * tmp = in;

	    safeout(fdwrite, in, cnt);	/* Write copy of input to real tty */
	    in += cnt;

	    if (tmp < out && in > out)
		out = in;
	    if (in  >= end)
		in  = ring;
	    if (out >= end)
		out = ring;
	} else {
	    if (errno != EINTR && errno != EAGAIN)
		error("Can not write to fd %d: %s\n", fdread, strerror(errno));
	}
    }

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

    parselog(flog, out, todo);
    out += todo;

    if (out >= end)
	out = ring;
out:
    if (nsigio < 0) { /* signal handler not set, so do it */
	save_sigio = signal(SIGIO, sigio);
	nsigio = 0;
    }
}

/*
 *
 */
void closeIO()
{
    ssize_t todo;

    if (!flog)
	goto out;

    if (in >= out)
	todo = in  - out;
    else
	todo = end - out;

    parselog(flog, out, todo);
    out += todo;

    if (!nl)
	fputc('\n', flog);

    (void)fclose(flog);
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

/* Used to hold signal call in place */
#define mbr() __asm__ __volatile__("": : :"memory")

/* main routine to fetch tty */
char * fetchtty(const pid_t pid, const pid_t ppid)
{
    int tty = 0, found = 0;
    pid_t ttypgrp = -1;
    pid_t  pgrp = getpgid(pid);
    pid_t ppgrp = getpgid(ppid);
    char * name;
    DIR * dev;
    struct dirent * d;
    struct stat st;

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
