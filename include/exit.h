#ifndef EXIT_H
#define EXIT_H
/**
 * @file exit.h
 * @author Caleb Edwards
 * @brief Header file for the exit command used in the command handler.
 * Exits the terminal when called and confirmed by the user.
 */

 /**
  * @brief Begins the shutdown process when the user
  * types 'exit' in the terminal. Confirmation by typing 
  * 'Y' or 'n' is then required to completely exit.
  * @param int return 1 to confirm exit and 0 to return to terminal.
  */
int exit_command(void);

#endif
