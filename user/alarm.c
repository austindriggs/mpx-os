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
#include <pcb.h>
#include <processes.h>
#include <loadR3.h>
#include "sys_req.h"
#include "sys_call.h"
#include "memory.h"


void alarm(void) {
    struct pcb* self = sys_get_current_process();
    //error checking
    if (!self) {
        // Shouldn't happen when properly dispatched, but guard anyway
        sys_req(EXIT);
        return;
    }
    AlarmData* data = (AlarmData*) self->args;

    while (1) {
        rtc_time_t current_time;
        get_time(&current_time);  
        
        if ((current_time.hour > data->hour) ||
            (current_time.hour == data->hour && current_time.minute > data->minute) ||
            (current_time.hour == data->hour && current_time.minute == data->minute && current_time.second >= data->second)) {
            sys_req(WRITE, COM1, data->message, strlen(data->message));
            sys_req(WRITE, COM1, "\r\n", 2);
            // free data before exiting so it doesn't leak or become dangling
            sys_free_mem(data);
            self->args = NULL; // avoid dangling pointer
            break;
        }
        sys_req(IDLE);
    }
    sys_req(EXIT);
}

void alarm_create(AlarmData* data){
    // Allocate memory for alarm data
    AlarmData* newData = (AlarmData*) sys_alloc_mem(sizeof(AlarmData));
    if (!newData) {
        sys_req(WRITE, COM1, "\033[31mError: Memory allocation failed.\033[0m\r\n", 53);
        return;
    }

    // Copy fields
    newData->hour = data->hour;
    newData->minute = data->minute;
    newData->second = data->second;
    strncpy(newData->message, data->message, sizeof(newData->message) - 1);
    newData->message[sizeof(newData->message) - 1] = '\0';

    // Generate unique name
    static int alarm_counter = 1;
    char name[16];
    my_strcpy(name, "alarm");
    char numbuf[8];
    itoa(alarm_counter++, numbuf);
    my_strcat(name, numbuf);

    // Create and queue PCB
    struct pcb* newProc = pcb_setup(name, CLASS_USER, 1, alarm);
    //error checking
    if (!newProc) {
        sys_free_mem(data);
        const char *err = "\033[31mError: Failed to setup alarm process.\033[0m\r\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }
    newProc->args = newData;
    pcb_insert(newProc);

    char* mes = "\033[32mAlarm set successfully.\033[0m\r\n";

    sys_req(WRITE, COM1, mes, strlen(mes));
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
    

    //error test message
    if (strlen(message) > 100 || message == NULL || *message =='\0') {
        const char *err = "\033[31mMust include valid message\033[0m\r\n";
        sys_req(WRITE, COM1, err, strlen(err));
        alarm_help();
        return;
    }
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
    AlarmData* data = (AlarmData*)sys_alloc_mem(sizeof(AlarmData));
    data->hour=hour;
    data->minute=minute;
    data->second=second;
    strncpy(data->message, message, sizeof(data->message) - 1);
    data->message[sizeof(data->message) - 1] = '\0';//ensure null termination
    alarm_create(data);
}

void alarm_help(void){
    const char *helpMsg =
        "\r\n\033[33malarm\033[0m <\033[36mhelp|time\033[0m> <\033[36mmessage\033[0m>   set alarm at given time to display desired message.   \r\n"
        "  \033[33mtime\033[0m                        desired time in form HH:MM:SS\r\n"
        "  \033[33m---\033[0m                        Hours must be 0-23\r\n"
        "  \033[33m---\033[0m                        Minutes must be 0-59\r\n"
        "  \033[33m---\033[0m                        Seconds must be 0-59\r\n"
        "  \033[33mmessage\033[0m                         desired message (must be under 100 chars).\r\n"
        "  \033[33mhelp\033[0m                         prints this message.\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}
