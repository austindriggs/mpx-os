#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <mpx/vm.h>
#include <mpx/serial.h>
#include <sys_req.h>

struct mcb_list mem_allocated_list = { NULL, NULL };
struct mcb_list mem_free_list = { NULL, NULL };

void initialize_heap(size_t size){
    void *addr = kmalloc(size, 0, NULL);
    if (!addr) return;

    // Place the initial MCB at the start of the returned region.
    struct mcb* initial_mcb = (struct mcb*)addr;

    // Data address starts immediately after the MCB
    initial_mcb->startAddr = (int)((uintptr_t)addr + sizeof(struct mcb));

    // Usable size is the total minus the MCB header (16 bytes)
    initial_mcb->size = (int)(size - sizeof(struct mcb));
    initial_mcb->next = NULL;
    initial_mcb->prev = NULL;

    // Start lists as free
    mem_free_list.head = mem_free_list.tail = initial_mcb;
    mem_allocated_list.head = mem_allocated_list.tail = NULL;
}

static void unlink_mcb(struct mcb_list *list, struct mcb *node){
    if (!list || !node) return;
    if (node->prev) node->prev->next = node->next;
    else list->head = node->next;

    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev;
    // Remove MCB from a list
    node->next = node->prev = NULL;
}

static void append_mcb(struct mcb_list *list, struct mcb *node){
    if (!list || !node) return;
    node->next = NULL;
    node->prev = list->tail;
    if (list->tail) list->tail->next = node;
    else list->head = node;
    // Add MCB to tail of a list
    list->tail = node;
}

void *allocate_memory(size_t req_size){
    if (req_size == 0) return NULL;

    // First Fit - Look for the first block that fits
    struct mcb *cur = mem_free_list.head;
    while (cur && (size_t)cur->size < req_size) cur = cur->next;
    if (!cur) return NULL;

    // If block is large enough, split it
    size_t remaining = (size_t)cur->size - req_size;
    if (remaining > sizeof(struct mcb)){

        // Create MCB for remaining free block
        uintptr_t alloc_data_addr = (uintptr_t)cur->startAddr;
        uintptr_t new_mcb_addr = alloc_data_addr + req_size;
        struct mcb *new_mcb = (struct mcb *)new_mcb_addr;
        
        // Set up the Free MCB
        new_mcb->startAddr = (int)(new_mcb_addr + sizeof(struct mcb));
        new_mcb->size = (int)(remaining - sizeof(struct mcb));

        // Replace cur in free list with the new MCB
        new_mcb->prev = cur->prev;
        new_mcb->next = cur->next;
        if (cur->prev) cur->prev->next = new_mcb;
        else mem_free_list.head = new_mcb;
        if (cur->next) cur->next->prev = new_mcb;
        else mem_free_list.tail = new_mcb;

        // Shrink cur to allocated size
        cur->size = (int)req_size;
        cur->next = cur->prev = NULL;

        // Add cur to allocated list
        append_mcb(&mem_allocated_list, cur);

        // Set pointer to unallocated space
        return (void *) (uintptr_t) cur->startAddr;
    } else {
        // If not enough room then allocate the whole block
        // Remove from free list
        unlink_mcb(&mem_free_list, cur);

        // Add to allocated list
        append_mcb(&mem_allocated_list, cur);

        return (void *) (uintptr_t) cur->startAddr;
    }
}

int free_memory(void *ptr){
    if (!ptr) return -1;

    // Convert user pointer to MCB pointer
    struct mcb *m = (struct mcb *)((uintptr_t)ptr - sizeof(struct mcb));

    // Check if MCB is in allocated list
    struct mcb *cur = mem_allocated_list.head;
    while (cur && cur != m)
        cur = cur->next;

    if (!cur)
        return -1;  // Not found → invalid free()

    // If found remove from allocated list
    unlink_mcb(&mem_allocated_list, m);

    // Insert into free list in order
    struct mcb *f = mem_free_list.head;
    struct mcb *prev = NULL;

    while (f && (uintptr_t)f < (uintptr_t)m) {
        prev = f;
        f = f->next;
    }

    // Insert between two blocks
    m->prev = prev;
    m->next = f;
    if (prev) prev->next = m;
    else mem_free_list.head = m;

    if (f) f->prev = m;
    else mem_free_list.tail = m;

    // Merge with the previous block if adjacent
    if (m->prev) {
        uintptr_t prev_end = (uintptr_t)m->prev->startAddr + m->prev->size;
        uintptr_t m_addr = (uintptr_t)m->startAddr - sizeof(struct mcb);

        if (prev_end == m_addr) {
            m->prev->size += sizeof(struct mcb) + m->size;

            // After merge remove the front block (m) from the list
            if (m->next) m->next->prev = m->prev;
            else mem_free_list.tail = m->prev;

            m->prev->next = m->next;

            // After merge use prev block as the whole merged mcb
            m = m->prev;
        }
    }

    // Merge with the next block if adjacent
    if (m->next) {
        uintptr_t m_end = (uintptr_t)m->startAddr + m->size;
        uintptr_t next_addr = (uintptr_t)m->next;

        if (m_end == next_addr) {
            struct mcb *next = m->next;

            // Merge next into m
            m->size += sizeof(struct mcb) + next->size;

            // After merge remove the next block from the list
            m->next = next->next;
            if (next->next) next->next->prev = m;
            else mem_free_list.tail = m;
        }
    }

    return 0;
}


