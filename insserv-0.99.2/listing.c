/*
 * listing.c
 *
 * Copyright 2000 Werner Fink, 2000 SuSE GmbH Nuernberg, Germany.
 *
 * This source is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <ctype.h>
#include "listing.h"

#define MAX_DEEP 99

/*
 * LVL_BOOT is already done if  one of the LVL_ALL will be entered.
 */
#define LVL_ALL		\
(LVL_HALT|LVL_ONE|LVL_TWO|LVL_THREE|LVL_FOUR|LVL_FIVE|LVL_REBOOT|LVL_SINGLE)

int maxorder = 0;  /* Maximum order of runlevels 0 upto 6 and S */

/* See listing.c for list_t and list_entry() macro */
#define getdir(list)  list_entry((list), struct dir_struct, d_list)
#define getlink(list) list_entry((list), struct link_struct, l_list)
#define getnode(list) list_entry((list), struct loop_struct, n_list)

/*
 * We handle services (aka scripts) as directories because
 * dependencies can be handels as symbolic links therein.
 * A provided service will be linked into a required service.
 * For the general typ of linked list see listing.h.
 */

typedef struct link_struct {
    list_t		l_list;	/* The linked list to other symbolic links */
    struct dir_struct * target;
} link_t;			/* This is a "symbolic link" */

typedef struct dir_struct {
    list_t	      d_list;	/* The linked list to other directories */
    link_t	        link;   /* First symbolic link in this directory */
    char	       order;
    char	      * name;
    char	    * script;
    unsigned int	 lvl;
} dir_t;			/* This is a "directory" */

static list_t dirs = { &(dirs), &(dirs) }, * d_start = &dirs;

typedef struct loop_struct {
    list_t	      n_list;	/* The linked list to other symbolic links */
    struct dir_struct * node;
} loop_t;			/* This is a "symbolic link" */

static list_t nodes = { &(nodes), &(nodes) }, * n_start = &nodes;

/*
 * Provide or find a service dir, set initial states and
 * link it into the maintaining if a new one.
 */
static dir_t * providedir(const char * name)
{
    dir_t  * this;
    list_t * ptr;

    for (ptr = d_start->next; ptr != d_start; ptr = ptr->next)
	if (!strcmp(getdir(ptr)->name,name))
	    goto out;

    this = (dir_t *)malloc(sizeof(dir_t));
    if (this) {
	list_t * l_start = &(this->link.l_list);
	insert(&(this->d_list), d_start->prev);
	l_start->next = l_start;
	l_start->prev = l_start;
	this->link.target = NULL;
	this->name   = xstrdup(name);
	this->script = NULL;
	this->order  = 0;
	this->lvl    = 0;
	ptr = d_start->prev;
	goto out;
    }
    ptr = NULL;
    error("%s", strerror(errno));
out:
    return getdir(ptr);
}

/*
 * Find a service dir by its script name.
 */
static dir_t * findscript(const char * script)
{
    dir_t  * this = NULL;
    list_t * ptr;
    register boolean found = false;

    for (ptr = d_start->next; ptr != d_start; ptr = ptr->next) {
	dir_t * tmp = getdir(ptr);

	if (!tmp->script)
	    continue;

	if (!strcmp(tmp->script,script)) {
	    found = true;
	    break;
	}
    }

    if (found)
	this = getdir(ptr);

    return this;
}

/*
 * Link a provided service into a required service.
 * If the services do not exist, they will be created.
 */
static void ln_sf(const char * isprovided, const char * itrequires)
{
    dir_t * target = providedir(isprovided);
    dir_t * dir    = providedir(itrequires);
    list_t * l_start = &(dir->link.l_list);
    link_t * this;

    if (target == dir)
	goto out;

    if (!dir->link.target) {
	dir->link.target = target;
	goto out;
    }

    this = (link_t *)malloc(sizeof(link_t));
    if (this) {
	insert(&(this->l_list), l_start->prev);
	this->target = target;
	goto out;
    }
    error("%s", strerror(errno));
out:
}

/*
 * Remember loops
 */
static boolean remembernode (dir_t * dir)
{
    list_t * ptr;
    loop_t * this;
    boolean ret = false;

    for (ptr = n_start->next; ptr !=  n_start; ptr = ptr->next)
	if (dir == getnode(ptr)->node) {
	    ret = true;
	    goto out;
	}

    this = (loop_t *)malloc(sizeof(loop_t));
    if (this) {
	insert(&(this->n_list), n_start->prev);
	this->node = dir;
	goto out;
    }
    error("%s", strerror(errno));
out:
    return ret;
}

/*
 * Recursively called function to follow all
 * links within a service dir.
 * Just like a `find * -follow' within a directory tree
 * of depth one with cross linked dependencies.
 */
static void __follow (dir_t * dir, dir_t * skip, int level, int offset)
{
    dir_t *tmp;
    register int deep = level;	/* Link depth, maybe we're called recursive */
    static boolean warned = false;

#ifdef SYSFAC_NOT_EXPANDED
    if (*dir->name == '$') 	/* Dirty hack to decrease the order caused by */
	offset--;		/* system facilities, may fail */
#endif

    for (tmp = dir; tmp; tmp = tmp->link.target) {
	list_t * dent, * l_start = &(tmp->link.l_list);

	if (++deep > MAX_DEEP) {
	    if (!warned)
		warn("Max recursions depth %d reached\n",  MAX_DEEP);
	    warned = true;
	    break;
	}

	if (!(dir->lvl & tmp->lvl))
	     continue;		/* Not same boot level */

	/*
	 * As higher the link depth, as higher the start order.
	 */
	if (tmp->order < (deep + offset))
	    tmp->order = (deep + offset);

	/*
	 * If more than one link is included, follow them all
	 */
	for (dent = l_start->next; dent != l_start; dent = dent->next) {
	    dir_t * target = getlink(dent)->target;

	    if (!(dir->lvl & target->lvl))
		continue;	/* Not same boot level */

	    if (target == dir)
		break;		/* Loop detected */

	    if (skip && target == skip) {
		if (!remembernode(skip))
		    warn("There is a loop at service %s\n",  skip->name);
		break;		/* Cross over detected */
	    }

	    __follow(target, tmp, deep, offset);
	}
    }
}

/*
 * Helper for follow_all: start with depth zero.
 */
inline static void follow(dir_t * dir)
{
    /* Link depth and offset starts here with zero */
    __follow(dir, NULL, 0, 0);
}

/*
 * Put not existing services into a guessed order.
 * The maximal order of not existing services can be
 * set if they are required by existing services.
 */
static void guess_order(dir_t * dir)
{
    dir_t * target = dir->link.target;
    register int min = 99, lvl = 0;
    int deep = 0;

    if (dir->script)		/* Skip it because we have read it */
	goto out;

    if (*dir->name == '$')	/* Don't touch our system facilities */
	goto out;

    if (!target)		/* No target available */
	goto out;

    {   /* No full loop required because we seek for the lowest order */

	list_t * dent, * l_start = &(dir->link.l_list);

	if (min > target->order)
	    min = target->order;

	lvl |= target->lvl;

	for (dent = l_start->next; dent != l_start; dent = dent->next) {
	    target = getlink(dent)->target;

	    if (++deep > MAX_DEEP)
		break;

	    if (target == dir)
		break;		/* Loop detected */

	    if (min > target->order)
		min = target->order;

	    lvl |= target->lvl;
	}
	if (min > 1) {		/* Set guessed order of this unknown script */
	    dir->order = min - 1;
	    dir->lvl |= lvl;	/* Set guessed runlevels of this unknown script */
	} else
	    dir->lvl  = LVL_BOOT;
    }

out:
}

/*
 * Follow all services and their dependencies recursivly.
 */
void follow_all()
{
    list_t *tmp;

    /*
     * Follow all scripts and calculate the main ordering.
     */
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next)
	follow(getdir(tmp));

    /*
     * Guess order of not installed scripts in comparision
     * to the well known scripts.
     */
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next)
	guess_order(getdir(tmp));

    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	if (!(getdir(tmp)->lvl & LVL_ALL))
	    continue;
	if (maxorder < getdir(tmp)->order)
	    maxorder = getdir(tmp)->order;
    }
}

/*
 * For debuging: show all services
 */
#if 1 // defined(DEBUG) && (DEBUG > 0)
void show_all()
{
    list_t *tmp;
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	dir_t * dir = getdir(tmp);
	if (dir->script)
	    fprintf(stderr, "%.2d %s 0x%.2x (%s)\n",
		   dir->order, dir->script, dir->lvl, dir->name);
	else
	    fprintf(stderr, "%.2d %s 0x%.2x (%%%s)\n",
		   dir->order, dir->name, dir->lvl, *dir->name == '$' ? "system" : "guessed");
    }
}
#endif

/*
 * Used within loops to get names not included in this runlevel.
 */
boolean notincluded(const char * script, const int runlevel)
{
    list_t *tmp;
    boolean ret = false;
    unsigned int lvl = 0;

    switch (runlevel) {
	case 0: lvl = LVL_HALT;   break;
	case 1: lvl = LVL_ONE;    break;
	case 2: lvl = LVL_TWO;    break;
	case 3: lvl = LVL_THREE;  break;
	case 4: lvl = LVL_FOUR;   break;
	case 5: lvl = LVL_FIVE;   break;
	case 6: lvl = LVL_REBOOT; break;
	case 7: lvl = LVL_SINGLE; break;
	case 8: lvl = LVL_BOOT;   break;
	default:
	    warn("Wrong runlevel %d\n", runlevel);
    }

    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	dir_t * dir = getdir(tmp);

	if (!dir->script)	/* No such file */
	    continue;

	if (dir->lvl & lvl)	/* Same runlevel */
	    continue;

	if (strcmp(script, dir->script))
	    continue;		/* Not this file */

	ret = true;		/* Not included */
	break;
    }

    return ret;
}

/*
 * Used within loops to get names and order out
 * of the service lists of a given runlevel.
 */
boolean foreach(char ** script, int * order, const int runlevel)
{
    static list_t * tmp;
    dir_t * dir;
    boolean ret;
    unsigned int lvl = 0;

    if (!*script)
	tmp  = d_start->next;

    switch (runlevel) {
	case 0: lvl = LVL_HALT;   break;
	case 1: lvl = LVL_ONE;    break;
	case 2: lvl = LVL_TWO;    break;
	case 3: lvl = LVL_THREE;  break;
	case 4: lvl = LVL_FOUR;   break;
	case 5: lvl = LVL_FIVE;   break;
	case 6: lvl = LVL_REBOOT; break;
	case 7: lvl = LVL_SINGLE; break;
	case 8: lvl = LVL_BOOT;	  break;
	default:
	    warn("Wrong runlevel %d\n", runlevel);
    }

    do {
	ret = false;
	if (tmp == d_start)
	    break;

	dir = getdir(tmp);

	ret = true;
	*script = dir->script;
	*order = dir->order;

	tmp = tmp->next;

    } while (!*script || !(dir->lvl & lvl));

    return ret;
}

/*
 * The same as requiresv, but here we use
 * several arguments instead of one string.
 */
void requiresl(const char * this, ...)
{
    va_list ap;
    char * requires;
    int count = 0;

    va_start(ap, this);
    while ((requires = va_arg(ap, char *))) {
	ln_sf(this, requires);
	count++;
    }
    va_end(ap);
    if (!count)
	providedir(this);
}

/*
 * THIS services REQUIRES that service.
 */
void requiresv(const char * this, const char * requires)
{
    int count = 0;
    char * token, * tmp = strdupa(requires);

    if (!tmp)
	error("%s", strerror(errno));

    while ((token = strsep(&tmp, delimeter))) {
	if (*token)
	    ln_sf(this, token);
	count++;
    }
    if (!count)
	providedir(this);
}

/*
 * Set the runlevels of a service.
 */
void runlevels(const char * this, const char * lvl)
{
    dir_t * dir = providedir(this);

    dir->lvl = str2lvl(lvl);
}

/*
 * Two helpers for runlevel bits and strings.
 */
unsigned int str2lvl(const char * lvl)
{
    char * token, *tmp = strdupa(lvl);
    int num;
    unsigned int ret = 0;

    if (!tmp)
	error("%s", strerror(errno));

    while ((token = strsep(&tmp, delimeter))) {
	if (!*token || strlen(token) != 1)
	    continue;
	if (!strpbrk(token, "0123456sSbB"))
	    continue;
	if (*token == 'S' || *token == 's')
	    num = 7;
	else if (*token == 'B' || *token == 'b')
	    num = 8;
	else
	    num = atoi(token);
	switch (num) {
	    case 0: ret |= LVL_HALT;   break;
	    case 1: ret |= LVL_ONE;    break;
	    case 2: ret |= LVL_TWO;    break;
	    case 3: ret |= LVL_THREE;  break;
	    case 4: ret |= LVL_FOUR;   break;
	    case 5: ret |= LVL_FIVE;   break;
	    case 6: ret |= LVL_REBOOT; break;
	    case 7: ret |= LVL_SINGLE; break;
	    case 8: ret |= LVL_BOOT;   break;
	    default:
		warn("Wrong runlevel %d\n", num);
	}
    }

    return ret;
}

char * lvl2str(const unsigned int lvl)
{
    char str[20], * ptr = str;
    int num;
    unsigned int bit = 0x001;

    memset(ptr , '\0', sizeof(str));
    for (num = 0; num < 9; num++) {
	if (bit & lvl) {
	    if      (num < 7)
		*(ptr++) = num + 48;
	    else if (num == 7)
		*(ptr++) = 'S';
	    else if (num == 8)
		*(ptr++) = 'B';
	    else
		error("Wrong runlevel %d\n", num + 48);
	    *(ptr++) = ' ';
	}
	bit <<= 1;
    }
    return xstrdup(str);
}

/*
 * Reorder all services starting with a service
 * being in same runlevels.
 */
void setorder(const char * script, const int order)
{
    dir_t * dir = findscript(script);
    list_t * tmp;
    int offset = 0;

    if (!dir)
	goto out;

    if (dir->order >= order) /* nothing to do */
	goto out;

    /*
     * Follow the script and re-calculate the ordering.
     */
    offset = order - (dir->order + 1);
    __follow(dir, NULL, dir->order, offset);

    /*
     * Guess order of not installed scripts in comparision
     * to the well known scripts.
     */
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next)
	guess_order(getdir(tmp));

    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	if (!(getdir(tmp)->lvl & LVL_ALL))
	    continue;
	if (maxorder < getdir(tmp)->order)
	    maxorder = getdir(tmp)->order;
    }

out:
}

/*
 * Get the order of a service.
 */
int getorder(const char * script)
{
    dir_t * dir = findscript(script);
    int order = -1;

    if (dir)
	order = getdir(dir)->order;

    return order;
}

/*
 * Provide a service if the corresponding script
 * was read and the scripts name was remembered.
 * A given script name marks a service as a readed one.
 * One script and several provided facilities leads
 * to the same order for those facilities.
 */
int makeprov(const char * name, const char * script)
{
    dir_t * alias = findscript(script);
    dir_t * dir   = providedir(name);
    int ret = 0;

    if (!dir->script) {
	if (!alias) {
	    dir->script = xstrdup(script);
	} else
	    dir->script = alias->script;
	goto out;
    }

    if (strcmp(dir->script, script))
	ret = -1;
out:
    return ret;
}

/*
 * The virtual facilities provides real services.
 */
void virtprov(const char * virt, const char * real)
{
    char * token, * ptr;
    dir_t * dir = providedir(virt);

    if (!real)
	goto out;

    ptr = strdupa(real);
    if (!ptr)
	error("%s", strerror(errno));

    while ((token = strsep(&ptr, delimeter))) {
	if (*token) {
	    dir_t * tmp;
	    /*
	     * optional real services are noted by a `+' sign
	     */
	    if (*token == '+')
		token++;
	    tmp = providedir(token);
	    ln_sf(virt, token);
	    dir->lvl |= tmp->lvl;
	}
    }

out: 
    if (!dir->lvl)		/* Unknown runlevel means before any runlevel */
	dir->lvl |= LVL_BOOT;
}
