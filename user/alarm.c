#include "clock.h"
#include <string.h>
#include <sys_req.h>
#include <stdint.h>
#include <mpx/interrupts.h>
#include <mpx/io.h>
#include "stdlib.h"
#include "itoa.h"
#include "alarm.h"

void alarm(const char* time, const char* message){




}

void alarm_command(const char* args){

}

void alarm_help(void){
    const char *helpMsg =
        "\r\n\033[33malarm\033[0m <\033[36mtime\033[0m> <\033[36mmessage\033[0m>   set alarm at given time to display desired message.   \r\n"
        "  \033[33mtime\033[0m                        use format HH:MM:SS\r\n"
        "  \033[33mmessage\033[0m                         message.\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

