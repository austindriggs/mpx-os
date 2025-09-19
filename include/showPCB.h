#ifndef SHOWPCB_H
#define SHOWPCB_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <comhand.h>
#include <itoa.h>

void show_command(const char* args);

void showPCB(const char* name);

void showReady(void);

void showBlocked(void);

void showAllPCB(void);

void show_pcb_help(void);

#endif
