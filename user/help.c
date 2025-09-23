#include <sys_req.h>
#include <string.h>
#include "help.h"
#include "exit.h"
#include "version.h"
#include "clock.h"
#include "setPriority.h"
#include "showPCB.h"
#include "ready.h"

void help_message(void) {
    // command help
    const char *helpMsg =
        "List Of Commands:\r\n"
        "\r\nhelp - prints this message\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
    exit_help();
    version_help();
    clock_help();
    set_priority_help();
    show_pcb_help();
    suspend_help();
    resume_help();

    // more help
    const char *docMsg = "For more help, see the user guide at https://github.com/WVU-CS450/MacaroniPenguins.\r\n";
    sys_req(WRITE, COM1, docMsg, strlen(docMsg));
}

void help_command(const char *args) {
    if (args == NULL || *args == '\0') {
        help_message();
    }
    else if (strcmp(args, "help") == 0) {
	help_message();
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
	sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
}
