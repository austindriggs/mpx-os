#include <sys_req.h>
#include <string.h>
#include "version.h"

void version_help(void) {
    const char *helpMsg =
        "\r\n\033[33mversion\033[0m [\033[36mall\033[0m|\033[36mhelp\033[0m]\r\n"
        "  \033[33mversion\033[0m         prints information about the current version as: R<x> <date> <hash> (<status>)\r\n"
        "  \033[33mversion\033[0m \033[36mall\033[0m     prints information about all version history as: R<x> <date> <hash>\r\n"
        "  \033[33mversion\033[0m \033[36mhelp\033[0m    prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

void version_latest(void) {
    char buffer[128];
    char *write_ptr = buffer;
    const char *read_ptr;

    // main version
    *write_ptr++ = 'R';
    *write_ptr++ = '2';
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
	"R1 2025-09-12 956ee13\r\n"
        "R0 2025-08-24 6738747\r\n";
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
        const char *argMsg = "\033[31mInvalid argument. Please try again.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        version_help();
    }
}
