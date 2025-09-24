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
            "\r\nshow [<name>|ready|blocked|all|help]\r\n"
            "  show <name>     prints details for the named process\r\n"
            "  show ready      prints details of processes in ready queue\r\n"
            "  show blocked    prints details of processes in blocked queue\r\n"
            "  show suspended  prints details of processes in the suspended queues\r\n"
            "  show all        prints details of all processes\r\n"
            "  show help       prints this message\r\n"
            "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

/**
 * Takes process name (string) and prints details for the process
 */
void showPCB(const char* name){
    char buffer[20];
    struct pcb* pcbPTR = pcb_find(name);\

    // Checks if the process exists
    if (pcbPTR == NULL){
        sys_req(WRITE, COM1, "Invalid Process: Given process does not exist\n", 46);
    }
    else {

        // Print name
        sys_req(WRITE, COM1, "Name: ", 6);
        sys_req(WRITE, COM1, pcbPTR->name, strlen(pcbPTR->name));
        sys_req(WRITE, COM1, "\n", 1);

        // Print Class
        sys_req(WRITE, COM1, "Class: ", 7);
        if (pcbPTR->process_class == CLASS_SYSTEM){
            sys_req(WRITE, COM1, "System/Kernel\n", 14);
        }
        else{
            sys_req(WRITE, COM1, "User\n", 12);
        }

        // Print state
        sys_req(WRITE, COM1, "State: ", 7);
        if (pcbPTR->execution_state == STATE_READY){
            sys_req(WRITE, COM1, "Ready\n", 6);
        }
        else if (pcbPTR->execution_state == STATE_RUNNING){
            sys_req(WRITE, COM1, "Running\n", 8);
        }
        else {
            sys_req(WRITE, COM1, "Blocked\n", 8);
        }

        // Print suspension state
        sys_req(WRITE, COM1, "Suspension Status: ", 19);
        if (pcbPTR->dispatch_state == DISPATCH_ACTIVE){
            sys_req(WRITE, COM1, "Not suspended\n", 14);
        }
        else {
            sys_req(WRITE, COM1, "Suspended\n", 10);
        }

        // Print priority
        sys_req(WRITE, COM1, "Priority: ", 10);
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
        sys_req(WRITE, COM1, "Ready Processes:\n", 17);
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
        sys_req(WRITE, COM1, "Blocked Processes:\n", 19);
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
        sys_req(WRITE, COM1, "Suspended Ready Processes:\n", 27);
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
        sys_req(WRITE, COM1, "Suspended Blocked Processes:\n", 29);
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
