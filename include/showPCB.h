#ifndef showPCB_H
#define showPCB_H

#include "pcb.h"
#include "sys_req.h"
#include "string.h"
#include "comhand.h"

void show_command(const char* args);

void showPCB(char* name);

void showReady();

void showBlocked();

void showAllPCB();

#endif
