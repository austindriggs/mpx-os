#include "comhand.h"
#include <string.h>
#include <itoa.h>
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
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <memory.h>   
#include <mpx/device.h>
#include <pcb.h>

/* Pointer for current process */
static struct pcb *curProc = NULL;

/* Pointer to system stack/kernel context */
static struct context *sysStackPtr = NULL;

/* Helper to return current process */
struct pcb *sys_get_current_process(void)
{
    return curProc;
}

/*
 * Scan all serial DCBs. For any with event_flag == unhandled_event:
 *  - take the IOCB from dcb->current
 *  - move the process from BLOCKED to READY
 *  - set its saved eax = #bytes transferred
 *  - free the IOCB
 *  - start the next IOCB in the queue, if any
 */
static void io_handle_completions(void)
{
    device devs[] = { COM1, COM2, COM3, COM4 };

    for (int i = 0; i < 4; ++i) {
        dcb_t *dcb = get_dcb_for_device(devs[i]);
        if (!dcb || dcb->open_status != open)
            continue;

        if (dcb->event_flag == unhandled_event && dcb->current != NULL) {
            iocb_t      *io = dcb->current;
            struct pcb  *p  = io->proc;

            /* Wake the blocked process */
            if (p != NULL) {
                pcb_remove(p);                  // remove from blocked queue
                p->execution_state = STATE_READY;
                pcb_insert(p);                  // insert into ready queue


                if (p->contextPtr) {
                    p->contextPtr->eax = (int)io->byte_count;
                }
            }

            /* Clear DCB */
            dcb->current    = NULL;
            dcb->state      = DEV_IDLE;
            dcb->event_flag = no_event;

            /* Grab next IOCB, if any */
            iocb_t *next = NULL;
            if (dcb->q_head) {
                next       = dcb->q_head;
                dcb->q_head = next->next;
                if (!dcb->q_head)
                    dcb->q_tail = NULL;
                next->next = NULL;
            }

            /* Free finished IOCB */
            sys_free_mem(io);

            /* Start next request, if present */
            if (next) {
                dcb->current    = next;
                dcb->state      = (next->op == IO_READ) ? DEV_READING : DEV_WRITING;
                dcb->event_flag = no_event;

                if (next->op == IO_READ) {
                    serial_read(dcb->device, next->buf, next->size);
                } else {
                    serial_write(dcb->device, next->buf, next->size);
                }
            }
        }
    }
}

/*
 * io_scheduler:
 *  - Builds an IOCB for the given request.
 *  - If device idle, starts I/O immediately (serial_read/serial_write).
 *  - If busy, enqueues the IOCB.
 *
 * Return values:
 *  < 0  : error (immediate; caller does not block)
 *  > 0  : I/O completed synchronously, return that byte count
 *  == 0 : I/O started and will complete asynchronously; caller should BLOCK.
 */
static int io_schedule(struct context *ctx)
{
    op_code op  = (op_code)ctx->eax;
    device dev  = (device)ctx->ebx;
    char  *buf  = (char *)ctx->ecx;
    size_t len  = (size_t)ctx->edx;

    dcb_t *dcb = get_dcb_for_device(dev);
    if (!dcb || dcb->open_status != open) {
        return (op == READ) ? -301 : -401;
    }

    if (!buf || len == 0) {
        return (op == READ) ? -302 : -402;
    }

    /* Allocate IOCB */
    iocb_t *io = (iocb_t *)sys_alloc_mem(sizeof(iocb_t));
    if (!io) {
        return -1;  // out of memory
    }

    memset(io, 0, sizeof(iocb_t));
    io->proc       = curProc;
    io->dev        = dcb;
    io->buf        = buf;
    io->size       = len;
    io->byte_count = 0;
    io->next       = NULL;

    if (op == READ) {
        io->op = IO_READ;
    } else if (op == WRITE) {
        io->op = IO_WRITE;
    } else {
        sys_free_mem(io);
        return -1;
    }

    /* If device idle, start immediately; else enqueue */
    if (dcb->current == NULL && dcb->state == DEV_IDLE) {
        dcb->current    = io;
        dcb->event_flag = no_event;
        dcb->state      = (io->op == IO_READ) ? DEV_READING : DEV_WRITING;

        int rc;
        if (io->op == IO_READ)
            rc = serial_read(dcb->device, buf, len);
        else
            rc = serial_write(dcb->device, buf, len);

        if (rc < 0) {
            dcb->current = NULL;
            dcb->state   = DEV_IDLE;
            sys_free_mem(io);
            return rc;
        }

        /* If completed immediately */
        if (dcb->event_flag == unhandled_event) {
            int done = (int)io->byte_count;
            sys_free_mem(io);
            dcb->current    = NULL;
            dcb->state      = DEV_IDLE;
            dcb->event_flag = no_event;
            return done;
        }

        /* Otherwise, I/O is in progress; caller must block. */
    } else {
        /* Device busy: append to queue */
        if (dcb->q_tail)
            dcb->q_tail->next = io;
        else
            dcb->q_head = io;
        dcb->q_tail = io;
    }

    return 0;   // started, will complete asynchronously
}

struct context *sys_call(struct context *curContext)
{
    /* Save kernel context */
    if (curProc == NULL) {
        sysStackPtr = curContext;
    }

    /* First: handle any completed I/O */
    io_handle_completions();

    /* Next ready process */
    struct pcb *newProc = ready_queue.head;

    /* IDLE: */
    if (curContext->eax == IDLE) {
        if (newProc == NULL) {
            /* No other process ready; just return to current process */
            if (curProc != NULL) {
                curContext->eax = -1;
                return curContext;  // back to same process
            } else {
                /* No processes at all; return to kernel */
                curContext->eax = -1;
                return curContext;
            }
        } else {
            if (curProc == NULL) {
                pcb_remove(newProc);
                curProc          = newProc;
                curContext->eax  = -1;
                return newProc->contextPtr;
            } else {
                curProc->contextPtr = curContext;
                pcb_insert(curProc);
                pcb_remove(newProc);
                curProc          = newProc;
                curContext->eax  = -1;
                return newProc->contextPtr;
            }
        }
    }

    /* EXIT: */
    else if (curContext->eax == EXIT) {
        if (newProc == NULL) {
            pcb_free(curProc);
            curProc          = NULL;
            sysStackPtr->eax = -1;
            return sysStackPtr;
        } else {
            pcb_free(curProc);
            pcb_remove(newProc);
            curProc    = newProc;
            curContext = curProc->contextPtr;
            curContext->eax = -1;
            return newProc->contextPtr;
        }
    }

    /* READ or WRITE */
    else if (curContext->eax == READ || curContext->eax == WRITE) {
        int rc = io_schedule(curContext);

        /* Immediate error: return to caller with error code */
        if (rc < 0) {
            curContext->eax = rc;
            return curContext;
        }

        /* Synchronous completion: rc is byte count */
        if (rc > 0) {
            curContext->eax = rc;
            return curContext;
        }

        /*
         * Asynchronous I/O started:
         *  - Mark current process BLOCKED
         *  - Save its context
         *  - Put it in blocked queue
         *  - Dispatch next READY process or return to kernel
         */
        if (curProc != NULL) {

            /* Remove from whatever queue it is currently in */
            pcb_remove(curProc);

            /* Mark blocked */
            curProc->execution_state = STATE_BLOCKED;
            curProc->contextPtr      = curContext;

            /* Insert into blocked queue properly */
            pcb_insert(curProc);
        }


        newProc = ready_queue.head;

        if (newProc == NULL) {
            // No other process ready then go idle 
            return curContext;
        } 
        else {
            pcb_remove(newProc);
            curProc = newProc;
            return newProc->contextPtr;
        }
    }

    /* Any other opcode */
    else {
        curContext->eax = -1;
        return curContext;
    }
}
