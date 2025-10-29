#ifndef ALARM_H
#define ALARM_H

/**
 * @brief Create an alarm with at the specified HH:MM:SS with specified message
 * @param hours passes the user defined hours 
 * @param minutes passes the user defined minutes
 * @param seconds passes the user defined seconds
 * @param message passes the user defined message
 */
void alarm(const int hours, const int minutes, const int seconds, const char* message);

/**
 * @brief Main handler for the alarm command.
 * @param args The argument string passed after 'alarm'
 */
void alarm_command(const char* args);

/**
 * @brief Prints help information related to the alarm command.
 */
void alarm_help(void);

#endif
