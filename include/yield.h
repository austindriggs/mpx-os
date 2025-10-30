#ifndef YIELD_H
#define YIELD_H

/**
 * @file yield.h
 * @brief Adds yield functionality so that a process can be set to IDLE
 */

 /**
  * @brief Yields the current process
  */
void yield(void);

/**
 * @brief Prints a help message for the yield function
 */
void yield_help(void);

/**
 * @brief Handles arguments given in the command handler for yield
 * @param args Arguments for the yield command
 */
void yield_command(const char* args);

#endif
