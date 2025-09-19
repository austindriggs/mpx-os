#ifndef SETPRIORITY_H
#define SETPRIORITY_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>

void set_priority_command(const char* args);

void set_priority_help(void);

void setPriority(char* name, int newPriority);


#endif
