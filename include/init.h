#ifndef INIT_H
#define INIT_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>

void create_pcb_command(const char* args);

void create_help(void);

void create_pcb(const char* name, int process_class, int priority);

void delete_pcb_command(const char* args);

void delete_help(void);

void delete_pcb(const char* name);

#endif
