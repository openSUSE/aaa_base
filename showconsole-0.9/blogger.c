#include <stdio.h>
#include <unistd.h>
#include "libblogger.h"

int main(int argc, char * argv[])
{
    int c, lvl = 'n';

    while ((c = getopt(argc, argv, "ndfsu")) != -1) {
	switch (c) {
	case B_NOTICE:
	case B_DONE:
	case B_FAILED:
	case B_SKIPPED:
	case B_UNUSED:
	    lvl = c;
	    break;
	case '?':
	default:
	    lvl = B_NOTICE;
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (!argc)
	exit(0);

    c = argc;
    if (bootlog(lvl, argv[0]) < 0)
	exit(0);

    argv++;
    argc--;

    for (c = 0; c < argc; c++)
	bootlog(-1, " %s", argv[c]);
    bootlog(-1, "\n");

    return 0;
}
