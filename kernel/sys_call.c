#include "sys_call.h"
#include "pcb.h"
#include "string.h"
#include "sys_req.h"

struct pcb *curProc = NULL;
struct context *sysStackPtr = NULL;

struct context *sys_call(struct context *curContext){
    sysStackPtr = curContext;
    if (curContext->eax == IDLE){

    }
    else if (curContext->eax == EXIT){

    }
    else {
        
    }
}
