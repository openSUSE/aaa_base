/*
 * listing.h
 *
 * Copyright 2000 Werner Fink, 2000 SuSE GmbH Nuernberg, Germany.
 *
 * This source is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

typedef struct list_struct {
    struct list_struct * next, * prev;
} list_t;

/*
 * Insert new entry as next member.
 */
static inline void insert (list_t * new, list_t * here)
{
    list_t * prev = here;
    list_t * next = here->next;

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/*
 * Remove entries, note that the pointer its self remains.
 */
static inline void delete (list_t * entry)
{
    list_t * prev = entry->prev;
    list_t * next = entry->next;

    next->prev = prev;
    prev->next = next;
}

static inline void join(list_t *list, list_t *head)
{
    list_t *first = list->next;

    if (first != list) {
	list_t *last = list->prev;
       	list_t *at = head->next;

       	first->prev = head;
       	head->next = first;

       	last->next = at;
       	at->prev = last;
    }
}

static inline int list_empty(list_t *head)
{
        return head->next == head;
}

#define list_entry(ptr, type, member)	\
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
#define list_for_each(pos, head)	\
	for (pos = (head)->next; pos != (head); pos = pos->next)
#define list_for_each_prev(pos, head)	\
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

typedef enum _boolean {false, true} boolean;
extern void follow_all();
extern void show_all();
extern void requiresl(const char * this, ...);
extern void requiresv(const char * this, const char * requires);
extern void runlevels(const char * this, const char * lvl);
extern unsigned int str2lvl(const char * lvl);
extern char * lvl2str(const unsigned int lvl);
extern int makeprov(const char * name, const char * script);
extern void setorder(const char * script, const int order, boolean recursive);
extern int getorder(const char * script);
extern boolean notincluded(const char * script, const int runlevel);
extern boolean foreach(char ** script, int * order, const int runlevel);
extern void virtprov(const char * virt, const char * real);
extern int maxorder;

/*
 * Common short cuts
 */
extern const char *delimeter;
extern void error (const char *fmt, ...);
extern void warn (const char *fmt, ...);

static inline char * xstrdup(const char *s)
{
    char * r = strdup(s);
    if (!r)
	error("%s", strerror(errno));
    return r;
} 

#define xreset(ptr)	\
	{char * tmp = (char *)ptr; if (ptr && *tmp) free(ptr);} ptr = NULL
#define xremove(x) if (remove(x) < 0) \
	warn ("can not remove(%s%s): %s\n", rcd, x, strerror(errno))
#define xsymlink(x,y) if (symlink(x, y) < 0) \
	warn ("can not symlink(%s, %s%s): %s\n", x, rcd, y, strerror(errno))

/*
 * Bits of the runlevels
 */
#define LVL_HALT	0x001
#define LVL_ONE		0x002
#define LVL_TWO		0x004
#define LVL_THREE	0x008
#define LVL_FOUR	0x010
#define LVL_FIVE	0x020
#define LVL_REBOOT	0x040
#define LVL_SINGLE	0x080
#define LVL_BOOT	0x100
