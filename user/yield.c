#include "sys_req.h"
#include <string.h>

void yield(void){
    sys_req(IDLE);
}

void yield_help(void){
    const char *helpMsg = "\r\n\033[33myield\033[0m [\033[36mhelp\033[0m]\r\n"
        "  \033[33myield\033[0m         yields the currently running process\r\n"
        "  \033[33myield \033[36mhelp    \033[0mprints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

void yield_command(const char* args){
    if (args == NULL || *args=='\0'){
        yield();
    }
    else if (strcmp(args, "help")==0){
        yield_help();
    }
    else{

    }
}
