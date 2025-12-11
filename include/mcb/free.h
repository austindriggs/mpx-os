#ifndef MCB_FREE_H
#define MCB_FREE_H

/**
 * @file mcb/free.h
 * @brief Shell command handler for freeing memory.
 */

/**
 * @brief Prints help/usage information for the `free` command.
 */
void free_help(void);

/**
 * @brief Executes the free command by calling the kernel function.
 * @param args String containing the hex address to free.
 * @return 0 on success, -1 on error.
 */
int free_command(char *args);

#endif
