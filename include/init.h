#ifndef INIT_H
#define INIT_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>

 /**
 * @brief Function for handling command input for creating PCB, called by comhand
 * @param args Arguments for handling create command
 */
void create_pcb_command(const char* args);


/**
 * @brief Provides help for how to use the create command
 */
void create_help(void);


/**
 * @brief Create the desired PCB
 * @param name Argument stores the desired name of PCB
 * @param process_class Argument stores the desired class of PCB
 * @param priority Argument stores the desired priority of PCB
 */
void create_pcb(const char* name, int process_class, int priority);


 /**
 * @brief Function for handling command input for deleting PCB, called by comhand
 * @param args Arguments for handling delete command
 */
void delete_pcb_command(const char* args);


/**
 * @brief Provides help for how to use the delete command
 */
void delete_help(void);


/**
 * @brief Delete the desired PCB
 * @param name Argument stores the name of desired PCB
 */
void delete_pcb(const char* name);

#endif
