#ifndef VERSION_H
#define VERSION_H

#ifndef GIT_DATE
#define GIT_DATE "unknown"
#endif

#ifndef GIT_HASH
#define GIT_HASH "unknown"
#endif

#ifndef GIT_DIRTY
#define GIT_DIRTY "unknown"
#endif

void version_help(void);

void version_latest(void);

void version_history(void);

void version_command(const char *args);

#endif
