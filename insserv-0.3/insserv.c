#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <regex.h>
#include <errno.h>
#include <limits.h>
#include "listing.h"

#ifndef  INITDIR
# define INITDIR	"/sbin/init.d"
#endif


/*
 * For a description of regular expressions see regex(7).
 */
#define COMM		"^#[[:blank:]]*"
#define VALUE		":[[:blank:]]*([[:print:][:blank:]]*)"
/* The second substring contains our value (the first is all) */
#define SUBNUM		2
#define START		"[-_]?start"
#define STOP		"[-_]?stop"

/* The main regular search expressions */
#define PROVIDES	COMM "provides" VALUE
#define REQUIRED	COMM "required"
#define DEFAULT		COMM "default"
#define REQUIRED_START  REQUIRED START VALUE
#define REQUIRED_STOP	REQUIRED STOP  VALUE
#define DEFAULT_START	DEFAULT  START VALUE
#define DEFAULT_STOP	DEFAULT  STOP  VALUE
#define DESCRIPTION	COMM "description" VALUE

/* The main line buffer if unique */
static char buf[LINE_MAX];
static char pwd[NAME_MAX+1];

/* Search results points here */
static char *provides = NULL;
static char *required_start = NULL;
static char *required_stop = NULL;
static char *default_start = NULL;
static char *default_stop = NULL;
static char *description = NULL;

/* Delimeters used for spliting results with strsep(3) */
const char *delimeter = " ,;\t";

/* The programs name */
char *myname = NULL;

/*
 * Internal logger
 */
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
    if (pwd[0])
	chdir(pwd);
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
 * Wrapper for regcomp(3)
 */
static void regcompiler (regex_t *preg, const char *regex, int cflags)
{
    register int ret = regcomp(preg, regex, cflags);
    if (ret) {
	regerror(ret, preg, buf, sizeof (buf));
	regfree (preg);
	error("%s\n", buf);
    }
    return;
}

/*
 * Wrapper for regexec(3)
 */
static boolean regexecutor (regex_t *preg, const char *string,
			size_t nmatch, regmatch_t pmatch[], int eflags)
{
    register int ret = regexec(preg, string, nmatch, pmatch, eflags);
    if (ret > REG_NOMATCH) {
	regerror(ret, preg, buf, sizeof (buf));
	regfree (preg);
	error("%s\n", buf);
    }
    return (ret ? false : true);
}

/*
 * The script scanning engine.
 */
static void scan_script(const char *path)
{
    regex_t reg_prov;
    regex_t reg_req_start;
    regex_t reg_req_stop;
    regex_t reg_def_start;
    regex_t reg_def_stop;
    regex_t reg_desc;
    regmatch_t subloc[SUBNUM], *val = &subloc[SUBNUM - 1];
    FILE *script;
    char *pbuf = buf;

    regcompiler(&reg_prov,	PROVIDES,	REG_EXTENDED|REG_ICASE);
    regcompiler(&reg_req_start, REQUIRED_START, REG_EXTENDED|REG_ICASE|REG_NEWLINE);
    regcompiler(&reg_req_stop,  REQUIRED_STOP,	REG_EXTENDED|REG_ICASE|REG_NEWLINE);
    regcompiler(&reg_def_start, DEFAULT_START,	REG_EXTENDED|REG_ICASE|REG_NEWLINE);
    regcompiler(&reg_def_stop,  DEFAULT_STOP,	REG_EXTENDED|REG_ICASE|REG_NEWLINE);
    regcompiler(&reg_desc,	DESCRIPTION,	REG_EXTENDED|REG_ICASE|REG_NEWLINE);

    script = fopen(path, "r");
    if (!script)
	error("fopen(%s): %s\n", path, strerror(errno));

    /* Reset old results */
    provides = NULL;
    required_start = NULL;
    required_stop = NULL;
    default_start = NULL;
    default_stop = NULL;
    description = NULL;

#define COMMON_ARGS	buf, SUBNUM, subloc, 0
    while (fgets(buf, sizeof(buf), script)) {
	if (!provides       && regexecutor(&reg_prov,	   COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		provides = strdup(pbuf+val->rm_so);
	    }
	}
	if (!required_start && regexecutor(&reg_req_start, COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		required_start = strdup(pbuf+val->rm_so);
	    }
	}
	if (!required_stop  && regexecutor(&reg_req_stop,  COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		required_stop = strdup(pbuf+val->rm_so);
	    }
	}
	if (!default_start  && regexecutor(&reg_def_start, COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		default_start = strdup(pbuf+val->rm_so);
	    }
	}
	if (!default_stop   && regexecutor(&reg_def_stop,  COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		default_stop = strdup(pbuf+val->rm_so);
	    }
	}
	if (!description    && regexecutor(&reg_desc,	   COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		description = strdup(pbuf+val->rm_so);
	    }
	}
    }
#undef COMMON_ARGS
    regfree(&reg_prov);
    regfree(&reg_req_start);
    regfree(&reg_req_stop);
    regfree(&reg_def_start);
    regfree(&reg_def_stop);
    regfree(&reg_desc);
    fclose(script);

    return;
}

/*
 * Open a runlevel directory, if it not
 * exists than create one.
 */
static DIR * openrcdir(const char * rcpath)
{
   DIR * rcdir;
   struct stat st;

    if (stat(rcpath, &st) < 0) {
	if (errno == ENOENT)
	    mkdir(rcpath, (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH));
	else
	    error("can not stat(%s): %s\n", rcpath, strerror(errno));
    }

    if ((rcdir = opendir(rcpath)) == NULL)
	error("can not opendir(%s): %s\n", rcpath, strerror(errno));

    return rcdir;
}

/*
 * Scan for a Start or Kill script within a runlevel directory.
 * We start were we leave the directory, the upper level
 * has to call rewinddir(3) if necessary.
 */
static char * scan_for(DIR * rcdir, const char * script, char type)
{
    struct dirent *d;
    char * ret = NULL;

    while ((d = readdir(rcdir)) != NULL) {
	char * ptr = d->d_name;

	if (*ptr != type)
	    continue;
	ptr++;

	if (strspn(ptr, "0123456789") != 2)
	    continue;
	ptr += 2;

	if (!strcmp(ptr, script)) {
	    ret = d->d_name;
	    break;
	}
    }
    return ret;
}

/*
 * Do the job.
 */
int main (int argc, char *argv[])
{
    DIR * initdir;
    struct dirent *d;
    struct stat st_script;
    char * end;
    char * path = INITDIR;
    int runlevel, order;

    myname = basename(*argv);
    argv++;
    argc--;

    if (argc > 1)
	error("usage: %s [init_script|init_directory]\n", myname);

    pwd[0] = '\0';
    if (!getcwd(pwd, NAME_MAX))
	error("can not get current directory\n");

    if (*argv) {
	if (stat(*argv, &st_script) < 0)
	    error("%s: %s\n", *argv, strerror(errno));

	if (S_ISDIR(st_script.st_mode))
	    path = *argv;
	else {
	    char * base;
	    base = strrchr(*argv, '/');
	    *(++base) = '\0';
	    path = *argv;
	}
	argv++;
	argc--;
    }

    if ((initdir = opendir(path)) == NULL)
	error("can not opendir(%s): %s\n", path, strerror(errno));

    if (chdir(path) < 0)
	error("can not change directory: %s\n", strerror(errno));

    while ((d = readdir(initdir)) != NULL) {
	errno = 0;
	/* d_type seems not to work, therefore use stat(2) */
	if (stat(d->d_name, &st_script) < 0) {
	    warn("can not stat(%s)\n", d->d_name);
	    continue;
	}
	if (!S_ISREG(st_script.st_mode) || !(S_IXUSR & st_script.st_mode))
	    continue;

	if (!strncmp(d->d_name, "README", strlen("README")))
	    continue;

	/* Common scripts not used within runlevels */
	if (!strcmp(d->d_name, "rc")	   ||
	    !strcmp(d->d_name, "rx")	   ||
	    !strcmp(d->d_name, "skeleton") ||
	    !strcmp(d->d_name, "powerfail"))
	    continue;

	if (!strcmp(d->d_name, "boot"))
	    continue;

	if ((end = strrchr(d->d_name, '.'))) {
	    end++;
	    if (!strcmp(end, "local"))
		continue;
	    if (!strcmp(end, "rpmsave"))
		continue;
	    if (!strcmp(end, "rpmnew"))
		continue;
	    if (!strcmp(end, "rpmorig"))
		continue;
	    if (!strcmp(end, "bak"))
		continue;
	    if (!strcmp(end, "old"))
		continue;
	    if (!strcmp(end, "save"))
		continue;
	}

	/* Leaved by editors */
	if (*(d->d_name) == '#' || *(d->d_name) == '.')
	    continue;
	if (d->d_name[strlen(d->d_name)-1] == '~')
	    continue;

	/* main scanner */
	scan_script(d->d_name);

	/* Common script ... */
	if (!strcmp(d->d_name, "halt")) {
	    makeprov("halt",   d->d_ino);
	    runlevels("halt",   "0");
	    continue;
	}

	/* ... and its link */
	if (!strcmp(d->d_name, "reboot")) {
	    makeprov("reboot", d->d_ino);
	    runlevels("reboot", "6");
	    continue;
	}

	/* Common script for single mode */
	if (!strcmp(d->d_name, "single")) {
	    makeprov("single", d->d_ino);
#if 0
	    runlevels("single", "S");
#else
	    runlevels("single", "1 S");
#endif
	    continue;
	}

	if (!provides) {
	    /* Oops, no comment found, guess one */
	    provides = d->d_name;
	    if (!required_start)
		required_start = "route nfs syslog autofs";
	}

	if (provides) {
	    char * token;
	    while ((token = strsep(&provides, delimeter))) {
		if (makeprov(token, d->d_ino) < 0) {
		    warn("%s: script %s: service %s already provided!\n",
			 myname, d->d_name, token);
		    continue;
		}
		if (required_start)
		    requiresv(token, required_start);

		/* Ahh ... set default multiuser with network */
		if (!default_start)
		    default_start = "3 5";
		runlevels(token, default_start);

		/*
		 * required_stop and default_stop arn't used in SuSE Linux.
		 */
	    }
	}

	/*
	 * reset pointers for the next script
	 */
    }
    closedir(initdir);

    /*
     * Now generate for all scripts the dependencies
     */
    follow_all();

    /*
     * Re-order some well known scripts to get
     * a more stable order collection.
     * Stable means that new scripts should not
     * force a full re-order of all starting numbers.
     */

    if (getorder("network") <  5) setorder("network",  5);
    setorder("route", (getorder("network") + 2));
    if (getorder("inetd")   < 20) setorder("inetd",   20);

    /*
     * Set order of some singluar scripts
     * (no dependencies or single link).
     */
    if (getorder("halt")   < 20) minorder("halt",   20);
    if (getorder("reboot") < 20) minorder("reboot", 20);
    if (getorder("single") < 20) minorder("single", 20);
    if (getorder("gpm")    < 20) minorder("gpm",    20);

    /*
     * Do not overwrite good old links.
     */
    if (getorder("serial")     < 10) setorder("serial",     10);
    if (getorder("boot.setup") < 20) setorder("boot.setup", 20);

    /*
     * Sorry but we support only [KS][0-9][0-9]<name>
     */
    if (maxorder > 99)
	error("Maximum of 99 in ordering reached\n");

#if defined(DEBUG) && (DEBUG > 0)
    printf("Maxorder %d\n", maxorder);
    show_all();
#else

    for (runlevel = 0; runlevel < 9; runlevel++) {
	char * script;
	char nlink[PATH_MAX+1], olink[PATH_MAX+1];
	char * rcd = NULL;
	DIR  * rcdir;

	switch (runlevel) {
	    case 0: rcd = "rc0.d/";  break;
	    case 1: rcd = "rc1.d/";  break;
	    case 2: rcd = "rc2.d/";  break;
	    case 3: rcd = "rc3.d/";  break;
	    case 4: rcd = "rc4.d/";  break;
	    case 5: rcd = "rc5.d/";  break;
	    case 6: rcd = "rc6.d/";  break;
	    case 7: rcd = "rcS.d/";  break;  /* runlevel S */
	    case 8: rcd = "boot.d/"; break;  /* runlevel B */
	    default:
		error("Wrong runlevel %d\n", runlevel);
	}

	script = NULL;
	rcdir = openrcdir(rcd);
	if (chdir(rcd) < 0) {
	    warn("can not change directory: %s\n", strerror(errno));
	    closedir(rcdir);
	    continue;
	}

	/*
	 * See if we found scripts which should not be
	 * included within this runlevel directory.
	 */
	while ((d = readdir(rcdir)) != NULL) {
	    char * ptr = d->d_name;

	    if (*ptr != 'S' && *ptr != 'K')
		continue;
	    ptr++;

	    if (strspn(ptr, "0123456789") != 2)
		continue;
	    ptr += 2;

	    if (!strcmp(ptr, "kbd"))
		continue;  /* kbd should run on any runlevel change */

	    if (notincluded(ptr, runlevel))
		if (remove(d->d_name) < 0)
		    warn ("can not remove(%s%s): %s\n", rcd, d->d_name, strerror(errno));
	}

	/*
	 * Seek for scripts which are included, link or
	 * correct order number if necessary.
	 */
	while (foreach(&script, &order, runlevel)) {
	    char * clink;
	    boolean found;

	    sprintf(olink, "../%s",   script);
	    sprintf(nlink, "S%.2d%s", order, script);

	    found = false;
	    rewinddir(rcdir);
	    while ((clink = scan_for(rcdir, script, 'S'))) {
		if (strcmp(clink, nlink)) {
		    if (remove(clink) < 0)
			warn ("can not remove(%s%s): %s\n", rcd, clink, strerror(errno));
		} else
		    found = true;
	    }

	    if (!found)
		if (symlink(olink, nlink) < 0)
		    warn ("can not symlink(%s, %s): %s\n", olink, nlink, strerror(errno));

	    /* Start link done, now Kill link */

	    if (!strcmp(script, "kbd"))
		continue;  /* kbd should run on any runlevel change */

	    sprintf(nlink, "K%.2d%s", (maxorder + 1) - order, script);

	    found = false;
	    rewinddir(rcdir);
	    while ((clink = scan_for(rcdir, script, 'K'))) {
		if (strcmp(clink, nlink)) {
		    if (remove(clink) < 0)
			warn ("can not remove(%s%s): %s\n", rcd, clink, strerror(errno));
		} else
		   found = true;
	    }

	    /*
	     * One way runlevels:
	     * Remove kill links from rc0.d/, rc6.d/, and boot.d/.
	     */
	    if (runlevel < 1 || runlevel == 6 || runlevel > 7) {
		if (found)
		    if (remove(nlink) < 0)
			warn ("can not remove(%s%s): %s\n", rcd, nlink, strerror(errno));
	    } else
		if (!found)
		    if (symlink(olink, nlink) < 0)
			warn ("can not symlink(%s, %s): %s\n", olink, nlink, strerror(errno));
	}
	chdir("..");
	closedir(rcdir);
    }
#endif

    /*
     * Back to the root(s)
     */
    chdir(pwd);

    return 0;
}
