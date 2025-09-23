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
        "\r\nsuspend [<name>|help]\r\n"
        "  suspend <name>    Puts a non-system process in the suspended state, and moves it to the appropriate queue.\r\n"
        "  suspend help      prints this message\r\n\r\n";
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
    }

    int result = suspend_pcb(args);
    if (result == -1) {
        const char *argMsg = "Invalid process/name. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        suspend_help();
    }
    else if (result == -2) {
        const char *argMsg = "Process is a system process and cannot be suspended.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else if (result == 1) {
        const char *argMsg = "The process is already suspended.";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else if (result == 0) {
        const char *argMsg = "The process has been suspended.";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        suspend_help();
    }
}



//////////////////////////////////////////////////////////////////////////////
// resume command

// help message for resume
void resume_help(void) {
    const char *helpMsg =
        "\r\nresume [<name>|help]\r\n"
        "  resume <name>    Puts a process in the active (not suspended) state, and moves it to the appropriate queue.\r\n"
        "  resume help      prints this message\r\n\r\n";
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
    }

    int result = resume_pcb(args);
    if (result == -1) {
        const char *argMsg = "Invalid process/name. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        resume_help();
    }
    else if (result == 1) {
        const char *argMsg = "The process is already active.";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else if (result == 0) {
        const char *argMsg = "The process has been resumed.";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        resume_help();
    }
}
