#ifndef ALARM_H
#define ALARM_H

/**
 * @brief Struct that stores data relating to the alarm
 * @struct stores Seconds, Minutes, Hours, and Message
 */
typedef struct {
    int hour;
    int minute;
    int second;
    char message[100];
} AlarmData;

/**
 * @brief Create an alarm with at the specified HH:MM:SS with specified message
 */
void alarm(void);

/**
 * @brief Creates a new pcb with alarm data
 * @param data Passes relevant data such as time (Hours, minutes, seconds) and alarm message
 */
void alarm_create(AlarmData* data);

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
