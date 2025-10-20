/**
 * @file pcb.h
 * @brief Process Control Block queue and stack functions.
 * 
 * Defines the data structure types and functions for operating
 * the PCB queues.
 * - Ready Queue: PCBs waiting to be executed by priority.
 * - Blocked Queue: PCBs waiting on something else to happen before its ready.
 * - Suspended-Ready Queue: PCBs not in the stack but ready.
 * - Suspended-Blocked Queue: PCBs not in the stack and blocked.
 * 
 */

#ifndef PCB_H
#define PCB_H
#include <stdint.h>
#define PCB_NAME_MAX_LEN 16 
#define PCB_STACK_MIN_SIZE 1024

enum process_class{
    CLASS_SYSTEM = 0,
    CLASS_USER = 1
};

enum execution_state{
    STATE_READY = 0,
    STATE_RUNNING = 1,
    STATE_BLOCKED = 2
};

enum dispatch_state{
    DISPATCH_ACTIVE = 0,
    DISPATCH_SUSPENDED = 1
};

struct context{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
} __attribute__((packed));

struct pcb{
    char name[PCB_NAME_MAX_LEN];
    enum process_class process_class;
    int priority; // 0 (highest) to 9
    enum execution_state execution_state;
    enum dispatch_state dispatch_state;
    void *stack; // Dynamically allocated, might manually allocate based on memory management.
    struct context* contextPtr;
    struct pcb* next;
    struct pcb* prev;
};

struct pcb_queue{
    struct pcb* head;
    struct pcb* tail;
};

// Queue initialization   
extern struct pcb_queue ready_queue;
extern struct pcb_queue blocked_queue;
extern struct pcb_queue suspended_ready_queue;
extern struct pcb_queue suspended_blocked_queue;

/**
 * @brief Allocates a new PCB and its stack.
 *
 * Initializes all members to zero and sets stack pointer.
 *
 * @return Pointer to the allocated PCB, or NULL if allocation fails.
 */
struct pcb* pcb_allocate(void);

/**
 * @brief Frees a previously allocated PCB and its stack.
 *
 * @param ptr Pointer to the PCB to free.
 * @return 0 on success, -1 if ptr is NULL.
 */
int pcb_free(struct pcb* ptr);

/**
 * @brief Initializes a PCB with given name, class, and priority.
 *
 * Sets default execution and dispatch states.
 *
 * @param name Name of the process.
 * @param process_class Class of the process (SYSTEM/USER).
 * @param priority Initial priority (0-9).
 * @return Pointer to initialized PCB, or NULL if allocation/setup fails.
 */
struct pcb* pcb_setup(const char* name, int process_class, int priority, void (*function)(void));

/**
 * @brief Finds a PCB by its name in all queues.
 *
 * @param name Name of the process to find.
 * @return Pointer to the PCB if found, NULL otherwise.
 */
struct pcb* pcb_find(const char* name);

/**
 * @brief Insert a PCB into the appropriate queue.
 *
 * Chooses the correct queue based on the PCB's dispatch and execution state,
 * then inserts it. If the target queue is the ready queue, PCBs are inserted
 * by priority (FIFO within same priority). Otherwise, they are appended FIFO
 * at the tail.
 *
 * @param ptr Pointer to the PCB to insert.
 */
void pcb_insert(struct pcb* ptr);

/**
 * @brief Remove a PCB from its current queue.
 *
 * Locates the PCB's queue based on its state, then detaches it by
 * fixing neighboring links. Clears the PCB's next/prev pointers.
 *
 * @param ptr Pointer to the PCB to remove.
 * @return 0 on success, -1 if PCB is NULL or not found in any queue.
 */
int pcb_remove(struct pcb* ptr);

#endif
