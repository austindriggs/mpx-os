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
        "List of Commands [arguments]:\r\n\r\n"
        "\033[33mhelp\033[0m             [\033[36mverbose\033[0m]\r\n"
        "\033[33mversion\033[0m          [\033[36mall\033[0m|\033[36mhelp\033[0m]\r\n"
        "\033[33mexit\033[0m             [\033[36mhelp\033[0m|\033[36mforce\033[0m]\r\n"
        "\033[33mclock\033[0m            [\033[36mget\033[0m|\033[36mset\033[0m|\033[36mhelp\033[0m]  [<\033[36mdate\033[0m>|<\033[36mtime\033[0m>]\r\n"
        "\033[33mshow\033[0m             [<\033[36mname\033[0m>|\033[36mready\033[0m|\033[36mblocked\033[0m|\033[36mall\033[0m|\033[36mhelp\033[0m]\r\n"
        "\033[33mpriority set\033[0m     [<\033[36mname\033[0m>|\033[36mhelp\033[0m] [<\033[36mpriority\033[0m>]\r\n"
        "\033[33msuspend\033[0m          [<\033[36mname\033[0m>|\033[36mhelp\033[0m]\r\n"
        "\033[33mresume\033[0m           [<\033[36mname\033[0m>|\033[36mhelp\033[0m]\r\n\r\n"
	"For more help, run '\033[33mhelp\033[0m \033[36mverbose\033[0m' or see the user guide at https://github.com/WVU-CS450/MacaroniPenguins.\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

void help_help(void) {
    const char *helpMsg =
        "List of Commands [arguments]:\r\n\r\n"
        "\033[33mhelp\033[0m [\033[36mverbose\033[0m]\r\n"
        "  \033[33mhelp\033[0m            prints a basic help message\r\n"
        "  \033[33mhelp\033[0m \033[36mverbose\033[0m    prints this message.\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

void help_verbose(void) {
    help_help();
    exit_help();
    version_help();
    clock_help();
    set_priority_help();
    show_pcb_help();
    suspend_help();
    resume_help();

    const char *docMsg =
        "For more help, see the user guide at https://github.com/WVU-CS450/MacaroniPenguins.\r\n";
    sys_req(WRITE, COM1, docMsg, strlen(docMsg));
}

void help_command(const char *args) {
    
    if (args == NULL || *args == '\0') {
        help_message();
    }
    else if (strcmp(args, "help") == 0) {
        help_message();
    }
    else if (strcmp(args, "verbose") == 0) {
        help_verbose();
    }
    else {
        const char *argMsg = "\033[31mInvalid argument. Please try again.\033[0m\r\n\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        help_help();
    }   
}

