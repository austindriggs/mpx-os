#ifndef COMHAND_H
#define COMHAND_H

/**
 @file comhand.h
 @brief Command handler interface for the OS.
* Reads from the polling input and executes commands.
*/

/**
 * @brief Prints a welcome message and penguin ASCII art to the terminal
 */
void com_startup(void);

/**
 * @brief Trim function to remove \\n and \r from the string.
 * @param str string variable to trim
 */
void trim_Input(char *str);

/**
 * @brief Enters a loop and waits for the user to input commands.
 */
void comhand(void);

#endif
