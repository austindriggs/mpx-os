#include <sys_req.h>
#include <string.h>
#include "help.h"

void help_command(void) {
    const char *helpMsg =
        "List Of Commands:\r\n"
        "help    - list commands\r\n"
        "exit    - exit the program\r\n"
        "version - Show OS version\r\n"
        "clock   - Print Real Time Clock\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}
