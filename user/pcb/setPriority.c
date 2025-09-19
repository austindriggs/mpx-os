#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <stdlib.h>

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

void set_priority_command(const char* args){
    if (args == NULL || *args =='\0'){
        sys_req(WRITE, COM1, "Error: Please ensure a name and priority are given\n", 51);
    }
    else{
        int temp = 0;
        for (int i=0; i<(int)strlen(args); i++){
            if (args[i] == ' ' && i<(int)strlen(args) && i != 0){
                temp = i;
                break;
            }
        }
        if (temp != 0){
            char name[temp];
            for (int i=0; i<temp; i++){
                name[i] = args[i];
            }
            const char *numArg = args + temp;
            int priority = atoi(numArg);
            setPriority(name, priority);
        }
        else{
            sys_req(WRITE, COM1, "Error: Please ensure a name and priority are given\n", 51);
        }
    }
}
