#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <ctype.h>
#include "listing.h"

#define MAX_DEEP 20

#define LVL_HALT	0x001
#define LVL_ONE		0x002
#define LVL_TWO		0x004
#define LVL_THREE	0x008
#define LVL_FOUR	0x010
#define LVL_FIVE	0x020
#define LVL_REBOOT	0x040
#define LVL_SINGLE	0x080
#define LVL_BOOT	0x100

extern const char *delimeter;
extern void error (const char *fmt, ...);
extern void warn (const char *fmt, ...);

int maxorder = 0;

typedef struct list_struct {
    struct list_struct * prev, * next;
} list_t;

inline static void insert (list_t * new, list_t * head)
{
    list_t * prev = head;
    list_t * next = head->next;

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
#define getdir(list)  list_entry((list), struct dir_struct, d_list)
#define getlink(list) list_entry((list), struct link_struct, l_list)

typedef struct link_struct {
    list_t		l_list;
    struct dir_struct * target;
} link_t;

typedef struct dir_struct {
    list_t	      d_list;
    link_t	        link;   /* first link in a directory */
    char	       order;
    ino_t	       inode;
    char	      * name;
    unsigned int	 lvl;
} dir_t;

static list_t dirs = { &(dirs), &(dirs) }, * d_start = &dirs;

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
	this->name  = strdup(name);
	if (!this->name)
	    goto err;
	this->order = 0;
	this->inode = 0;
	this->lvl   = 0;
	ptr = d_start->prev;
	goto out;
    }
err:
    ptr = NULL;
    error("%s", strerror(errno));
out:
    return getdir(ptr);
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
	insert(&(dir->link.l_list), l_start->prev);
	goto out;
    }

    this = (link_t *)malloc(sizeof(link_t));
    if (this) {
	insert(&(this->l_list), l_start->prev);
	this->target = target;
	goto out;
    }
out:
}

/*
 * Recursively called function to follow all
 * links within a service dir.
 * Just like a `find * -follow' within a directory tree
 * of deep one with cross linked dependencies.
 */
static void __follow (dir_t * dir, int level)
{
    dir_t *tmp;
    register int deep = level; /* Link deep, maybe we're called recursive */

    for (tmp = dir; tmp; tmp = tmp->link.target) {
	list_t * dent, * l_start = &(tmp->link.l_list);

	if (++deep > MAX_DEEP) {
	    static int warned = 0;
	    if (warned++ < 5)
		warn("Max recursions deep %d reached\n",  MAX_DEEP);
	    break;
	}

	/*
	 * As higher the link deep, as higher the start order.
	 */
	if (tmp->order < deep)
	    tmp->order = deep;

	if (maxorder < tmp->order)
	    maxorder = tmp->order;

	/*
	 * If more than one link is included, follow them all
	 */
	for (dent = l_start->next; dent != l_start; dent = dent->next) {
	    dir_t * target = getlink(dent)->target;

	    __follow(target, deep);
	}
    }
}

/*
 * Helper for follow_all: start with deep zero.
 */
inline static void follow(dir_t * dir)
{
    int deep = 0;	       /* Link deep, starts here with zero */
    __follow(dir, deep);
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

    if (dir->inode)		/* Skip it because we have read it */
	goto out;

    if (!target)		/* No target available */
	goto out;

    {   /* No full loop required because we seek for the lowest order */

	list_t * dent, * l_start = &(dir->link.l_list);

	if (min > target->order)
	    min = target->order;

	if (maxorder < target->order)
	    maxorder = target->order;

	lvl |= target->lvl;

	for (dent = l_start->next; dent != l_start; dent = dent->next) {
	    target = getlink(dent)->target;

	    if (++deep > MAX_DEEP)
		break;

	    if (min > target->order)
		min = target->order;

	    if (maxorder < target->order)
		maxorder = target->order;

	    lvl |= target->lvl;
	}
	dir->order = min - 1;	/* Set guessed order of this unknown script */
	dir->lvl |= lvl;	/* Set guessed runlevels of this unknown script */

	if (maxorder < dir->order)
	    maxorder = dir->order;
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
}

/*
 * For debuging: show all services
 */
void show_all()
{
    list_t *tmp;
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	dir_t * dir = getdir(tmp);
	if (dir->inode)
	    printf("%.2d %s 0x%.2x\n", dir->order, dir->name, dir->lvl);
	else
	    printf("%.2d %s 0x%.2x (guessed)\n", dir->order, dir->name, dir->lvl); 
    }
}

/*
 *  Used within loops to get names not included in this runlevel.
 */
boolean notincluded(const char * name, const int runlevel)
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
	    error("Wrong runlevel %d\n", runlevel);
    }

    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	dir_t * dir = getdir(tmp);

	if (!dir->inode)	/* No such file */
	    continue;

	if (dir->lvl & lvl)	/* Same runlevel */
	    continue;

	if (strcmp(name, dir->name))
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
boolean foreach(char ** name, int * order, const int runlevel)
{
    static list_t * tmp;
    dir_t * dir;
    boolean ret;
    ino_t inode;
    unsigned int lvl = 0;

    if (!*name)
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
	    error("Wrong runlevel %d\n", runlevel);
    }

    do {
	ret = false;
	if (tmp == d_start)
	    break;

	dir = getdir(tmp);

	ret = true;
	*name  = dir->name;
	*order = dir->order;
	inode  = dir->inode;

	tmp = tmp->next;

    } while (!inode || !(dir->lvl & lvl));

    return ret;
}

/*
 * The same as requiresv, bbut here we use
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

    while ((token = strsep(&tmp, delimeter))) {
	if (*token) {
	    ln_sf(this, token);
	}
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
    char * token, *tmp = strdupa(lvl);
    int num;

    while ((token = strsep(&tmp, delimeter))) {
	if (!*token || strlen(token) != 1)
	    continue;
	if (!strpbrk(token, "0123456SB"))
	    continue;
	if (*token == 'S')
	    num = 7;
	else if (*token == 'B')
	    num = 8;
	else
	    num = atoi(token);
	switch (num) {
	    case 0: dir->lvl |= LVL_HALT;   break;
	    case 1: dir->lvl |= LVL_ONE;    break;
	    case 2: dir->lvl |= LVL_TWO;    break;
	    case 3: dir->lvl |= LVL_THREE;  break;
	    case 4: dir->lvl |= LVL_FOUR;   break;
	    case 5: dir->lvl |= LVL_FIVE;   break;
	    case 6: dir->lvl |= LVL_REBOOT; break;
	    case 7: dir->lvl |= LVL_SINGLE; break;
	    case 8: dir->lvl |= LVL_BOOT;   break;
	    default: break;
	}
    }
}

/*
 * Reorder all services starting with a service
 * being in same runlevels.
 */
void setorder(const char * name, const int order)
{
    dir_t * dir = providedir(name);
    list_t * tmp;
    int offset = 0;

    if (dir->order >= order)
	goto out;

    offset = order - dir->order;
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next) {
	dir_t * cur = getdir(tmp);

	/*
	 * The scripts should be belong to
	 * similar runlevels.
	 */
	if (!(cur->lvl & dir->lvl))
	    continue;

	if (cur->order > dir->order)
	    cur->order += offset;

	if (maxorder < cur->order)
	    maxorder = cur->order;
    }
    dir->order += offset;
    if (maxorder < dir->order)
	maxorder = dir->order;

    /*
     * Guess order of not installed scripts in comparision
     * to the well known scripts.
     */
    for (tmp = d_start->next; tmp != d_start; tmp = tmp->next)
	guess_order(getdir(tmp));
out:
}

/*
 * Set order number of a service. This may change
 * ordering. Only usefull if order is known and/or
 * no dependencies have to satisfied.
 */
void minorder(const char * name, const int order)
{
    dir_t * dir = providedir(name);
    if (dir->order < order)
	dir->order = order;
}

/*
 * Get the order of a service.
 */
int getorder(const char * name)
{
    dir_t * dir = providedir(name);
    return getdir(dir)->order;
}

/*
 * Provide a service if the corresponding script
 * was read and the inode number was detected.
 * A given inode marks a service as a readed one.
 */
int makeprov(const char * name, const ino_t inode)
{
    dir_t * dir = providedir(name);
    int ret = 0;

    if (!dir->inode) {
	dir->inode = inode;
	goto out;
    }

    if (dir->inode != inode)
	ret = -1;
out:
    return ret;
}
