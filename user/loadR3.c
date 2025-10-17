#include <pcb.h>
#include <processes.h>
#include <loadR3.h>
#include "sys_req.h"
#include "string.h"

void loadR3(void){
    struct pcb* newProc = pcb_setup("proc1", CLASS_USER, 1, proc1);
    pcb_insert(newProc);
    newProc = pcb_setup("proc2", CLASS_USER, 2, proc2);
    pcb_insert(newProc);
    newProc = pcb_setup("proc3", CLASS_USER, 3, proc3);
    pcb_insert(newProc);
    newProc = pcb_setup("proc4", CLASS_USER, 4, proc4);
    pcb_insert(newProc);
    newProc = pcb_setup("proc5", CLASS_USER, 5, proc5);
    pcb_insert(newProc);
}

void loadR3_suspended(void){

}

/*
void loadProcess(char* name){

}
*/

void load_command(const char* args){
    if (args==NULL || *args == '\0'){
        //sys_req(WRITE, COM1, "In Load\n", 8);
        loadR3();
    }
    else if (strncmp(args, "suspended", 9)==0){
        sys_req(WRITE, COM1, "In Load\n", 8);
        loadR3_suspended();
    }
    else{
        loadR3();
    }
}
