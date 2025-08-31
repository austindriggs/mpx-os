#ifndef COMHAND_H
#define COMHAND_H

/**
 @file comhand.h
 @author Caleb Edwards
 @brief Command handler interface for the OS.
* Reads from the polling COM1 input and executes commands
* based on keywords such as 'help' or 'exit'.
*/

/**
 * @brief Prints a welcome message and penguin ASCII art to the terminal
 */
void com_startup(void);

/**
 * @brief Trim function to remove \\n and \r from the string.
 * @param str string to trim
 */
void trim_Input(char *str);

/**
 * @brief Enters a loop and waits for the user to input commands.
 */
void comhand(void);

#endif
