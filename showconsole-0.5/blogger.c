#include <stdio.h>
#include <unistd.h>
#include "libblogger.h"

/*
 * This does not work on serial console
 */
int main(int argc, char * argv[])
{
    int c, lvl = 'n';

    while ((c = getopt(argc, argv, "ndfsu")) != -1) {
	switch (c) {
	case 'n':
	case 'd':
	case 'f':
	case 's':
	case 'u':
	    lvl = c;
	    break;
	case '?':
	default:
	    lvl = 'n';
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (!argc)
	exit(0);

    if (bootlog_h(lvl) < 0)
	exit(0);
    c = argc;
    while (c--)
	bootlog_m(argv[c]);
    bootlog_e();

    return 0;
}
