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
    struct list_struct * prev, * next;
} list_t;

static inline void insert (list_t * new, list_t * head)
{
    list_t * prev = head;
    list_t * next = head->next;

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void delete (list_t * entry)
{
    list_t * prev = entry->prev;
    list_t * next = entry->next;

    next->prev = prev;
    prev->next = next;
}

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

typedef enum _boolean {false, true} boolean;
extern void follow_all();
extern void show_all();
extern void requiresl(const char * this, ...);
extern void requiresv(const char * this, const char * requires);
extern void runlevels(const char * this, const char * lvl);
extern int makeprov(const char * name, const char * script);
extern void setorder(const char * script, const int order);
extern int getorder(const char * script);
extern boolean notincluded(const char * script, const int runlevel);
extern boolean foreach(char ** script, int * order, const int runlevel);
extern void virtprov(const char * virt, const char * real);
extern int maxorder;
