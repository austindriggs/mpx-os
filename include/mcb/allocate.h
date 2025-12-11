#ifndef MCB_ALLOCATE_H
#define MCB_ALLOCATE_H

#include <stddef.h>

/**
 * @file mcb/allocate.h
 * @brief Shell command handler for allocating memory via allocate_memory() and printing the resulting address in hex.
 */

/**
 * @brief Prints help/usage information for the `allocate` command.
 */
void allocate_help(void);

/**
 * @brief Executes the allocate command by calling the kernel function.
 * @param args  The argument string passed by the shell: size between 1-50,000 or help.
 * @return 0 on success, -1 on error.
 */
int allocate_command(char *args);

#endif
