#include <sys_req.h>
#include <string.h>

#include "pcb.h"



//////////////////////////////////////////////////////////////////////////////
// suspend command

// help message for suspend
void suspend_help(void) {
    const char *helpMsg =
        "\r\nsuspend [name|help]\r\n"
        "  suspend name    Puts a process in the suspended state, and moves it the appropriate queue.\r\n"
        "  suspend help    prints this message\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

// Puts a process in the suspended state, and moves it the appropriate queue.
int suspend_pcb(const char* process_name) {
    // check for invalid name or process
    if (!process_name || strlen(process_name) == 0) return -1;
    struct pcb* found_pcb_ptr = pcb_find(process_name);
    if (!found_pcb_ptr) return -1;

    // check if already suspended
    if (found_pcb_ptr->dispatch_state == DISPATCH_SUSPENDED) return 1;

    // put the process into the correct queue
    if (pcb_remove(found_pcb_ptr) != 0) return 1;
    found_pcb_ptr->dispatch_state = DISPATCH_SUSPENDED;
    pcb_insert(found_pcb_ptr);

    return 0;
}

// argument handler for resume
void resume_command(const char *args) {
    if (args == NULL || *args == '\0') {
        version_latest();
    }
    else if (strcmp(args, "all") == 0) {
        version_latest();
        version_history();
    }
    else if (strcmp(args, "help") == 0) {
        version_help();
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
}



//////////////////////////////////////////////////////////////////////////////
// resume command

// help message for resume
void resume_help(void) {
    const char *helpMsg =
        "\r\nresume [name|help]\r\n"
        "  resume name    Puts a process in the active (not suspended) state, and moves it the appropriate queue.\r\n"
        "  resume help    prints this message\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

// Puts a process in the active (not suspended) state, and moves it the appropriate queue.
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
    if (args == NULL || *args == '\0') {
        version_latest();
    }
    else if (strcmp(args, "all") == 0) {
        version_latest();
        version_history();
    }
    else if (strcmp(args, "help") == 0) {
        version_help();
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
}
