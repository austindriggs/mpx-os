#include <sys_req.h>
#include <string.h>
#include "version.h"

#ifndef GIT_DATE
#define GIT_DATE "unknown"
#endif

#ifndef GIT_HASH
#define GIT_HASH "unknown"
#endif

#ifndef GIT_DIRTY
#define GIT_DIRTY "unknown"
#endif

void version_help(void) {
    const char *helpMsg =
        "\r\nversion [all]\r\n"
        "  version         prints information about the current version\r\n"
        "  version all     prints information about all version history\r\n"
        "  version help    prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

//void version_latest(void) {
//    const char *latestMsg = "R1\r\n";
//    sys_req(WRITE, COM1, latestMsg, strlen(latestMsg));
//}

void version_latest(void) {
    char buffer[128];
    char *write_ptr = buffer;
    const char *read_ptr;

    // main version
    *write_ptr++ = 'R';
    *write_ptr++ = '1';
    *write_ptr++ = ' ';

    // git metadata
    read_ptr = GIT_DATE;
    while (*read_ptr) {
        *write_ptr++ = *read_ptr++;
    }
    *write_ptr++ = ' ';

    read_ptr = GIT_HASH;
    while (*read_ptr) {
        *write_ptr++ = *read_ptr++;
    }
    *write_ptr++ = ' ';

    *write_ptr++ = '(';
    read_ptr = GIT_DIRTY;
    while (*read_ptr) {
        *write_ptr++ = *read_ptr++;
    }
    *write_ptr++ = ')';

    *write_ptr++ = '\r';
    *write_ptr++ = '\n';
    *write_ptr = '\0';

    // print out
    sys_req(WRITE, COM1, buffer, write_ptr - buffer);
}

void version_history(void) {
    const char *historyMsg =
        "R0 2025-08-24 6738747"
        "\r\n";
    sys_req(WRITE, COM1, historyMsg, strlen(historyMsg));
}

void version_command(const char *args) {
    if (args == NULL || *args == '\0') {
        version_latest();
    }
    else if (strcmp(args, "all") == 0) {
        version_latest();
        version_history();
    }
    else if (strcmp(args, "help") == 0) {
        version_help();
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
}
