#include "comhand.h"
#include <string.h>
#include <sys_req.h>
#include "help.h"
#include "exit.h"

 // Trim Function for input (trims \r and \n from input)
void trim_Input(char *str){
    int length = strlen(str);
    while (length > 0 && (str[length - 1] == '\n' || str[length - 1] == '\r')){
        str[length - 1] = '\0';
        length --;
    }
}


void comhand(void)
{
    // Penguin ASCII image on startup
    sys_req(WRITE, COM1, "\r\n-------------------------------------------------------\r\n", 60);
    sys_req(WRITE, COM1, "\r\n", 3);
    const char *penguinMsg2 =
        "      \033[33m\\|/\033[0m         Welcome to \033[33mMacaroniOS\033[0m!\n"
        "   -=(o ).        \033[36m$(version init)\033[0m\n"
        "      '.-.\\\n"
        "      /|  \\\\      CS450: Operating Systems Structure\n"
        "     ' |  ||\n"
        "       _\\_):,_    Type '\033[33mhelp\033[0m' or '\033[31mexit\033[0m'.\r\n\r\n";

    sys_req(WRITE, COM1, penguinMsg2, strlen(penguinMsg2));



    // loop through the entire buffer
    for (;;) {
        char buf[100] = {0};

        // Read Input
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        
        // Trim the input (dont have strcspn so use this) (could also use a secondary buffer and strtok)
        buf[nread] = '\0'; // adds null terminator
        trim_Input(buf);    // trims \r\n

        sys_req(WRITE, COM1, "\r\n", 2); // Start new line

        if (strcmp(buf, "exit") == 0) {
           if (exit_command()){
            return;
           }
        }
        else if (strcmp(buf, "help") == 0) {
            help_command();
        }
        else if (buf[0] == '\0') {
            sys_req(WRITE, COM1, "\r", 2);
        }
        else {
            const char *invalidMsg = "Invalid command. Try again, type 'help' for all commands.\r\n\r\n";
            sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));
        }
    }
}
