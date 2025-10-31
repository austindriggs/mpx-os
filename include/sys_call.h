#ifndef SYS_CALL_H
#define SYS_CALL_H
#include <pcb.h>
#include <sys_req.h>

/**
 * @file sys_call.h
 * @brief Handles context switching when sys_req function is used
 * @param curContext Pointer to the current context
 */
struct context *sys_call(struct context *curContext);

/**
 * @brief Helper that returns the current process
 */
struct pcb* sys_get_current_process(void);

#endif
