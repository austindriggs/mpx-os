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
        sys_req(WRITE, COM1, "\033[31mInvalid Process: Given process does not exist\033[0m\n", 56);
    }

    // Checks if the process class
    else if (pcbPTR->process_class == CLASS_SYSTEM){
        sys_req(WRITE, COM1, "\033[31mInvalid Process: Given process is a kernel/system level process\033[0m\n", 74);
    }

    // Validates priority is possible
    else if (newPriority>9 || newPriority<0){
        sys_req(WRITE, COM1, "\033[31mInvalid Priority: Priority level must be between 0 (Highest) and 9 (Lowest)\033[0m\n", 86);
    }

    // Sets priority
    else if (newPriority<=9 && newPriority>=0){
        struct pcb* prevPTR = pcbPTR->prev;
        struct pcb* nextPTR = pcbPTR->next;
        if (prevPTR != NULL && prevPTR->priority > newPriority){
            while (prevPTR != NULL && prevPTR->priority > newPriority){

                // Update from initial previous to initial next
                prevPTR->next = nextPTR;

                // Update from intial next to initial prev
                if (nextPTR != NULL){
                    nextPTR->prev = prevPTR;
                }

                // Update prev to prev of initial previous
                pcbPTR->prev = prevPTR->prev;

                // Update next of previous of initial previous to pcbPTR
                if (prevPTR->prev != NULL){
                    prevPTR->prev->next = pcbPTR;
                }

                // Update next to be initial previous
                if (pcbPTR->next != NULL){
                    pcbPTR->next = prevPTR;
                }

                // Update prev of initial previous to pcbPTR
                prevPTR->prev = pcbPTR;

                // Change what initial previous and initial next are
                prevPTR = pcbPTR->prev;

                if (pcbPTR->next != NULL){
                    nextPTR = pcbPTR->next;
                }
            }
        }
        else if (nextPTR != NULL && nextPTR->priority <= newPriority && pcbPTR->priority != newPriority){
            while (nextPTR != NULL && nextPTR->priority <= newPriority){
                // Update from initial previous to initial next
                prevPTR->next = nextPTR;

                // Update from initial next to initial prev
                nextPTR->prev = prevPTR;

                // Update next from initial next to next next
                pcbPTR->next = nextPTR->next;

                 // Update prev of next next to pcbPTR
                if (nextPTR->next != NULL){
                    nextPTR->next->prev = pcbPTR;
                }
                
                // Update next next to pcbPTR
                nextPTR->next = pcbPTR;

                // Update pcbPTR prev to initial next
                pcbPTR->prev = nextPTR;

                prevPTR = pcbPTR->prev;
                nextPTR = pcbPTR->next;
            }
        }
        pcbPTR->priority = newPriority;
    }
}

/**
 * Help message for setting priority
 */
void set_priority_help(void){
    const char *helpMessage =
        "\r\n\033[33mpriority set\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m] [<\033[36mpriority\033[0m>]\r\n"
        "  \033[33mpriority set\033[0m <\033[36mname\033[0m> <\033[36mpriority\033[0m>    sets the priority of a process to an integer priority value\r\n"
        "  \033[33mpriority set\033[0m \033[36mhelp\033[0m                 prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

/**
 * Creates function for use in command handler, for setting priority and getting help
 */
void set_priority_command(const char* args){
    
    // Checks for arguments
    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "\033[31mError: Please ensure a name and priority are given\033[0m\n", 61);
        set_priority_help();
    }

    // Checks if argument is help
    else if (strcmp(args, "help")==0){
        set_priority_help();
    }

    // Handles getting process name and priority number
    else{
        int isNumber = 1;
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
            for (int i = 0; i<(int)strlen(numArg)-1; i++){
                if ((numArg[i] <= '0' || numArg[i] >= '9') && (numArg[i]!='\0' && numArg[i]!=' ')){
                    isNumber = 0;
                    break;
                }
            }
            if (isNumber == 1){
                int priority = atoi(numArg);
                setPriority(name, priority);
            }
            else {
                sys_req(WRITE, COM1, "\033[31mError: Please ensure the priority argument is a number\033[0m\n", 64);
                set_priority_help();
            }
        }
        else{
            sys_req(WRITE, COM1, "\033[31mError: Please ensure a name and priority are given\033[0m\n", 61);
            set_priority_help();
        }
    }
}
