#ifndef ALARM_H
#define ALARM_H

void alarm(const int hours, const int minutes, const int seconds, const char* message);

void alarm_command(const char* args);

void alarm_help(void);

#endif
