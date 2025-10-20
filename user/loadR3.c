#include <pcb.h>
#include <processes.h>
#include <loadR3.h>
#include "sys_req.h"
#include "string.h"
#include "ready.h"

void load_help(void){
    char *helpMsg = "\033[33mload\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m|\033[36msuspended\033[0m]\r\n"
        "  \033[33mload\033[0m            loads all 5 premade processes\r\n"
        "  \033[33mload \033[0m<\033[36mname\033[0m>     loads a specific process, name is like 'proc1'\r\n"
        "  \033[33mload \033[36msuspended  \033[0mloads all 5 processes as suspended\r\n"
        "  \033[33mload \033[36mhelp \033[0m      prints this message\r\n";
        sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

void loadR3(void){
    struct pcb* newProc = pcb_setup("proc1", CLASS_USER, 1, proc1);
    pcb_insert(newProc);
    newProc = pcb_setup("proc2", CLASS_USER, 2, proc2);
    pcb_insert(newProc);
    newProc = pcb_setup("proc3", CLASS_USER, 3, proc3);
    pcb_insert(newProc);
    newProc = pcb_setup("proc4", CLASS_USER, 4, proc4);
    pcb_insert(newProc);
    newProc = pcb_setup("proc5", CLASS_USER, 5, proc5);
    pcb_insert(newProc);
}

void loadR3_suspended(void){
    loadR3();
    suspend_pcb("proc1");
    suspend_pcb("proc2");
    suspend_pcb("proc3");
    suspend_pcb("proc4");
    suspend_pcb("proc5");
}

void loadProcess(const char* name){
    if (strcmp(name, "proc1")==0){
        struct pcb* newProc = pcb_setup("proc1", CLASS_USER, 1, proc1);
        pcb_insert(newProc);
    }
    else if (strcmp(name, "proc2")==0){
        struct pcb* newProc = pcb_setup("proc2", CLASS_USER, 1, proc2);
        pcb_insert(newProc);
    }
    else if (strcmp(name, "proc2")==0){
        struct pcb* newProc = pcb_setup("proc3", CLASS_USER, 1, proc3);
        pcb_insert(newProc);
    }
    else if (strcmp(name, "proc2")==0){
        struct pcb* newProc = pcb_setup("proc4", CLASS_USER, 1, proc4);
        pcb_insert(newProc);
    }
    else if (strcmp(name, "proc2")==0){
        struct pcb* newProc = pcb_setup("proc5", CLASS_USER, 1, proc5);
        pcb_insert(newProc);
    }
    else{
        char *errorMsg = "\033[31mError: Please enter a valid process name, like 'proc1'\033[0m\r\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        load_help();
    }
}

void load_command(const char* args){
    if (args==NULL || *args == '\0'){
        loadR3();
    }
    else if (strncmp(args, "suspended", 9)==0){
        loadR3_suspended();
    }
    else if (strncmp(args, "help", 4)==0){
        load_help();
    }
    else{
        loadProcess(args);
    }
}
