#ifndef SHOWPCB_H
#define SHOWPCB_H

#include <pcb.h>
#include <sys_req.h>
#include <string.h>
#include <comhand.h>
#include <itoa.h>

/**
 * @file showPCB.h
 * @brief Provides ability to display a specific PCB, all PCBs, or PCBs in specific queues
 */

/**
 * @brief Handles which functions should be used, for use in comhand
 * @param args Arguments used for performing correct function
 */
void show_command(const char* args);

/**
 * @brief Displays the details for a PCB if the PCB exists
 * @param name The name of the PCB to be displayed
 */
void showPCB(const char* name);

/**
 * @brief Displays all PCBs in the ready queue
 */
void showReady(void);

/**
 * @brief Displays all PCBs in the blocked queue
 */
void showBlocked(void);

/**
 * @brief Displays all PCBs in all queues
 */
void showAllPCB(void);

/**
 * @brief Help function for the show command
 */
void show_pcb_help(void);

/**
 * @brief Displays all PCBs in the suspended queues
 */
void showSuspended(void);

#endif
