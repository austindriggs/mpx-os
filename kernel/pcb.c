#include "pcb.h"
#include <string.h>     
#include <sys_req.h>   
#include <memory.h>

/**
 * TODO
 * Will add in error messaging at different points
 */

// Queue Initialization
struct pcb_queue ready_queue = { NULL, NULL };
struct pcb_queue blocked_queue = { NULL, NULL };
struct pcb_queue suspended_ready_queue = { NULL, NULL };
struct pcb_queue suspended_blocked_queue = { NULL, NULL };

// PCB Allocate
struct pcb* pcb_allocate(void){
    struct pcb* ptr =(struct pcb*) sys_alloc_mem(sizeof(struct pcb));
    if (!ptr) return NULL;
    memset(ptr, 0, sizeof(struct pcb)); // initialize to 0
    ptr->stack = (char*) sys_alloc_mem(PCB_STACK_SIZE);
    
    // if allocation fails
    if(!ptr->stack){
        sys_free_mem(ptr);
        return NULL;
    }
    
    memset(ptr->stack, 0, PCB_STACK_SIZE);
    ptr->stack_ptr = ptr->stack + PCB_STACK_SIZE - sizeof(void*); // Downward stack movement
    ptr->next = NULL;
    ptr->prev = NULL;

    return ptr;
}

// PCB Free
int pcb_free(struct pcb* ptr){
    if (!ptr) return -1;
    if (ptr->stack) sys_free_mem(ptr->stack);
    sys_free_mem(ptr);
    return 0;
}

// PCB Setup
struct pcb* pcb_setup(const char* name, int process_class, int priority){
    if (!name || strlen(name) >= PCB_NAME_LEN) return NULL;
    if (priority < 0 || priority > 9) return NULL; // Also handle in User Functions
    struct pcb* ptr = pcb_allocate();
    if (!ptr) return NULL;
    strncpy(ptr->name, name, PCB_NAME_LEN - 1);
    ptr->name[PCB_NAME_LEN - 1] = '\0';
    ptr->process_class = process_class;
    ptr->priority = priority;
    ptr->execution_state = STATE_READY;
    ptr->dispatch_state = DISPATCH_ACTIVE;
    return ptr;
}

// PCB Find
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

// PCB Insert (PLEASE REVIEW AND DOUBLE CHECK)
void pcb_insert(struct pcb* ptr){
    if (!ptr) return;
    struct pcb_queue* q_ptr = NULL;
    if (ptr->dispatch_state == DISPATCH_SUSPENDED){
        if (ptr->execution_state == STATE_BLOCKED){
            q_ptr = &suspended_blocked_queue;
        } else {
            q_ptr = &suspended_ready_queue;
        }
    } else {
        if (ptr->execution_state == STATE_BLOCKED){
            q_ptr = &blocked_queue;
        } else {
            q_ptr = &ready_queue;
        }
    }

    // Insert
    if (q_ptr == &ready_queue){
        //FIFO within same priority
        struct pcb* cur = q_ptr->head;
        while (cur && cur->priority <= ptr->priority){
            cur = cur->next;
        }
        if (!cur){
            // insert at tail
            if (!q_ptr->head) {
                q_ptr->head = q_ptr->tail = ptr;
            } else {
                q_ptr->tail->next = ptr;
                ptr->prev = q_ptr->tail;
                q_ptr->tail = ptr;
            }
        } else {
            // insert before cur
            ptr->next = cur;
            ptr->prev = cur->prev;
            if (cur->prev) cur->prev->next = ptr;
            else q_ptr->head = ptr;
            cur->prev = ptr;
        }
    } else {
        // FIFO insert at tail
        if (!q_ptr->head){
            q_ptr->head = q_ptr->tail = ptr;
        } else {
            q_ptr->tail->next = ptr;
            ptr->prev = q_ptr->tail;
            q_ptr->tail = ptr;
        }
    }
}

// PCB Remove (PELASE REVIEW AND DOUBLE CHECK)
int pcb_remove(struct pcb* ptr){
    if (!ptr) return -1;
    struct pcb_queue* q_ptr = NULL;
    if (ptr->dispatch_state == DISPATCH_SUSPENDED){
        if (ptr->execution_state == STATE_BLOCKED) q_ptr = &suspended_blocked_queue;
        else q_ptr = &suspended_ready_queue;
    } else {
        if (ptr->execution_state == STATE_BLOCKED) q_ptr = &blocked_queue;
        else q_ptr = &ready_queue;
    }
    if (!q_ptr || !q_ptr->head) return -1;
    if (ptr->prev) ptr->prev->next = ptr->next;
    else q_ptr->head = ptr->next;
    if (ptr->next) ptr->next->prev = ptr->prev;
    else q_ptr->tail = ptr->prev;
    ptr->next = ptr->prev = NULL;
    return 0;
}
