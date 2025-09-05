#include "comhand.h"
#include <string.h>
#include <sys_req.h>
#include "help.h"
#include "exit.h"
#include "version.h"

// penguin ASCII image on startup
void com_startup(void) {
    sys_req(WRITE, COM1, "\r\n-------------------------------------------------------\r\n", 60);
    sys_req(WRITE, COM1, "\r\n", 3);
    const char *penguinMsg =
        "      \033[33m\\|/\033[0m         Welcome to \033[33mMacaroniOS\033[0m!\n"
        "   -=(o ).        \033[36m$(version intro)\033[0m\n"
        "      '.-.\\\n"
        "      /|  \\\\      CS450: Operating Systems Structure\n"
        "     ' |  ||\n"
        "       _\\_):,_    Type '\033[33mhelp\033[0m' or '\033[31mexit\033[0m'.\r\n\r\n";

    sys_req(WRITE, COM1, penguinMsg, strlen(penguinMsg));
}

// trim Function for input (trims \r and \n from input)
void trim_Input(char *str){
    int length = strlen(str);
    while (length > 0 && (str[length - 1] == '\n' || str[length - 1] == '\r')){
        str[length - 1] = '\0';
        length --;
    }
}

// command handler (DOES NOT handle arguments)
void comhand(void)
{
    // startup message
    com_startup();

    // loop through the entire buffer
    for (;;) {
        char buf[100] = {0};

        // read command
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        buf[nread] = '\0'; // adds null terminator
        trim_Input(buf);    // trims \r\n
        sys_req(WRITE, COM1, "\r\n", 2); // Start new line

	// command logic: each function handles its own argument(s) for better encapsulation
	if (strncmp(buf, "exit", 4) == 0) {
	    char *args = buf + 4;
	    while (*args == ' ') args++;
            if (exit_command(args)) {
                return;
            }
        }
	else if (strncmp(buf, "version", 4) == 0) {
	    char *args = buf + 7;
	    while (*args == ' ') args++;
	    version_command(args);
	}
        else if (strncmp(buf, "help", 4) == 0) {
	    char *args = buf + 4;
            while (*args == ' ') args++;
            help_command(args);
        }
        else if (buf[0] == '\0') {
            sys_req(WRITE, COM1, "\r", 2);
        }
        else {
            const char *invalidMsg = "Invalid command. Try again, type 'help' for all commands.\r\n";
            sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));
        }
    }
}
