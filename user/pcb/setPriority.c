#include <pcb.h>
#include <sys_req.h>
#include <string.h>

void setPriority(char* name, int newPriority){
    struct pcb* pcbPTR = pcb_find(name);
    if (pcbPTR == NULL){
        sys_req(WRITE, COM1, "Invalid Process: Given process does not exist\n", 46);
    }
    else if (pcbPTR->process_class == CLASS_SYSTEM){
        sys_req(WRITE, COM1, "Invalid Process: Given process is a kernel/system level process\n", 64);
    }
    else if (newPriority>9 || newPriority<0){
        sys_req(WRITE, COM1, "Invalid Priority: Priority level must be between 0 (Highest) and 9 (Lowest)\n", 76);
    }
    else if (newPriority<=9 && newPriority>=0){
        pcbPTR->priority = newPriority;
    }
}
