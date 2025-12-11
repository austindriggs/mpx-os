#ifndef MPX_VM_H
#define MPX_VM_H
#include <stddef.h>

/**
 @file mpx/vm.h
 @brief Kernel functions for virtual memory and primitive allocation
*/

/**
 * @brief Memory Control Block (MCB) structure.
 */
struct mcb{
    int startAddr;
    int size;
    struct mcb* next;
    struct mcb* prev;
    // Only needed if doing single list
    // enum alloc_stat alloc_stat;
};

struct mcb_list{
    struct mcb* head;
    struct mcb* tail;
};

extern struct mcb_list mem_allocated_list;
extern struct mcb_list mem_free_list;

/**
 Allocates memory from a primitive heap.
 @param size The size of memory to allocate
 @param align If non-zero, align the allocation to a page boundary
 @param phys_addr If non-NULL, a pointer to a pointer that will
                  hold the physical address of the new memory
 @return The newly allocated memory
 */
void *kmalloc(size_t size, int align, void **phys_addr);

/**
 Initializes the kernel page directory and initial kernel heap area.
 Performs identity mapping of the kernel frames such that the virtual
 addresses are equivalent to the physical addresses.
*/
void vm_init(void);

/**
 * @brief Initializes the heap with a given size.
 * @param size The total size of the heap to initialize.
 */
void initialize_heap(size_t size);

/**
 * @brief Allocates a block of memory of the requested size.
 * @param req_size The size of memory to allocate.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void *allocate_memory(size_t req_size);

/**
 * @brief Frees a previously allocated block of memory.
 * @param ptr Pointer to the memory block to free.
 * @return 0 on success, -1 on failure.
 */
int free_memory(void *ptr);

#endif
