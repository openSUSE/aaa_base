extern void pushd(const char * path);
extern void popd(void);
extern char * fetchtty(const pid_t pid, const pid_t ppid);
extern void prepareIO(void (*rfunc)(int), void (*cfunc)(void), const int in, const int out);
extern void safeIO (void);
extern void closeIO(void);
