#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <stdlib.h>
#include "block.h"
#include "showPCB.h"

///////////////////////////////////////////////////////
//block function


//help message for block 
void block_help(void){
    const char *helpMessage =
        "\r\n\033[33mblock\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33mblock\033[0m <\033[36mname\033[0m>      block the requested process with the given name (1-16 characters)\r\n"
        "  \033[33mblock\033[0m \033[36mhelp\033[0m        prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

//blocks the selected process
void block_pcb(const char* name){
    //check to see if process is already blocked
    struct pcb* p = pcb_find(name);
    if(p->execution_state==STATE_BLOCKED){
        sys_req(WRITE, COM1, "\033[31mProcess has already been blocked.\033[0m\n", 44);
    }

    // Checks if the name is appropriate length
    else if (name == NULL || strlen(name) < 1 || strlen(name) > PCB_NAME_MAX_LEN){
        sys_req(WRITE, COM1, "\033[31mInvalid Name: Given name must be 1-16 characters\033[0m\n", 59);
        block_help();
    }

    // Checks if the process exists
    else if (p == NULL){
        sys_req(WRITE, COM1, "\033[31mCould not find process: use 'show' for list of processes\033[0m\n", 67);
        show_pcb_help();
    }

    //block the selected process
    else{
        pcb_remove(p);
        p->execution_state=STATE_BLOCKED;
        pcb_insert(p);
        sys_req(WRITE, COM1, "\033[36mProcess blocked successfully\033[0m\n", 39);
    }
}

//function for use in comhand
void block_pcb_command(const char* args){
    //validation
    for (int i=0; i<(int)strlen(args); i++){
        if (args[i] == ' '){
            sys_req(WRITE, COM1, "\033[31mError: Please ensure a valid name is given\033[0m\n", 47);
            block_help();
            return;
        }
    }
    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "\033[31mError: Please ensure a name is given\033[0m\n", 47);
        block_help();
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        block_help();
    }

    // Handles blocking process
    else{
            block_pcb(args);
    }
}

///////////////////////////////////////////////////////
//unblock function


//help message for unblock 
void unblock_help(void){
    const char *helpMessage =
        "\r\n\033[33munblock\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33munblock\033[0m <\033[36mname\033[0m>      unblock the requested process with the given name (1-16 characters)\r\n"
        "  \033[33munblock\033[0m \033[36mhelp\033[0m        prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

//unblocks the selected process
void unblock_pcb(const char* name){
    //check to see if process is already unblocked
    struct pcb* p = pcb_find(name);
    if(p->execution_state==STATE_READY){
        sys_req(WRITE, COM1, "\033[31mProcess has already been unblocked.\033[0m\n", 46);
    }

    // Checks if the name is appropriate length
    else if (name == NULL || strlen(name) < 1 || strlen(name) > PCB_NAME_MAX_LEN){
        sys_req(WRITE, COM1, "\033[31mInvalid Name: Given name must be 1-16 characters\033[0m\n", 59);
        unblock_help();
    }

    // Checks if the process exists
    else if (p == NULL){
        sys_req(WRITE, COM1, "\033[31mCould not find process: use 'show' for list of processes\033[0m\n", 67);
        show_pcb_help();
    }

    //unblock the selected process
    else{
        pcb_remove(p);
        p->execution_state=STATE_READY;
        pcb_insert(p);
        sys_req(WRITE, COM1, "\033[36mProcess unblocked successfully\033[0m\n", 41);
    }
}

//function for use in comhand
void unblock_pcb_command(const char* args){
    //validation
    for (int i=0; i<(int)strlen(args); i++){
        if (args[i] == ' '){
            sys_req(WRITE, COM1, "\033[31mError: Please ensure a valid name is given\033[0m\n", 47);
            unblock_help();
            return;
        }
    }
    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "\033[31mError: Please ensure a name is given\033[0m\n", 47);
        unblock_help();
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        unblock_help();
    }

    // Handles unblocking process
    else{
            unblock_pcb(args);
    }
}
