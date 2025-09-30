#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include "showPCB.h"

///////////////////////////////////////////////////////
//create function


//help message for create 
void create_help(void){
    const char *helpMessage =
        "\r\n\033[33mcreate\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m] [<\033[36mclass\033[0m>] [<\033[36mpriority\033[0m>]\r\n"
        "  \033[33mcreate\033[0m <\033[36mname\033[0m> <\033[36mclass\033[0m> <\033[36mpriority\033[0m>    create a new process with the given:\r\n" 
        "  \033   nname (1-16 characters)\r\n" 
        "  \033   cclass (0=system or 1=user)\r\n" 
        "  \033   ppriority (0=highest to 9=lowest)\r\n"
        "  \033[33mcreate\033[0m \033[36mhelp\033[0m    prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

//creates a new pcb and inserts it into the queue
void create_pcb(const char* name, int process_class, int priority){
    struct pcb* p = pcb_find(name);

    // Checks if the process exists
    if (p != NULL){
        sys_req(WRITE, COM1, "\033[31mInvalid Name: Given process already exists\033[0m\n", 56);
    }

    // Checks if the name is appropriate length
    else if (name == NULL || strlen(name) < 1 || strlen(name) > PCB_NAME_MAX_LEN){
        sys_req(WRITE, COM1, "\033[31mInvalid Name: Given name must be 1-16 characters\033[0m\n", 59);
        create_help();
    }

    // Checks if the process class is valid
    else if (process_class != CLASS_SYSTEM && process_class != CLASS_USER){
        sys_req(WRITE, COM1, "\033[31mInvalid Process: Given class must be 0 (system) or 1 (user)\033[0m\n", 70);
        create_help();
    }

    // Validates priority 
    else if (priority>9 || priority<0){
        sys_req(WRITE, COM1, "\033[31mInvalid Priority: Priority level must be between 0 (Highest) and 9 (Lowest)\033[0m\n", 86);
        create_help();
    }

    else {
        struct pcb* newProcess = pcb_setup(name, process_class, priority);
        pcb_insert(newProcess);
    }
    
}

//function for use in comhand
void create_pcb_command(const char* args){

    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "\033[31mError: Please ensure a name, class, and priority are given\033[0m\n", 69);
        create_help();
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        create_help();
    }

    // Handles getting process name, class, and priority number
    else{
        // get a substring for the process name from the args
        int temp = 0;
        for (int i=0; i<(int)strlen(args); i++){
            if (args[i] == ' ' && i<(int)strlen(args) && i != 0){
                temp = i;
                break;
            }
        }
        //parse the substring above into name
        if (temp != 0){
            char name[temp + 1]; 
            for (int i=0; i<temp; i++){
                name[i] = args[i];
            }
            name[temp] = '\0'; // Added in null terminator
            //parse process class
            char class[2];
            const char *numArg = args + temp +1;
            strncpy(class, numArg, 1);
            class[1]='\0';
            int process_class = atoi(class);
            //parse process priority
            numArg+=2;
            int priority = atoi(numArg);
            create_pcb(name, process_class, priority);
            showAllPCB();
        }
        else{
            sys_req(WRITE, COM1, "\033[31mError: Please ensure a name, class, and priority are given\033[0m\n", 69);
            create_help();
        }
    }
}


///////////////////////////////////////////////////////////
//delete function



//help message for delete 
void delete_help(void){
    const char *helpMessage =
        "\r\n\033[33mdelete\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33mdelete\033[0m <\033[36mname\033[0m>    deletes the requested user process with the given name (1-16 characters)\r\n"
        "  \033[33mdelete\033[0m \033[36mhelp\033[0m      prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}


//Finds the requested process and removes it from the queue
void delete_pcb(const char* name){
    struct pcb* proc_to_delete  = pcb_find(name);

    // Checks if the process exists
    if (proc_to_delete  == NULL){
        sys_req(WRITE, COM1, "\033[31mInvalid Name: Given process does not exist\033[0m\n", 53);
    }

    // Checks if the name is appropriate length
    else if (name == NULL || strlen(name) < 1 || strlen(name) > PCB_NAME_MAX_LEN){
        sys_req(WRITE, COM1, "\033[31mInvalid Name: Given name must be 1-16 characters\033[0m\n", 59);
        delete_help();
    }

    // Checks if the process class
    else if (proc_to_delete ->process_class == CLASS_SYSTEM){
        sys_req(WRITE, COM1, "\033[31mInvalid Process: Given process is a kernel/system level process\033[0m\n", 74);
    }

    else{
        pcb_remove(proc_to_delete );
        pcb_free(proc_to_delete );
    }
}

//function for use in comhand
void delete_pcb_command(const char* args){

    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "\033[31mError: Please ensure a name is given\033[0m\n", 47);
        delete_help();
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        delete_help();
    }

    // Handles getting process name
    else{
        delete_pcb(args);
        showAllPCB();
    }
}
