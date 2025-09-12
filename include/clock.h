#ifndef CLOCK_H
#define CLOCK_H

#include <string.h>
#include <sys_req.h>
#include <stdint.h>
#include <mpx/interrupts.h>
#include <mpx/io.h>

/**
 * @file clock.h
 * @brief Handles accesses to the Real Time Clock (RTC)
 */

/**
 * @brief Struct that stores data relating to the time
 * @struct stores Seconds, Minutes, and Hours
 */
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
} rtc_time_t;

/**
 * @brief Struct that stores data relating to the date
 * @struct stores Day, Month, and Year
 */
typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t year;   // Last two digits of the year
} rtc_date_t;


/**
 * @brief Accesses the RTC and stores the current time into specified struct
 * @param time Stores the current Seconds, Minutes, and Hours 
 */
void get_time(rtc_time_t *time);

/**
 * @brief Stores the specified time into associated RTC registers
 * @param time Passes user defined Seconds, Minutes, and Hours 
 */
void set_time(const rtc_time_t *time);

/**
 * @brief Accesses the RTC and stores the current date into specified struct
 * @param date Stores the current Day, Month, and Year 
 */
void get_date(rtc_date_t *date);

/**
 * @brief Stores the specified date into associated RTC registers
 * @param date Passes user defined Day, Month, and Year 
 */
void set_date(const rtc_date_t *date);

/**
 * @brief Prints the specified time in the form HH:MM:SS
 * @param time Passes the Seconds, Minutes, and Hours 
 */
void print_time(rtc_time_t *time);

/**
 * @brief Prints the specified date in the form MM/DD/YY
 * @param time Passes the Day, Month, and Year 
 */
void print_date(const rtc_date_t *date);

/**
 * @brief Prints help information related to the clock command.
 */
void clock_help(void);

/**
 * @brief Main handler for the clock command.
 * @param args The argument string passed after 'clock'
 */
void clock_command(const char *args);

//---- Helper Functions ----//

/**
 * @brief Concatenates src to the end of dest
 * @param dest The argument string to be appended to
 * @param src The argument string to append to 
 */
void my_strcat(char *dest, const char *src);

/**
 * @brief Copies src to dest
 * @param dest Stores a copy of src
 * @param src Copied to dest 
 */
void my_strcpy(char *dest, const char *src);

/**
 * @brief Writes data to an RTC register
 * @param reg Address of specified RTC register
 * @param value The byte to write to the RTC register
 */
void rtc_write(uint8_t reg, uint8_t value);

/**
 * @brief Reads data from an RTC register
 * @param reg Address of specified RTC register
 */
uint8_t rtc_read(uint8_t reg);

/**
 * @brief Converts an integer from binary to Binary Coded Decimal (BCD)
 * @param value The value to convert
 * @returns The specified value as a BCD
 */
uint8_t bin_to_bcd(uint8_t value);

/**
 * @brief Converts an integer from Binary Coded Decimal (BCD) to binary
 * @param value The value to convert
 * @returns The specified value in binary
 */
uint8_t bcd_to_bin(uint8_t value);

/**
 * @brief Converts the standard RTC timezone (UTC) to EST
 */
void tz_correction(void);

#endif
