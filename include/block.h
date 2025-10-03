#ifndef BLOCK_H
#define BLOCK_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>



 /**
 * @brief Function for handling command input for blocking PCB, called by comhand
 * @param args Arguments for handling block command
 */
void block_pcb_command(const char* args);


/**
 * @brief Provides help for how to use the block command
 */
void block_help(void);


/**
 * @brief Block the desired PCB
 * @param name Argument stores the name of desired PCB
 */
void block_pcb(const char* name);


/**
 * @brief Function for handling command input for unblocking PCB, called by comhand
 * @param args Arguments for handling unblock command
 */
void unblock_pcb_command(const char* args);


/**
 * @brief Provides help for how to use the unblock command
 */
void unblock_help(void);


/**
 * @brief Unblock the desired PCB
 * @param name Argument stores the name of desired PCB
 */
void unblock_pcb(const char* name);

#endif
