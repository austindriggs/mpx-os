#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <stdlib.h>

/**
 * Function for setting priority. Takes a string (name) and priority (integer)
 */
void setPriority(char* name, int newPriority){
    struct pcb* pcbPTR = pcb_find(name);

    // Checks if the process exists
    if (pcbPTR == NULL){
        sys_req(WRITE, COM1, "Invalid Process: Given process does not exist\n", 46);
    }

    // Checks if the process class
    else if (pcbPTR->process_class == CLASS_SYSTEM){
        sys_req(WRITE, COM1, "Invalid Process: Given process is a kernel/system level process\n", 64);
    }

    // Validates priority is possible
    else if (newPriority>9 || newPriority<0){
        sys_req(WRITE, COM1, "Invalid Priority: Priority level must be between 0 (Highest) and 9 (Lowest)\n", 76);
    }

    // Sets priority
    else if (newPriority<=9 && newPriority>=0){
        pcbPTR->priority = newPriority;
    }
}

/**
 * Help message for setting priority
 */
void set_priority_help(void){
    const char *helpMessage =
            "\r\npriority set [<name>|help] [<priority>]\r\n"
            "  priority set <name> <priority>    sets the priority of a process to an integer priority value\r\n"
            "  priority help                     prints this message\r\n"
            "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

/**
 * Creates function for use in command handler, for setting priority and getting help
 */
void set_priority_command(const char* args){
    
    // Checks for arguments
    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "Error: Please ensure a name and priority are given\n", 51);
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        set_priority_help();
    }

    // Handles getting process name and priority number
    else{
        int temp = 0;
        for (int i=0; i<(int)strlen(args); i++){
            if (args[i] == ' ' && i<(int)strlen(args) && i != 0){
                temp = i;
                break;
            }
        }
        if (temp != 0){
            char name[temp + 1]; 
            for (int i=0; i<temp; i++){
                name[i] = args[i];
            }
            name[temp] = '\0'; // Added in null terminator
            const char *numArg = args + temp;
            int priority = atoi(numArg);
            setPriority(name, priority);
        }
        else{
            sys_req(WRITE, COM1, "Error: Please ensure a name and priority are given\n", 51);
            set_priority_help();
        }
    }
}
