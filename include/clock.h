#ifndef CLOCK_H
#define CLOCK_H

#include <string.h>
#include <sys_req.h>
#include <stdint.h>
#include <mpx/interrupts.h>
#include <mpx/io.h>

/**
 * @file clock.h
 * @author Eammon Anderson
 * @brief Header file for the clock functions.
 * get/set TIME get/set DATE.
 */

// Time structure
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
} rtc_time_t;

// Date structure
typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t year;   // Last two digits of the year
} rtc_date_t;

void get_time(rtc_time_t *time);
void set_time(const rtc_time_t *time);

void get_date(rtc_date_t *date);
void set_date(const rtc_date_t *date);

void print_clock(const rtc_time_t *time, const rtc_date_t *date);

void clock_command(void);

#endif
