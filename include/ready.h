#ifndef PCB_READY_H
#define PCB_READY_H

/**
 * @file ready.h
 * @brief Commands to suspend or resume a process.
 */

#include "pcb.h"

/**
 * @brief Prints the help message for the suspend command.
 */
void suspend_help(void);

/**
 * @brief Puts a process in the suspended state, and moves it the appropriate queue.
 * @param process_name Process's name (checks for validity)
 * @return int 0 for success, -1 for invalid process/name, and 1 if already suspended.
 */
int suspend_pcb(const char* process_name);

/**
 * @brief Handles command-line arguments for the suspend command.
 * @param args Command argument string (process name or "help").
 */
void suspend_command(const char *args);

/**
 * @brief Prints the help message for the resume command.
 */
void resume_help(void);

/**
 * @brief Puts a process in the active (not suspended) state, and moves it the appropriate queue.
 * @param process_name Process's name (checks for validity)
 * @return int 0 for success, -1 for invalid process/name, and 1 if already active.
 */
int resume_pcb(const char* process_name);

/**
 * @brief Handles command-line arguments for the resume command.
 * @param args Command argument string (process name or "help").
 */
void resume_command(const char *args);

#endif
