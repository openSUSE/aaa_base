extern int bootlog(const int lvl, const char *fmt, ...);
#define B_NOTICE	((int)'n')	/* Notice  */
#define B_DONE		((int)'d')	/* Done    */
#define B_FAILED	((int)'f')	/* Failed  */
#define B_SKIPPED	((int)'s')	/* Skipped */
#define B_UNUSED	((int)'u')	/* Unused  */
