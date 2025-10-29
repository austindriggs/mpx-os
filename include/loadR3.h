#ifndef LOADR3_H
#define LOADR3_H

/**
 * @file loadR3.h
 * @brief Handles loading the R3 processes into the system
 */

 /**
  * @brief Displays a help message for loading the processes
  */
void load_help(void);

/**
 * @brief Loads all 5 of the R3 processes
 */
void loadR3(void);


/**
 * @brief Loads all 5 of the processes, but in a suspended state
 */
void loadR3_suspended(void);

/**
 * @brief Loads a specific R3 process
 * @param name The name of a specific process to load
 */
void loadProcess(const char* name);

/**
 * @brief Handles the selection of which load function to call
 * @param args Argument for selecting which load function, for example "help"
 */
void load_command(const char* args);

#endif
