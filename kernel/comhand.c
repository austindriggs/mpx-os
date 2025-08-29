#include "comhand.h"
#include <string.h>
#include <sys_req.h>
#include "help.h"
#include "exit.h"
#include "version.h"

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
    // Startup Message
    const char *welcomeMsg = "\r\nWelcome to MacaroniOS";
    const char *introMsg = "\r\nType help to see a list of commands, Type exit to shutdown!\r\n\r\n";
    sys_req(WRITE, COM1, welcomeMsg, strlen(welcomeMsg));
    sys_req(WRITE, COM1, introMsg, strlen(introMsg));


    // Penguin ASCII Image for console (placeholder)
    

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
        else if (strcmp(buf, "version") == 0) {
            print_version();
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
