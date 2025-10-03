#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <comhand.h>
#include <itoa.h>

/**
 * Help message for showing pcbs
 */
void show_pcb_help(void){
    const char *helpMessage = 
        "\r\n\033[33mshow\033[0m [<\033[36mname\033[0m>|\033[36mready\033[0m|\033[36mblocked\033[0m|\033[36msuspended\033[0m|\033[36mall\033[0m|\033[36mhelp\033[0m]\r\n"
        "  \033[33mshow\033[0m <\033[36mname\033[0m>     prints details for the named process\r\n"
        "  \033[33mshow\033[0m \033[36mready\033[0m      prints details of processes in ready queue\r\n"
        "  \033[33mshow\033[0m \033[36mblocked\033[0m    prints details of processes in blocked queue\r\n"
        "  \033[33mshow\033[0m \033[36msuspended\033[0m  prints details of processes in the suspended queues\r\n"
        "  \033[33mshow\033[0m \033[36mall\033[0m        prints details of all processes\r\n"
        "  \033[33mshow\033[0m \033[36mhelp\033[0m       prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

/**
 * Takes process name (string) and prints details for the process
 */
void showPCB(const char* name){
    char buffer[20];
    struct pcb* pcbPTR = pcb_find(name);

    // Checks if the process exists
    if (pcbPTR == NULL){
        sys_req(WRITE, COM1, "\033[31mInvalid Process: Given process does not exist\033[0m\n", 56);
        show_pcb_help();
    }
    else {

        // Print name
        sys_req(WRITE, COM1, "Name: ", strlen("Name: "));
        sys_req(WRITE, COM1, pcbPTR->name, strlen(pcbPTR->name));
        sys_req(WRITE, COM1, "\n", 1);

        // Print Class
        sys_req(WRITE, COM1, "Class: ", strlen("Class: "));
        if (pcbPTR->process_class == CLASS_SYSTEM){
            sys_req(WRITE, COM1, "System/Kernel\n", strlen("System/Kernel\n"));
        }
        else{
            sys_req(WRITE, COM1, "User\n", strlen("User\n"));
        }

        // Print state
        sys_req(WRITE, COM1, "State: ", strlen("State: "));
        if (pcbPTR->execution_state == STATE_READY){
            sys_req(WRITE, COM1, "Ready\n", strlen("Ready\n"));
        }
        else if (pcbPTR->execution_state == STATE_RUNNING){
            sys_req(WRITE, COM1, "Running\n", strlen("Running\n"));
        }
        else {
            sys_req(WRITE, COM1, "Blocked\n", strlen("Blocked\n"));
        }

        // Print suspension state
        sys_req(WRITE, COM1, "Suspension Status: ", strlen("Suspension Status: "));
        if (pcbPTR->dispatch_state == DISPATCH_ACTIVE){
            sys_req(WRITE, COM1, "Not suspended\n", strlen("Not suspended\n"));
        }
        else {
            sys_req(WRITE, COM1, "Suspended\n", strlen("Suspended\n"));
        }

        // Print priority
        sys_req(WRITE, COM1, "Priority: ", strlen("Priority: "));
        itoa(pcbPTR->priority, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, "\n\n", 2);
    }
}

/**
 * Prints all processes in the ready queue
 */
void showReady(void){
    struct pcb* nextPtr = ready_queue.head;
    
    // Checks if there are any processes in the ready queue
    if (nextPtr == NULL){
        sys_req(WRITE, COM1, "No processes in the ready queue\n", 32);
    }

    // If there are processes, move through queue and print each
    else{
        sys_req(WRITE, COM1, "\033[32mReady Processes:\033[0m\n", 27); 
        while(nextPtr){
            showPCB(nextPtr->name);
            nextPtr = nextPtr->next;
        }
    }
}

/**
 * Prints all processes in the blocked queue
 */
void showBlocked(void){
    struct pcb* nextPtr = blocked_queue.head;

    // Checks if there are any processes in the blocked queue
    if (nextPtr == NULL){
        sys_req(WRITE, COM1, "No processes in the blocked queue\n", 34);
    }

    // If there are processes, move through queue and print each
    else{
        sys_req(WRITE, COM1, "\033[31mBlocked Processes:\033[0m\n\r", 29); 
        while(nextPtr){
            showPCB(nextPtr->name);
            nextPtr = nextPtr->next;
        }
    }
}

void showSuspended(void){
    struct pcb* nextPtr = suspended_ready_queue.head;

    // Checks if there are suspended-ready processes
    if (nextPtr == NULL){
        sys_req(WRITE, COM1, "No processes in the suspended-ready queue\n", 42);
    }
    else{
        sys_req(WRITE, COM1, "\033[33mSuspended Ready Processes:\033[0m\n\r", 37); 
        while(nextPtr){
            showPCB(nextPtr->name);
            nextPtr = nextPtr->next;
        }
    }

    nextPtr = suspended_blocked_queue.head;

    if (nextPtr == NULL){
        sys_req(WRITE, COM1, "No processes in the suspended-blocked queue\n", 44);
    }
    else{
        sys_req(WRITE, COM1, "\033[33mSuspended Blocked Processes:\033[0m\n\r", 39);
        while(nextPtr){
            showPCB(nextPtr->name);
            nextPtr = nextPtr->next;
        }
    }
}

/**
 * Prints all processes
 */
void showAllPCB(void){
    showReady();
    showBlocked();
    showSuspended();
}


/**
 * Function for use in command handler. Handles arguments for showing pcbs
 */
void show_command(const char* args){
    
    // Checks if there are no arguments, or if argument is all
    if (args == NULL || *args=='\0' || (strcmp(args, "all")==0)){
        showAllPCB();
    }

    // Checks if the argument is ready
    else if (strncmp(args, "ready", 5) == 0){
        showReady();
    }

    // Checks if the argument is blocked
    else if (strncmp(args, "blocked", 7) == 0){
        showBlocked();
    }

    // Checks if the argument is help
    else if (strncmp(args, "help", 4)==0){
        show_pcb_help();
    }

    else if (strncmp(args, "suspended", 9)==0){
        showSuspended();
    }

    // Handles any other input
    else{
        showPCB(args);
    }
}
