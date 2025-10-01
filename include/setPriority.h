#ifndef SETPRIORITY_H
#define SETPRIORITY_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>

/**
 * @file setPtriority.h
 * @brief Provides ability to set priority for processes
 */

/**
 * @brief Function for handling command input for setting priority, called by comhand
 * @param args Arguments for handling set priority command
 */
void set_priority_command(const char* args);

/**
 * @brief Provides help for how to use the priority set command
 */
void set_priority_help(void);

/**
 * @brief Sets priority of a process
 * @param name The name of the process that is being modified
 * @param newPriority The priority that the process is to be set to
 */
void setPriority(char* name, int newPriority);


#endif
