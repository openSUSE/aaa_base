#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <paths.h>

#ifndef  BLOGD
# define BLOGD	"blogd"
#endif

/*
 * Use Esacape sequences which are handled by console
 * driver of linux kernel but not used.  We transport
 * with this our informations to the parser of blogd
 * whereas the linux console throw them away.
 * Problem: Does not work on serial console.
 */

#define ESS	"\233["
#define ESN1	"\033]1"	/* Notice  */
#define ESN2	"\033]2"	/* Done    */
#define ESN3	"\033]3"	/* Failed  */
#define ESN4	"\033]4"	/* Skipped */
#define ESN5	"\033]5"	/* Unused  */
#define ESNE	"\033];"	/* End     */

int bootlog_h(const int lvl)
{
    char pidfile[strlen(_PATH_VARRUN)+strlen(BLOGD)+1+3+1];
    char *head = ESN1;
    char *term;
    struct stat st;

    strcat(strcat(strcpy(pidfile, _PATH_VARRUN), BLOGD), ".pid");
    if (lstat(pidfile, &st) < 0)
	return -1;

    if (!(term = getenv("TERM")))
	return -1;

    if (strcmp(term, "linux"))
	return -1;

    switch (lvl) {
	case 'n':
	    head = ESN1;
	    break;
	case 'd':
	    head = ESN2;
	    break;
	case 'f':
	    head = ESN3;
	    break;
	case 's':
	    head = ESN4;
	    break;
	case 'u':
	    head = ESN5;
	    break;
	case '?':
	default:
	    head = ESN1;
	    break;
    }

    printf("%s", head);
    return 0;
}

void bootlog_e()
{
    printf("%s", ESNE);
}

void bootlog_m(const char *mesg)
{
    while(*mesg)
	    printf(ESS "%c", (unsigned int)*mesg++);
}

int bootlog(const int lvl, const char *mesg)
{
    if (bootlog_h(lvl) < 0)
	return -1;
    bootlog_m(mesg);
    bootlog_e();
    return 0;
}
