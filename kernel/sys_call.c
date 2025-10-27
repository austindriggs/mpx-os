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
    if (curProc == NULL){
        sysStackPtr = curContext;
    }
    struct pcb *newProc = ready_queue.head;
    
    if (curContext->eax == IDLE){
        if (newProc == NULL){
            curContext->eax = -1;
            return sysStackPtr; // Changed to sysStackPtr from curContext so it returns to kmain for shutdown, instead of constantly looping.
        }
        else {
            if (curProc == NULL){
                pcb_remove(newProc);
                curProc = newProc;
                curContext->eax = -1;
                return newProc->contextPtr;
            }
            else{
                curProc->contextPtr = curContext;
                pcb_insert(curProc);
                pcb_remove(newProc);
                curProc = newProc;
                curContext->eax = -1;
                return newProc->contextPtr;
            }
        }
    }
    else if (curContext->eax == EXIT){
        if (newProc == NULL){
            pcb_free(curProc);
            curProc = NULL;
            sysStackPtr->eax = -1;
            return sysStackPtr;
        }
        else {
            pcb_free(curProc);
            pcb_remove(newProc);
            curProc = newProc;
            curContext = curProc->contextPtr;
            curContext->eax = -1;
            return newProc->contextPtr;
        }
    }
    else {
        curContext->eax = -1;
        return curContext;
    }
}
