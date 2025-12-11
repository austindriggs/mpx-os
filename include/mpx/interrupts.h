#ifndef MPX_INTERRUPTS_H
#define MPX_INTERRUPTS_H

#include <stdint.h>
#include <stddef.h>
#include <sys_req.h>
#include <stdlib.h>

#include <mpx/device.h>   

#define RING_BUFFER_SIZE 100

/**
 * @file mpx/interrupts.h
 * @brief Kernel functions related to software and hardware interrupts
 */

/** Disable interrupts */
#define cli() __asm__ volatile ("cli")

/** Enable interrupts */
#define sti() __asm__ volatile ("sti")

/* Core interrupt setup */
void irq_init(void);
void pic_init(void);
void idt_init(void);
void idt_install(int vector, void (*handler)(void *));

/* ---------- Device / I/O state enums ---------- */

typedef enum {
    DEV_CLOSED = 0,
    DEV_IDLE,
    DEV_READING,
    DEV_WRITING
} dev_state_t;

typedef enum {
    open   = 1,
    closed = 0
} open_status;


typedef enum {
    allocated   = 1,
    unallocated = 0
} allocation_status;

typedef enum {
    no_event        = 0,
    unhandled_event = 1
} event_flag;

typedef enum {
    IO_READ = 0,
    IO_WRITE
} io_op_t;

/* Forward declarations */
struct pcb;
struct dcb;

/**
 * @struct iocb
 * @brief I/O Control Block for one read or write request.
 */
typedef struct iocb {
    struct pcb *proc;      
    struct dcb *dev;       
    io_op_t     op;         

    char       *buf;        
    size_t      size;      
    size_t      byte_count; 

    struct iocb *next;   
} iocb_t;

/**
 * @struct dcb
 * @brief Device Control Block for a serial port.
 */
typedef struct dcb {
    device            device;           
    dev_state_t       state;             
    open_status       open_status;      
    allocation_status allocation_status; 
    event_flag        event_flag;        

    /* Ring buffer for inputs when no read is active */
    char   ring_buffer[RING_BUFFER_SIZE];
    size_t ring_head;
    size_t ring_tail;
    size_t ring_count;

    /* I/O queue for this device */
    iocb_t *q_head;
    iocb_t *q_tail;
    iocb_t *current;         

    uint16_t base_port; 
} dcb_t;


/**
 * @brief Get the DCB for a given serial device.
 */
dcb_t *get_dcb_for_device(device dev);

/**
 * @brief Open a serial device for interrupt-driven I/O.
 *
 * Error codes follow the R6 notes:
 *  -100 invalid device
 *  -101 invalid event flag pointer (not used here, but reserved)
 *  -102 invalid baud (no divisor)
 *  -103 already open
 */
int serial_open(device dev, int speed);

/**
 * @brief Close a serial device.
 *
 * Error codes (from notes):
 *  -201 device not open
 */
int serial_close(device dev);

/**
 * @brief Begin an interrupt-driven read on dev.
 *
 * This does NOT block. It:
 *  - assumes dcb->current points to the IOCB for this op
 *  - initializes state/event_flag
 *  - copies any pre-typed chars from the ring buffer
 *  - returns 0 on success, or negative error.
 */
int serial_read(device dev, char *buf, size_t len);

/**
 * @brief Begin an interrupt-driven write on dev.
 *
 * This does NOT block. It:
 *  - assumes dcb->current points to the IOCB for this op
 *  - writes first byte to THR
 *  - enables THR empty interrupts
 *  - returns 0 on success, or negative error.
 */
int serial_write(device dev, char *buf, size_t len);

/**
 * @brief Serial ISR
 */
void serial_interrupt(void);

#endif
