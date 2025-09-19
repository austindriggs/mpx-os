#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <comhand.h>
#include <itoa.h>

void show_pcb_help(void){
    const char *helpMessage = 
            "\r\nshow [name|ready|blocked|all|help]\r\n"
            "  show name        prints details for the named process\r\n"
            "  show ready       prints details of processes in ready queue\r\n"
            "  show blocked     prints details of processes in blocked queue\r\n"
            "  show all         prints details of all processes\r\n"
            "  show help        prints this message\r\n"
            "\r\n";
    sys_req(WRITE, COM1, helpMessage, strlen(helpMessage));
}

void showPCB(const char* name){
    char buffer[20];
    struct pcb* pcbPTR = pcb_find(name);
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

void showReady(void){
    // Move through ready queue
    // Call showPCB and print each PCB in queue
    struct pcb* nextPtr = ready_queue.head;
    if (nextPtr == NULL){
        sys_req(WRITE, COM1, "No processes in the ready queue\n", 32);
    }
    else{
        sys_req(WRITE, COM1, "Ready Processes:\n", 17);
        while(nextPtr){
            showPCB(nextPtr->name);
            nextPtr = nextPtr->next;
        }
    }
}

void showBlocked(void){
    // Same as showReady, but blocked queue
    struct pcb* nextPtr = blocked_queue.head;
    if (nextPtr == NULL){
        sys_req(WRITE, COM1, "No processes in the blocked queue\n", 34);
    }
    else{
        sys_req(WRITE, COM1, "Blocked Processes:\n", 19);
        while(nextPtr){
            showPCB(nextPtr->name);
            nextPtr = nextPtr->next;
        }
    }
}

void showAllPCB(void){
    showReady();
    showBlocked();
}

void show_command(const char* args){
    if (args == NULL || *args=='\0' || (strcmp(args, "all")==0)){
        showAllPCB();
    }
    else if (strcmp(args, "ready") == 0){
        showReady();
    }
    else if (strcmp(args, "blocked") == 0){
        showBlocked();
    }
    else if (strcmp(args, "help")==0){
        show_pcb_help();
    }
    else{
        showPCB(args);
    }
}
