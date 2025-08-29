#include <sys_req.h>
#include <string.h>
#include "comhand.h"
#include "exit.h"

int exit_command(void) {
    const char *confirmationMsg = "\r\nExit? (Y/n): \r\n";
    sys_req(WRITE, COM1, confirmationMsg, strlen(confirmationMsg));

    while (1) {
        char confirmation[2] = {0};
        int nreadExit = sys_req(READ, COM1, confirmation, sizeof(confirmation));
        confirmation[nreadExit] = '\0';

        if (confirmation[0] == 'y' || confirmation[0] == 'Y') {
            const char *exitMsg = "\r\nExiting...\r\n";
            sys_req(WRITE, COM1, exitMsg, strlen(exitMsg));
            return 1; // tell comhand to exit
        } else if (confirmation[0] == 'n' || confirmation[0] == 'N') {
            const char *returnMsg = "\r\nReturning...\r\n";
            sys_req(WRITE, COM1, returnMsg, strlen(returnMsg));
            return 0;
        } else {
            const char *invalidMsg = "\r\nInvalid input. Please type 'Y' or 'n'.\r\n";
            sys_req(WRITE, COM1, invalidMsg, strlen(invalidMsg));
        }
    }
}
