#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include "comhand.h"

void show_command(const char* args){
    if (args == NULL || args == '\0'){
        showAllPCB();
    }
    else if (strcmp(args, "ready") == 0){
        showReady();
    }
    else if (strcmp(args, "blocked") == 0){
        showBlocked();
    }
    else {
        showPCB(args);
    }
}

void showPCB(char* name){
    if (/*No PCB with that name*/0){
        sys_req(WRITE, COM1, "Invalid Process: Given process does not exist", 45);
    }
    else {

    }
}

void showReady(){
    // Move through ready queue
    // Call showPCB and print each PCB in queue
}

void showBlocked(){
    // Same as showReady, but blocked queue
}

void showAllPCB(){
    // Call both showReady and showBlocked
}