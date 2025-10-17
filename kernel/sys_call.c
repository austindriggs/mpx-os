#include "comhand.h"
#include <string.h>
#include <sys_req.h>
#include "help.h"
#include "exit.h"
#include "version.h"
#include "clock.h"
#include "showPCB.h"
#include "setPriority.h"
#include "ready.h"
#include "init.h"
#include "block.h"
#include "yield.h"

static struct pcb *curProc = NULL;
static struct context *sysStackPtr = NULL;

struct context* sys_call(struct context *curContext){
    if (sysStackPtr == NULL){
        sysStackPtr = curContext;

    }
    struct pcb *newProc = ready_queue.head;
    if (curContext->eax == IDLE){
        if (newProc == NULL){
            curContext->eax = 0;
            return curContext;
        }
        else {
            if (curProc == NULL){
                pcb_remove(newProc);
                curProc = newProc;
                curContext->eax = 0;
                return newProc->stack_ptr;
            }
            else{
                curProc->stack_ptr = curContext;
                pcb_insert(curProc);
                pcb_remove(newProc);
                curProc = newProc;
                curContext->eax = 0;
                return newProc->stack_ptr;
            }
        }
    }
    else if (curContext->eax == EXIT){
        if (newProc == NULL){
            pcb_free(curProc);
            curProc = NULL;
            sysStackPtr->eax = 0;
            return sysStackPtr;
        }
        else {
            pcb_free(curProc);
            pcb_remove(newProc);
            curProc = newProc;
            curContext = curProc->stack_ptr;
            curContext->eax = 0;
            return newProc->stack_ptr;
        }
    }
    else {
        curContext->eax = -1;
        return curContext;
    }
}
