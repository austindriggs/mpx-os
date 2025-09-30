#ifndef BLOCK_H
#define BLOCK_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>

void block_pcb_command(const char* args);

void block_help(void);

void block_pcb(const char* name);

void unblock_pcb_command(const char* args);

void unblock_help(void);

void unblock_pcb(const char* name);

#endif
