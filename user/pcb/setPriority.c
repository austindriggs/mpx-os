#include <pcb.h>
#include <sys_req.h>
#include <string.h>

void setPriority(char* name, int newPriority){
    struct pcb* pcbPTR = pcb_find(name);
    if (pcbPTR == NULL){
        sys_req(WRITE, COM1, "Invalid Process: Given process does not exist", 45);
    }
    else if (pcbPTR->process_class == CLASS_SYSTEM){
        sys_req(WRITE, COM1, "Invalid Process: Given process is a kernel/system level process", 63);
    }
    else if (newPriority>9 || newPriority<0){
        sys_req(WRITE, COM1, "Invalid Priority: Priority level must be between 0 (Highest) and 9 (Lowest)", 75);
    }
    else if (newPriority<=9 && newPriority>=0){
        pcbPTR->priority = newPriority;
    }
}