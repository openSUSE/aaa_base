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
#include <time.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <paths.h>
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
    popd();
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
 * Remove pidfile
 */
static void rmfpid()
{
    char buf[strlen(_PATH_VARRUN)+strlen(myname)+1+3+1];
    strcat(strcat(strcpy(buf, _PATH_VARRUN), myname), ".pid");
    unlink(buf);
}

/*
 * Write pidfile
 */
static void pidfile()
{
    char buf[strlen(_PATH_VARRUN)+strlen(myname)+1+3+1];
    FILE * fpid;

    strcat(strcat(strcpy(buf, _PATH_VARRUN), myname), ".pid");
    if ((fpid = fopen (buf, "w")) == NULL) {
	warn("can not open %s: %s\n", buf, strerror(errno));
	goto out;
    }
    fprintf(fpid, "%d\n", (int)getpid());
    fclose(fpid);
    atexit(rmfpid);
out:
}

/*
 *  Signal handler
 */
static void (*saved_sigttin) = SIG_DFL;
static void (*saved_sigttou) = SIG_DFL;
static void (*saved_sigtstp) = SIG_DFL;
static void (*saved_sighup)  = SIG_DFL;
static void (*saved_sigint)  = SIG_DFL;
static void (*saved_sigquit) = SIG_DFL;
static void (*saved_sigterm) = SIG_DFL;
static volatile sig_atomic_t signaled = 0;
static void sighandle(int sig)
{
    signaled = sig;
}

/*
 * To be able to reconnect to real tty on EIO
 */
static char * tty;
static void reconnect(int fd)
{
    int newfd = -1;

    if ((newfd = open(tty, O_WRONLY|O_NONBLOCK)) < 0)
	error("can not open %s: %s\n", tty, strerror(errno));
 
    if (newfd != 1)
	dup2(newfd,  1);
    if (newfd != 2)
	dup2(newfd,  2);

    if (fd == 1 || fd == 2)
	goto out;
    if (newfd != fd)
	dup2(newfd, fd);
out:
    if (newfd > 2)
	close(newfd);
}

/*
 * Now do the job
 */
int main(int argc, char *argv[])
{
    int fd, flags;
    int ptm, pts;
    pid_t pid;  
    char ptsname[NAME_MAX+1];
    struct termios t;
    struct winsize w;
    time_t tt;
    char *stt;

    myname = basename(*argv);

    saved_sigttin = signal(SIGTTIN, SIG_IGN);
    saved_sigttou = signal(SIGTTOU, SIG_IGN);
    saved_sigtstp = signal(SIGTSTP, SIG_IGN);
    saved_sighup  = signal(SIGHUP,  SIG_IGN);
    saved_sigint  = signal(SIGINT,  sighandle);
    saved_sigquit = signal(SIGQUIT, sighandle);
    saved_sigterm = signal(SIGTERM, sighandle);

    if (argc > 2)
	error("usage: %s [/dev/tty<X>]\n", myname);

    if (argc == 2)
	tty = argv[1];
    else
	tty = fetchtty(getpid(), getppid());

    if (!tty || !*tty)
	error("can not fetch real tty\n");

    if ((fd = open(tty, O_WRONLY|O_NONBLOCK)) < 0)
	error("can not open %s: %s\n", tty, strerror(errno));

    if ((flags = fcntl(fd, F_GETFL)) < 0)
	error("can not get terminal flags: %s\n", strerror(errno));
    flags &= ~(O_NONBLOCK);
    if (fcntl(fd, F_GETFL, flags) < 0)
	error("can not set terminal flags: %s\n", strerror(errno));

    if (tcgetattr(fd, &t) < 0)
	error("can not get terminal parameters: %s\n", strerror(errno));

    w.ws_row = 0;
    w.ws_col = 0;
    if (ioctl(fd, TIOCGWINSZ, &w) < 0)
	error("can not get terminal parameters: %s\n", strerror(errno));

    if (!w.ws_row)
	w.ws_row = 24;
    if (!w.ws_col)
	w.ws_row = 80;

    if (openpty(&ptm, &pts, ptsname, &t, &w) < 0)
	error("can not open pty/tty pair: %s\n", strerror(errno));

    (void)ioctl(0, TIOCCONS, NULL);  /* Undo any current map if any */
    if (ioctl(pts, TIOCCONS, NULL) < 0)
	error("can not set console device: %s\n", strerror(errno));


    switch ((pid = fork())) {
    case 0:
	/* Get our own session */
	setsid();
	/* Reconnect our own terminal I/O */
	dup2(ptm, 0);
	dup2(fd,  1);
	dup2(fd,  2);
	close(ptm);
	close(fd);
	break;
    case -1:
	close(pts);
	close(ptm);
	close(fd);
	error("can not execute: %s\n", strerror(errno));
	break;
    default:
	time(&tt);
	stt = ctime(&tt);
	close(pts);
	close(ptm);
	close(fd);
	printf("\rBoot logging started at %.24s\n", stt);
	exit(0);
    }

    prepareIO(reconnect, pidfile);
    do {
	safeIO(0, 1);
    } while (!signaled);
    closeIO();

    close(pts);
    rmfpid();

    signal(SIGTTIN, saved_sigttin);
    signal(SIGTTOU, saved_sigttou);
    signal(SIGTSTP, saved_sigtstp);
    signal(SIGHUP,  saved_sighup);
    signal(SIGINT,  saved_sigint);
    signal(SIGQUIT, saved_sigquit);
    signal(SIGTERM, saved_sigterm);

    return 0;
}
