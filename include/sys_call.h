#ifndef SYS_CALL_H
#define SYS_CALL_H
#include <pcb.h>
#include <sys_req.h>

struct context *sys_call(struct context *curContext);

#endif
