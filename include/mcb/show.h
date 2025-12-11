#ifndef MCB_SHOW_H
#define MCB_SHOW_H

/**
 * @file show.h
 * @brief Functions for displaying information about MCBs.
 */

/**
 * @brief Prints help information for the "show mcb" command.
 */
void show_mcb_help(void);

/**
 * @brief Handles the "show mcb" shell command.
 * @param args  User argument string: address or help.
 * @return The address parsed on success, or -1 on failure.
 */
int show_mcb_command(char *args);

/**
 * @brief Prints allocated memory blocks.
 */
void show_mcb_allocated(void);

/**
 * @brief Prints free memory blocks.
 */
void show_mcb_free(void);

/**
 * @brief Prints all memory blocks.
 */
void show_mcb_all(void);

#endif
