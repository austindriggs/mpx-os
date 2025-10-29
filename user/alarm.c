#include "clock.h"
#include <string.h>
#include <sys_req.h>
#include <stdint.h>
#include <mpx/interrupts.h>
#include <mpx/io.h>
#include "stdlib.h"
#include "itoa.h"
#include "alarm.h"
#include "clock.h"

void alarm(const int hours, const int minutes, const int seconds, const char* message){

    sys_req(WRITE, COM1, "\033[31mError: Please ensure a name, class, and priority are given\033[0m\n", 69);



}

void alarm_command(const char* args){
    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "\033[31mError: Please ensure a name, class, and priority are given\033[0m\n", 69);
        alarm_help();
        return;
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        alarm_help();
        return;
    }
    char time_str[9]; //to hold time substring of args
    const char* time_ptr = strncpy(time_str, args, 8);
    const char* message = args + 9;  // skip "HH:MM:SS "
    
    // Expected format: HH:MM:SS
    if (strlen(time_ptr) != 8 || time_ptr[2] != ':' || time_ptr[5] != ':') {
        const char *err = "\033[31mInvalid format. Use set time <HH:MM:SS>\033[0m\r\n";
        sys_req(WRITE, COM1, err, strlen(err));
        alarm_help();
        return;
    }

    char buf[3];
        buf[2] = '\0';

        buf[0] = time_ptr[0]; buf[1] = time_ptr[1];
        int hour = atoi(buf);

        buf[0] = time_ptr[3]; buf[1] = time_ptr[4];
        int minute = atoi(buf);

        buf[0] = time_ptr[6]; buf[1] = time_ptr[7];
        int second = atoi(buf);

        if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
            const char *err = "\033[31mInvalid time values.\033[0m\r\n";
            sys_req(WRITE, COM1, err, strlen(err));
            alarm_help();
            return;
        }
        alarm(hour, minute, second, message);
}

void alarm_help(void){
    const char *helpMsg =
        "\r\n\033[33malarm\033[0m <\033[36mhelp|time\033[0m> <\033[36mmessage\033[0m>   set alarm at given time to display desired message.   \r\n"
        "  \033[33mtime\033[0m                        desired time in form HH:MM:SS\r\n"
        "  \033[33m---\033[0m                        Hours must be 0-23\r\n"
        "  \033[33m---\033[0m                        Minutes must be 0-59\r\n"
        "  \033[33m---\033[0m                        Seconds must be 0-59\r\n"
        "  \033[33mmessage\033[0m                         desired message.\r\n"
        "  \033[33mhelp\033[0m                         prints this message.\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

