#include "clock.h"
#include <string.h>
#include <sys_req.h>
#include <stdint.h>
#include <mpx/interrupts.h>
#include <mpx/io.h>
#include <mpx/serial.h>
#include "itoa.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

// RTC register indexes
#define REG_SECONDS   0x00
#define REG_MINUTES   0x02
#define REG_HOURS     0x04
#define REG_DAY       0x07
#define REG_MONTH     0x08
#define REG_YEAR      0x09
#define REG_STATUS_B  0x0B

// --- Helpers ---
static inline uint8_t bcd_to_bin(uint8_t value) {
    return ((value >> 4) * 10) + (value & 0x0F);
}

static inline uint8_t bin_to_bcd(uint8_t value) {
    return ((value / 10) << 4) | (value % 10);
}

static uint8_t rtc_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static void rtc_write(uint8_t reg, uint8_t value) {
    outb(CMOS_ADDRESS, reg);
    outb(CMOS_DATA, value);
}

static void my_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

static void my_strcat(char *dest, const char *src) {
    while (*dest) dest++;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void clock_command(const char *args){
    if (args == NULL || *args == '\0') {
        rtc_time_t t;
        rtc_date_t d;
        get_time(&t);
        get_date(&d);
        print_time(&t);
        print_date(&d);
        print_time(&t);
    }
    else if (strcmp(args, "get time") == 0) {
        rtc_time_t t;
        rtc_date_t d;
        get_date(&d);
        get_time(&t);
        print_date(&d);
        print_time(&t);
    }
    else if (strcmp(args, "get date") == 0) {
        rtc_time_t t;
        rtc_date_t d;
        get_time(&t);
        get_date(&d);
        print_time(&t);
        print_date(&d);
    }
    else if (strcmp(args, "help") == 0) {
        clock_help();
    }
    else {
        const char *argMsg = "Invalid argument. Please try again.\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
    }
}

void print_time(const rtc_time_t *time){
    char buffer[100];
    char num[4];
    size_t len = strlen(buffer);

    my_strcpy(buffer, "\r\n\nTime: ");
    itoa(time->hour, num);
    my_strcat(buffer, num);
    my_strcat(buffer, ":");

    itoa(time->minute, num);
    my_strcat(buffer, num);
    my_strcat(buffer, ":");

    itoa(time->second, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "\r\n\n");

    sys_req(WRITE, COM1, buffer, len);
}

void print_date(const rtc_date_t *date){
    char buffer[100];
    char num[4];
    size_t len = strlen(buffer);

    my_strcpy(buffer, "\r\n\nDate: ");

    itoa(date->month, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "/");

    itoa(date->day, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "/");

    itoa(date->year, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "\r\n\n");

    sys_req(WRITE, COM1, buffer, len);
}

void get_time(rtc_time_t *time) {
    uint8_t regB = rtc_read(REG_STATUS_B);

    time->second = rtc_read(REG_SECONDS);
    time->minute = rtc_read(REG_MINUTES);
    time->hour   = rtc_read(REG_HOURS);

    // Convert from BCD if needed
    if (!(regB & 0x04)) {
        time->second = bcd_to_bin(time->second);
        time->minute = bcd_to_bin(time->minute);
        time->hour   = bcd_to_bin(time->hour);
    }
}

void set_time(const rtc_time_t *time) {
    uint8_t regB = rtc_read(REG_STATUS_B);

    cli();
    if (!(regB & 0x04)) { // BCD mode
        rtc_write(REG_SECONDS, bin_to_bcd(time->second));
        rtc_write(REG_MINUTES, bin_to_bcd(time->minute));
        rtc_write(REG_HOURS,   bin_to_bcd(time->hour));
    } else { // Binary mode
        rtc_write(REG_SECONDS, time->second);
        rtc_write(REG_MINUTES, time->minute);
        rtc_write(REG_HOURS,   time->hour);
    }
    sti();
}

void get_date(rtc_date_t *date) {
    uint8_t regB = rtc_read(REG_STATUS_B);

    date->day   = rtc_read(REG_DAY);
    date->month = rtc_read(REG_MONTH);
    date->year  = rtc_read(REG_YEAR);

    // Convert from BCD if needed
    if (!(regB & 0x04)) {
        date->day   = bcd_to_bin(date->day);
        date->month = bcd_to_bin(date->month);
        date->year  = bcd_to_bin(date->year);
    }
}

void set_date(const rtc_date_t *date) {
    uint8_t regB = rtc_read(REG_STATUS_B);

    cli();
    if (!(regB & 0x04)) { // BCD mode
        rtc_write(REG_DAY,   bin_to_bcd(date->day));
        rtc_write(REG_MONTH, bin_to_bcd(date->month));
        rtc_write(REG_YEAR,  bin_to_bcd(date->year));
    } else { // Binary mode
        rtc_write(REG_DAY,   date->day);
        rtc_write(REG_MONTH, date->month);
        rtc_write(REG_YEAR,  date->year);
    }
    sti();
}

void clock_help(void) {
    const char *helpMsg =
        "\r\nclock [get] [date|time]\r\n"
        "\r\nclock [set] [date] [DDMMYY]\r\n"
        "\r\nclock [set] [time] [HHMMSS]\r\n"
        "\r\nclock [help] \r\n"
        "  clock get time         prints the current time as: {hour}:{minute}:{second}\r\n"
        "  clock get date         prints the current date as: {day}/{month}/{year}\r\n"
        "  clock set time         prompts the user for the current time: Hours then Minutes\r\n"
        "  clock set date         prompts the user for the current date: Day of month, Month, then Year\r\n"
        "  clock help             prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}
