#include "pcb.h"
#include <string.h>     
#include <sys_req.h>   
#include <memory.h>
#include <showPCB.h>

/**
 * TODO
 * Will add in error messaging at different points
 */

// Queue Initialization
struct pcb_queue ready_queue = { NULL, NULL };
struct pcb_queue blocked_queue = { NULL, NULL };
struct pcb_queue suspended_ready_queue = { NULL, NULL };
struct pcb_queue suspended_blocked_queue = { NULL, NULL };

struct pcb* pcb_allocate(void){
    struct pcb* ptr =(struct pcb*) sys_alloc_mem(sizeof(struct pcb));
    if (!ptr) return NULL;
    memset(ptr, 0, sizeof(struct pcb)); // initialize to 0
    ptr->stack = sys_alloc_mem(PCB_STACK_MIN_SIZE);
    
    // if allocation fails
    if(!ptr->stack){
        sys_free_mem(ptr);
        return NULL;
    }
    
    memset(ptr->stack, 0, PCB_STACK_MIN_SIZE);
    ptr->contextPtr = (struct context*)((uint8_t *)ptr->stack + PCB_STACK_MIN_SIZE - sizeof(struct context)); // Downward stack movement
    ptr->next = NULL;
    ptr->prev = NULL;

    return ptr;
}

int pcb_free(struct pcb* ptr){
    if (!ptr) return -1;
    if (ptr->stack) sys_free_mem(ptr->stack);
    sys_free_mem(ptr);
    return 0;
}

struct pcb* pcb_setup(const char* name, int process_class, int priority, void (*function)(void)){
    if (!name || strlen(name) >= PCB_NAME_MAX_LEN) return NULL;
    if (priority < 0 || priority > 9) return NULL; // Also handle in User Functions
    if (pcb_find(name)!=NULL){
        char* errorMsg = "\033[31mError: Process with name '";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        sys_req(WRITE, COM1, name, strlen(name));
        errorMsg = "' already exists\033[0m\r\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        return NULL;
    }
    struct pcb* ptr = pcb_allocate();
    if (!ptr) return NULL;
    strncpy(ptr->name, name, PCB_NAME_MAX_LEN - 1);
    ptr->name[PCB_NAME_MAX_LEN - 1] = '\0';
    ptr->process_class = process_class;
    ptr->priority = priority;
    ptr->execution_state = STATE_READY;
    ptr->dispatch_state = DISPATCH_ACTIVE;
    ptr->contextPtr->fs = 0x10;
    ptr->contextPtr->gs = 0x10;
    ptr->contextPtr->ds = 0x10;
    ptr->contextPtr->es = 0x10;
    ptr->contextPtr->cs = 0x08;
    ptr->contextPtr->ebp = 0;
    ptr->contextPtr->esp = (uint32_t)((uint8_t *)ptr->contextPtr + sizeof(struct context));
    ptr->contextPtr->eflags = 0x202;
    ptr->contextPtr->eip = (uint32_t)function;
    return ptr;
}

struct pcb* pcb_find(const char* name){
    if (!name) return NULL;
    struct pcb_queue* queues[] = {
        &ready_queue,
        &blocked_queue,
        &suspended_ready_queue,
        &suspended_blocked_queue
    };
    for (int i = 0; i < 4; i++){
        struct pcb* find_ptr = queues[i]->head;
        while(find_ptr){
            if (strcmp(find_ptr->name, name) == 0){
                return find_ptr;
            }
            find_ptr = find_ptr->next;
        }
    }
    return NULL; // Not found
}

void pcb_insert(struct pcb* ptr) {
    // Safety check
    if (!ptr) return;

    struct pcb_queue* q_ptr = NULL;

    // Select correct queue based on dispatch and execution state
    if (ptr->dispatch_state == DISPATCH_SUSPENDED) {
        if (ptr->execution_state == STATE_BLOCKED) {
            q_ptr = &suspended_blocked_queue;
        } else {
            q_ptr = &suspended_ready_queue;
        }
    } else {
        if (ptr->execution_state == STATE_BLOCKED) {
            q_ptr = &blocked_queue;
        } else {
            q_ptr = &ready_queue;
        }
    }

    // Insert PCB into the chosen queue
    if (q_ptr == &ready_queue) {
        // Priority-ordered insert (FIFO within same priority)
        struct pcb* cur = q_ptr->head;
        while (cur && cur->priority <= ptr->priority) {
            cur = cur->next;
        }

        // if cur is NULL, we reached the end of the list
        if (!cur) {
            // Insert at tail if no higher-priority PCB found
            if (!q_ptr->head) {
                q_ptr->head = q_ptr->tail = ptr;
            } else {
                q_ptr->tail->next = ptr;
                ptr->prev = q_ptr->tail;
                q_ptr->tail = ptr;
            }
        } 
        // else insert before cur
        else {
            ptr->next = cur;
            ptr->prev = cur->prev;
            if (cur->prev) cur->prev->next = ptr;
            else q_ptr->head = ptr;
            cur->prev = ptr;
        }
    } else {
    // For non-ready queues: insert new PCB at the tail (FIFO order).

    // Case 1: The queue is empty (no head yet).
    if (!q_ptr->head) {
        // Make the new PCB both the head and the tail of the queue,
        // since it's the only element.
        q_ptr->head = q_ptr->tail = ptr;
    } else {
        // Case 2: Queue already has at least one PCB.

        // Link the current tail's 'next' pointer to the new PCB.
        q_ptr->tail->next = ptr;

        // Link the new PCB's 'prev' pointer back to the old tail.
        ptr->prev = q_ptr->tail;

        // Update the queue's tail to point to the new PCB.
        // Now 'ptr' becomes the new last element.
        q_ptr->tail = ptr;
    }
}

}

int pcb_remove(struct pcb* ptr) {
    if (!ptr) return -1;

    struct pcb_queue* q_ptr = NULL;

    // Determine which queue the PCB belongs to
    if (ptr->dispatch_state == DISPATCH_SUSPENDED) {
        if (ptr->execution_state == STATE_BLOCKED)
            q_ptr = &suspended_blocked_queue;
        else
            q_ptr = &suspended_ready_queue;
    } else {
        if (ptr->execution_state == STATE_BLOCKED)
            q_ptr = &blocked_queue;
        else
            q_ptr = &ready_queue;
    }

    // Nothing to remove if queue is empty
    if (!q_ptr || !q_ptr->head) return -1;

    // Relink neighbors to bypass 'ptr'
    if (ptr->prev) ptr->prev->next = ptr->next;
    else q_ptr->head = ptr->next;

    if (ptr->next) ptr->next->prev = ptr->prev;
    else q_ptr->tail = ptr->prev;

    // Clear links
    ptr->next = ptr->prev = NULL;

    return 0;
}
