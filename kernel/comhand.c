#include <comhand.h>
#include <string.h>
#include <sys_req.h>

 // Trim Function for input (trims \r and \n from input)
    void trimInput(char *str){
        int length = strlen(str);
        while (length > 0 && (str[length - 1] == '\n' || str[length - 1] == '\r')){
            str[length - 1] = '\0';
            length --;
        }
    }


void comhand(void)
{
    // Startup Message
    const char *welcomeMsg = "\r\nWelcome to MacaroniPenguins OS";
    const char *introMsg = "\r\nType help to see a list of commands, Type exit to shutdown!\r\n";
    sys_req(WRITE, COM1, welcomeMsg, strlen(welcomeMsg));
    sys_req(WRITE, COM1, introMsg, strlen(introMsg));

    // Penguin ASCII Image for console (placeholder)


    for (;;) {
        char buf[100] = {0};

        //prompt user for input**
        sys_req(WRITE, COM1, "( ')> ", 6);
        
        // Read Input
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        
        // Trim the input (dont have strcspn so use this) (could also use a secondary buffer and strtok)
        buf[nread] = '\0'; // adds null terminator
        trimInput(buf);    // trims \r\n

       
       
        /* the following simply echoes the input */
        sys_req(WRITE, COM1, buf, nread);
        sys_req(WRITE, COM1, "\r\n", 2); // Start new line

        // Exit Command
        if (strcmp(buf, "exit") == 0) {
            const char *confirmationMsg = "\r\nExit? (y/n): \r\n";
            sys_req(WRITE, COM1, confirmationMsg, strlen(confirmationMsg));

        while(1){
                char confirmation[3] = {0}; // size 1 doesn't work for enter and null terminator
            
                // Read y or n
                int nreadExit = sys_req(READ, COM1, confirmation, sizeof(confirmation));
                confirmation[nreadExit] = '\0'; // add null terminator
                trimInput(confirmation); // trim \r\n

                // if y or n, then exit or return
                if (confirmation[0] == 'y' || confirmation[0] == 'Y'){
                    const char *exitMsg = "Exiting...\r\n";
                    sys_req(WRITE, COM1, exitMsg, strlen(exitMsg));
                    return;
                } else if (confirmation[0] == 'n' || confirmation[0] == 'N'){
                    const char *returnMsg = "Returning...\r\n";
                    sys_req(WRITE, COM1, returnMsg, strlen(returnMsg));
                    break;
                } else {
                    const char *invalidMsg = "Invalid input. Please type 'y' or 'n'.\r\n";
                    sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));

                }
            }
        }
        
        // Help Command
        else if (strcmp(buf, "help") == 0) {
            const char *helpMsg = 
            "\r\nList Of Commands:\r\n"
            "help - list commands\r\n"
            "exit - exit the program\r\n"
            "version - Show OS version\r\n"
            "\r\n";
            sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
        }
        // Version Command

        // Unknown Input
        else {
            const char *invalidMsg = "Invalid command. Try again, type 'help' for all commands.\r\n";
            sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));
        }

    }
}
