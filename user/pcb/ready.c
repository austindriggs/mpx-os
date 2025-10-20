#include <sys_req.h>
#include <string.h>

#include "pcb.h"

// Because the commands "suspend" and "resume" share so much code, they were
// put into the same file for convenience, called "ready".



//////////////////////////////////////////////////////////////////////////////
// suspend command

// help message for suspend
void suspend_help(void) {
    const char *helpMsg =
        "\r\n\033[33msuspend\033[0m [<\033[36mname\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33msuspend\033[0m <\033[36mname\033[0m>    Puts a non-system process in the suspended state, and moves it to the appropriate queue.\r\n"
        "  \033[33msuspend\033[0m \033[36mhelp\033[0m      prints this message\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

// Puts a non-system process in the suspended state, and moves it to the appropriate queue.
int suspend_pcb(const char* process_name) {
    // check for invalid name or process
    if (!process_name || strlen(process_name) == 0) return -1;
    struct pcb* found_pcb_ptr = pcb_find(process_name);
    if (!found_pcb_ptr) return -1;

    // check if system process
    if (found_pcb_ptr->process_class == CLASS_SYSTEM) return -2;

    // check if already suspended
    if (found_pcb_ptr->dispatch_state == DISPATCH_SUSPENDED) return 1;

    // put the process into the correct queue
    if (pcb_remove(found_pcb_ptr) != 0) return 1;
    found_pcb_ptr->dispatch_state = DISPATCH_SUSPENDED;
    pcb_insert(found_pcb_ptr);

    return 0;
}

// argument handler for suspend
void suspend_command(const char *args) {
    if (args == NULL || *args == '\0' || strcmp(args, "help") == 0) {
        suspend_help();
        return;
    }

    int result = suspend_pcb(args);
    if (result == -1) {
        const char *argMsg = "\033[31mInvalid process/name. Please try again.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        suspend_help();
    }
    else if (result == -2) {
        const char *argMsg = "\033[31mProcess is a system process and cannot be suspended.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else if (result == 1) {
        const char *argMsg = "\033[31mThe process is already suspended.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else if (result == 0) {
        const char *argMsg = "\033[36mThe process has been suspended.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else {
        const char *argMsg = "\033[31mInvalid argument. Please try again.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }

    return;
}



//////////////////////////////////////////////////////////////////////////////
// resume command

// help message for resume
void resume_help(void) {
    const char *helpMsg =
        "\r\n\033[33mresume\033[0m [<\033[36mname\033[0m>|\033[36mall\033[0m]|\033[36mhelp\033[0m]\r\n"
        "  \033[33mresume\033[0m <\033[36mname\033[0m>    Puts a process in the active (not suspended) state, and moves it to the appropriate queue.\r\n"
        "  \033[33mresume\033[0m \033[36mall\033[0m      Puts all suspended processes in the ready state, and moves it to the appropriate queue.\r\n"
        "  \033[33mresume\033[0m \033[36mhelp\033[0m      prints this message\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

// Puts a process in the active (not suspended) state, and moves it to the appropriate queue.
int resume_pcb(const char* process_name) {
    // check for invalid name or process
    if (!process_name || strlen(process_name) == 0) return -1;
    struct pcb* found_pcb_ptr = pcb_find(process_name);
    if (!found_pcb_ptr) return -1;

    // check if already active
    if (found_pcb_ptr->dispatch_state == DISPATCH_ACTIVE) return 1;

    // put the process into the correct queue
    if (pcb_remove(found_pcb_ptr) != 0) return 1;
    found_pcb_ptr->dispatch_state = DISPATCH_ACTIVE;
    pcb_insert(found_pcb_ptr);

    return 0;
}

// argument handler for resume
void resume_command(const char *args) {
    if (args == NULL || *args == '\0' || strcmp(args, "help") == 0) {
        resume_help();
        return;
    }
    else if (args == NULL || *args == '\0' || strcmp(args, "all") == 0) {
        struct pcb* curPtr = suspended_ready_queue.head;
        struct pcb* tempPtr;
        // Checks if there are any processes in the ready queue
        if (curPtr == NULL){
            sys_req(WRITE, COM1, "No processes in the suspended ready queue\n", 42);
        }

        // If there are processes, move through queue and print each 

        else{
            char* message = "\033[32mResuming all Processes:\033[0m\n";
            sys_req(WRITE, COM1, message, strlen(message)); 
            while(curPtr){
                tempPtr = curPtr->next;
                resume_pcb(curPtr->name);
                curPtr = tempPtr;
            }
        }
        return;
    }


    int result = resume_pcb(args);
    if (result == -1) {
        const char *argMsg = "\033[31mInvalid process/name. Please try again.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        resume_help();
    }
    else if (result == 1) {
        const char *argMsg = "\033[31mThe process is already active.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else if (result == 0) {
        const char *argMsg = "\033[36mThe process has been resumed.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else {
        const char *argMsg = "\033[31mInvalid argument. Please try again.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }

    return;
}
