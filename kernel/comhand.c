#include <comhand.h>
#include <string.h>
#include <sys_req.h>

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
    const char *penguinArt =
    "     / /       \n"
    "    .--.       \n"
    "   |o_o |      \n"
    "   |:_/ |      \n"
    "  //   \\ \\   \n"
    " (|     | )    \n"
    " /'\\_   _/`\\ \n"
    " \\___)=(___/  \r\n";
    sys_req(WRITE, COM1, penguinArt, strlen(penguinArt));


    for (;;) {
        char buf[100] = {0};

        // Read Input
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        
        // Trim the input (dont have strcspn so use this) (could also use a secondary buffer and strtok)
        buf[nread] = '\0'; // adds null terminator
        trim_Input(buf);    // trims \r\n

       
       
        /* the following simply echoes the input */
        //sys_req(WRITE, COM1, buf, nread);
        sys_req(WRITE, COM1, "\r\n", 2); // Start new line

        // Exit Command
        if (strcmp(buf, "exit") == 0) {
            const char *confirmationMsg = "\r\nExit? (Y/n): \r\n";
            sys_req(WRITE, COM1, confirmationMsg, strlen(confirmationMsg));

        while(1){
                char confirmation[2] = {0}; // size 1 doesn't work for enter and null terminator
            
                // Read y or n
                int nreadExit = sys_req(READ, COM1, confirmation, sizeof(confirmation));
                confirmation[nreadExit] = '\0'; // add null terminator
                trim_Input(confirmation); // trim \r\n

                // if y or n, then exit or return
                if (confirmation[0] == 'y' || confirmation[0] == 'Y'){
                    const char *exitMsg = "\r\nExiting...\r\n";
                    sys_req(WRITE, COM1, exitMsg, strlen(exitMsg));
                    return;
                } else if (confirmation[0] == 'n' || confirmation[0] == 'N'){
                    const char *returnMsg = "\r\nReturning...\r\n";
                    sys_req(WRITE, COM1, returnMsg, strlen(returnMsg));
                    break;
                } else {
                    const char *invalidMsg = "\r\nInvalid input. Please type 'Y' or 'n'.\r\n";
                    sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));

                }
            }
        }
        
        // Help Command
        else if (strcmp(buf, "help") == 0) {
            const char *helpMsg = 
            "List Of Commands:\r\n"
            "help - list commands\r\n"
            "exit - exit the program\r\n"
            "version - Show OS version\r\n"
            "\r\n";
            sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
        }
        // Version Command
      
        // New Line on Enter
        else if (strcmp(buf, "\0") == 0){
            sys_req(WRITE, COM1, "\r", 2); // Start new line
        }

        // Unknown Input
        else {
            const char *invalidMsg = "Invalid command. Try again, type 'help' for all commands.\r\n\r\n";
            sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));
        }

    }
}
