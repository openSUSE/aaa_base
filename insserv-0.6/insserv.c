/*
 * insserv(.c)
 *
 * Copyright 2000 Werner Fink, 2000 SuSE GmbH Nuernberg, Germany.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

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
# define INITDIR	"/etc/init.d"
#endif
#ifndef  INSCONF
# define INSCONF	"/etc/insserv.conf"
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

/* System facility search within /etc/insserv.conf */
#define EQSIGN		"([[:blank:]]?[=:]?[[:blank:]]?|[[:blank:]]+)"
#define CONFLINE	"^(\\$[a-z0-9_-]+)" EQSIGN "([[:print:][:blank:]]*)"
#define SUBCONF		2
#define SUBCONFNUM	4

/* The main line buffer if unique */
static char buf[LINE_MAX];

/* Search results points here */
static char *provides = NULL;
static char *required_start = NULL;
static char *required_stop = NULL;
static char *default_start = NULL;
static char *default_stop = NULL;
static char *description = NULL;
static char empty[1] = "";

/* Delimeters used for spliting results with strsep(3) */
const char *delimeter = " ,;\t";

/* declare */
void error (const char *fmt, ...);

/*
 * push and pod directory changes
 */
typedef struct pwd_struct {
    list_t	deep;
    char	*pwd;
} pwd_t;
#define getpwd(list)	list_entry((list), struct pwd_struct, deep)

static list_t pwd = { &(pwd), &(pwd) }, * topd = &(pwd);

static void pushd(const char * path)
{
    pwd_t *  dir;

    dir = (pwd_t *)malloc(sizeof(pwd_t));
    if (dir) {
	if (!(dir->pwd = getcwd(NULL, 0)))
	    goto err;
	insert(&(dir->deep), topd->prev);
	goto out;
    }
err:
    error("%s", strerror(errno));
out:
    if (chdir(path) < 0)
	    error ("pushd() can not change to directory %s: %s\n", path, strerror(errno));
}

static void popd(void)
{
    list_t * tail = topd->prev;
    pwd_t *  dir;

    if (tail == topd)
	goto out;
    dir = getpwd(tail);
    if (chdir(dir->pwd) < 0)
	error ("popd() can not change directory %s: %s\n", dir->pwd, strerror(errno));
    free(dir->pwd);
    delete(tail);
    free(dir);
out:
}

/*
 * Internal logger
 */
char *myname = NULL;
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
		if (!provides)
		    error("%s", strerror(errno));
	    } else
		provides = empty;
	}
	if (!required_start && regexecutor(&reg_req_start, COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		required_start = strdup(pbuf+val->rm_so);
		if (!required_start)
		    error("%s", strerror(errno));
	    } else
		required_start = empty;
	}
	if (!required_stop  && regexecutor(&reg_req_stop,  COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		required_stop = strdup(pbuf+val->rm_so);
		if (!required_stop)
		    error("%s", strerror(errno));
	    } else
		required_stop = empty;
	}
	if (!default_start  && regexecutor(&reg_def_start, COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		default_start = strdup(pbuf+val->rm_so);
		if (!default_start)
		    error("%s", strerror(errno));
	    } else
		default_start = empty;
	}
	if (!default_stop   && regexecutor(&reg_def_stop,  COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		default_stop = strdup(pbuf+val->rm_so);
		if (!default_stop)
		    error("%s", strerror(errno));
	    } else
		default_stop = empty;
	}
	if (!description    && regexecutor(&reg_desc,	   COMMON_ARGS) == true) {
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		description = strdup(pbuf+val->rm_so);
		if (!description)
		    error("%s", strerror(errno));
	    } else
		description = empty;
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
 * The script scanning engine.
 */
static void scan_conf(void)
{
    regex_t reg_conf;
    regmatch_t subloc[SUBCONFNUM], *val = NULL;
    FILE *conf;
    char *pbuf = buf;

    regcompiler(&reg_conf, CONFLINE, REG_EXTENDED|REG_ICASE);

    do {
	char * fptr = INSCONF;
	if (*fptr == '/')
	    fptr++;
	/* Try relativ location first */
	if ((conf = fopen(fptr, "r")))
	    break;
	/* Try absolute location */
	if ((conf = fopen(INSCONF, "r")))
	    break;
	goto err;
    } while (1);

    while (fgets(buf, sizeof(buf), conf)) {
	if (*pbuf == '#')
	    continue;
	if (regexecutor(&reg_conf, buf, SUBCONFNUM, subloc, 0) == true) {
	    char * virt = NULL, * real = NULL;
	    val = &subloc[SUBCONF - 1];
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		virt = pbuf+val->rm_so;
	    }
	    val = &subloc[SUBCONFNUM - 1];
	    if (val->rm_so < val->rm_eo) {
		*(pbuf+val->rm_eo) = '\0';
		real = pbuf+val->rm_so;
	    }
	    if (virt)
		virtprov(virt, real);
	}
    }
    regfree(&reg_conf);
    fclose(conf);
    return;
err:
    warn("fopen(%s): %s\n", INSCONF, strerror(errno));
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
 *  Check for script in list.
 */
static boolean chkfor(const char * script, char **list, const int cnt)
{
    boolean isinc = false;
    register int c = cnt;
    while (c--) {
	if (!strcmp(script, list[c])) {
	    isinc = true;
	    break;
	}
    }
    return isinc;
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
    int runlevel, order, c;
    boolean del = false;

    myname = basename(*argv);

    while ((c = getopt(argc, argv, "r")) != -1) {
	switch (c) {
	    case 'r':
		del = true;
		break;
	    case '?':
		error("usage: %s [[-r] init_script|init_directory]\n", myname);
	    case 'h':
		warn ("usage: %s [[-r] init_script|init_directory]\n", myname);
		exit(0);
	    default:
		break;
	}
    }
    argv += optind;
    argc -= optind;

    if (!argc && del)
	error("usage: %s [[-r] init_script|init_directory]\n", myname);

    if (*argv) {
	if (stat(*argv, &st_script) < 0) {
	    if (errno != ENOENT)
		error("%s: %s\n", *argv, strerror(errno));
	    pushd(path);
	    if (stat(*argv, &st_script) < 0)
		error("%s: %s\n", *argv, strerror(errno));
	    popd();
	}

	if (S_ISDIR(st_script.st_mode)) {
	    path = *argv;
	    if (del)
		error("usage: %s [[-r] init_script|init_directory]\n", myname);
	    argv++;
	    argc--;
	    if (argc)
		error("usage: %s [[-r] init_script|init_directory]\n", myname);
	} else {
	    char * base, * ptr = strdup(*argv);
	    if (!ptr)
		error("%s", strerror(errno));
	    if ((base = strrchr(ptr, '/'))) {
		*(++base) = '\0';
		path = ptr;
	    } else
		free(ptr);
	}
    }

    c = argc;
    while (c--) {
	char * base;
	if (stat(argv[c], &st_script) < 0) {
	    if (errno != ENOENT)
		error("%s: %s\n", argv[c], strerror(errno));
	    pushd(path);
	    if (stat(argv[c], &st_script) < 0)
		error("%s: %s\n", *argv, strerror(errno));
	    popd();
	}
	if ((base = strrchr(argv[c], '/'))) {
	    base++;
	    argv[c] = base;
	}
    }

    if ((initdir = opendir(path)) == NULL)
	error("can not opendir(%s): %s\n", path, strerror(errno));

    pushd(path);
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

	if (!strncmp(d->d_name, "core", strlen("core")))
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
	    if (!strcmp(end,  "local"))
		continue;
	    /* .rmporig, .rpmnew, .rmpsave, ... */
	    if (!strncmp(end, "rpm", 3))
		continue;
	    /* .bak, .backup, ... */
	    if (!strncmp(end, "ba", 2))
		continue;
	    if (!strcmp(end,  "old"))
		continue;
	    if (!strcmp(end,  "new"))
		continue;
	    if (!strcmp(end,  "save"))
		continue;
	    /* Used by vi like editors */
	    if (!strcmp(end,  "swp"))
		continue;
	    /* modern core dump */
	    if (!strcmp(end,  "core"))
		continue;
	}

	/* Leaved by emacs like editors */
	if (d->d_name[strlen(d->d_name)-1] == '~')
	    continue;

	if (strspn(d->d_name, "0123456789$.#_-\\*"))
	    continue;

	/* main scanner */
	scan_script(d->d_name);

	/* Common script ... */
	if (!strcmp(d->d_name, "halt")) {
	    makeprov("halt",   d->d_name);
	    runlevels("halt",   "0");
	    continue;
	}

	/* ... and its link */
	if (!strcmp(d->d_name, "reboot")) {
	    makeprov("reboot", d->d_name);
	    runlevels("reboot", "6");
	    continue;
	}

	/* Common script for single mode */
	if (!strcmp(d->d_name, "single")) {
	    makeprov("single", d->d_name);
#if 0
	    runlevels("single", "S");
#else
	    runlevels("single", "1 S");
	    requiresv("single", "kbd");
#endif
	    continue;
	}

	if (!provides || provides == empty) {
	    /* Oops, no comment found, guess one */
	    provides = d->d_name;
	}

	if (provides) {
	    char * token;
	    while ((token = strsep(&provides, delimeter))) {
		if (*token == '$') {
		    warn("script %s provides system facility %s, skiped!\n", d->d_name, token);
		    continue;
		}
		if (makeprov(token, d->d_name) < 0) {
		    warn("script %s: service %s already provided!\n", d->d_name, token);
		    continue;
		}
		if (required_start && required_start != empty)
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
    popd();
    closedir(initdir);

    /*
     * Scan and set our configuration for virtual services.
     */
    scan_conf();

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

    if ((order = getorder("network")) > 0) {
	if (order < 5) setorder("network", 5);
	if (getorder("route") > 0)
	    setorder("route", (getorder("network") + 2));
    }
    if ((order = getorder("inetd"))  > 0 && order < 20) setorder("inetd",  20);

    /*
     * Set order of some singluar scripts
     * (no dependencies or single link).
     */
    if ((order = getorder("halt"))   > 0 && order < 20) setorder("halt",   20);
    if ((order = getorder("reboot")) > 0 && order < 20) setorder("reboot", 20);
    if ((order = getorder("single")) > 0) {
	if (order < 20) setorder("single", 20);
	if (getorder("kbd") > 0)
	    setorder("single", (getorder("kbd") + 2));
    }

    /*
     * Do not overwrite good old links.
     */
    if ((order = getorder("serial"))     > 0 && order < 10) setorder("serial",     10);
    if ((order = getorder("boot.setup")) > 0 && order < 20) setorder("boot.setup", 20);
    if ((order = getorder("gpm"))        > 0 && order < 20) setorder("gpm",        20);

    /*
     * Sorry but we support only [KS][0-9][0-9]<name>
     */
    if (maxorder > 99)
	error("Maximum of 99 in ordering reached\n");

#if defined(DEBUG) && (DEBUG > 0)
    printf("Maxorder %d\n", maxorder);
    show_all();
#else

    pushd(path);
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
	rcdir = openrcdir(rcd); /* Creates runlevel directory if necessary */
	pushd(rcd);

#define xremove(x) if (remove(x) < 0) \
	warn ("can not remove(%s%s): %s\n", rcd, x, strerror(errno))

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

	    if (stat(d->d_name, &st_script) < 0)
		xremove(d->d_name);

	    if (notincluded(ptr, runlevel))
		xremove(d->d_name);
	}

	/*
	 * Seek for scripts which are included, link or
	 * correct order number if necessary.
	 */

#define xsymlink(x,y) if (symlink(x, y) < 0) \
	warn ("can not symlink(%s, %s%s): %s\n", x, rcd, y, strerror(errno))

	while (foreach(&script, &order, runlevel)) {
	    char * clink;
	    boolean found, this = chkfor(script, argv, argc);

	    if (*script == '$')		/* Do not link in virtual dependencies */
		continue;

	    sprintf(olink, "../%s",   script);
	    sprintf(nlink, "S%.2d%s", order, script);

	    found = false;
	    rewinddir(rcdir);
	    while ((clink = scan_for(rcdir, script, 'S'))) {
		found = true;
		if (strcmp(clink, nlink)) {
		    xremove(clink);		/* Wrong order, remove link */
		    if (!this)
			xsymlink(olink, nlink);	/* Not ours, but correct order */
		    if (this && !del)
			xsymlink(olink, nlink);	/* Restore, with correct order */
		} else {
		    if (del && this)
			xremove(clink);		/* Found it, remove link */
		}
	    }

	    if (this) {
		/*
		 * If we haven't found it and we shouldn't delete it
		 * we try to add it.
		 */
		if (!del && !found)
		    xsymlink(olink, nlink);
	    }

	    /* Start link done, now Kill link */

	    if (!strcmp(script, "kbd"))
		continue;  /* kbd should run on any runlevel change */

	    sprintf(nlink, "K%.2d%s", (maxorder + 1) - order, script);

	    found = false;
	    rewinddir(rcdir);
	    while ((clink = scan_for(rcdir, script, 'K'))) {
		found = true;
		if (strcmp(clink, nlink)) {
		    xremove(clink);		/* Wrong order, remove link */
		    if (!this)
			xsymlink(olink, nlink);	/* Not ours, but correct order */
		    if (this && !del)
			xsymlink(olink, nlink);	/* Restore, with correct order */
		} else {
		    if (del && this)
			xremove(clink);		/* Found it, remove link */
		}
	    }

	    /*
	     * One way runlevels:
	     * Remove kill links from rc0.d/, rc6.d/, and boot.d/.
	     */
	    if (runlevel < 1 || runlevel == 6 || runlevel > 7) {
		if (found)
		    xremove(nlink);
	    } else {
		if (this) {
		    /*
		     * If we haven't found it and we shouldn't delete it
		     * we try to add it.
		     */
		    if (!del && !found)
			xsymlink(olink, nlink);
		}
	    }
	}
	popd();
	closedir(rcdir);
    }
#endif

    /*
     * Back to the root(s)
     */
    popd();

    return 0;
}
