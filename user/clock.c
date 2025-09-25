#include "clock.h"
#include <string.h>
#include <sys_req.h>
#include <stdint.h>
#include <mpx/interrupts.h>
#include <mpx/io.h>
#include "stdlib.h"
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
uint8_t bcd_to_bin(uint8_t value) {
    return ((value >> 4) * 10) + (value & 0x0F);
}

uint8_t bin_to_bcd(uint8_t value) {
    return ((value / 10) << 4) | (value % 10);
}

uint8_t rtc_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void rtc_write(uint8_t reg, uint8_t value) {
    outb(CMOS_ADDRESS, reg);
    outb(CMOS_DATA, value);
}

void my_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void my_strcat(char *dest, const char *src) {
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
        print_date(&d);
        print_time(&t);
    }
    else if (strcmp(args, "get time") == 0) {
        rtc_time_t t;
        get_time(&t);
        print_time(&t);
    }
    else if (strcmp(args, "get date") == 0) {
        rtc_date_t d;
        get_date(&d);
        print_date(&d);
    }
    else if (strncmp(args, "set time ", 9) == 0) {
        const char *val = args + 9;  // skip "set time "

        // Expected format: HH:MM:SS
        if (strlen(val) != 8 || val[2] != ':' || val[5] != ':') {
            const char *err = "\033[31mInvalid format. Use set time <HH:MM:SS>\033[0m\r\n";
            sys_req(WRITE, COM1, err, strlen(err));
            clock_help();
            return;
        }

        char buf[3];
        buf[2] = '\0';

        buf[0] = val[0]; buf[1] = val[1];
        int hour = atoi(buf);

        buf[0] = val[3]; buf[1] = val[4];
        int minute = atoi(buf);

        buf[0] = val[6]; buf[1] = val[7];
        int second = atoi(buf);

        if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
            const char *err = "\033[31mInvalid time values.\033[0m\r\n";
            sys_req(WRITE, COM1, err, strlen(err));
            clock_help();
            return;
        }

        rtc_time_t t = { second, minute, hour };
        set_time(&t);
        print_time(&t);
    }
    else if (strncmp(args, "set date ", 9) == 0) {
        const char *val = args + 9;  // skip "set date "

        // Expected format: DD/MM/YY
        if (strlen(val) != 8 || val[2] != '/' || val[5] != '/') {
            const char *err = "\033[31mInvalid format. Use set date <MM/DD/YY>\033[0m\r\n";
            sys_req(WRITE, COM1, err, strlen(err));
            clock_help();
            return;
        }

        char buf[3];
        buf[2] = '\0';

        buf[0] = val[0]; buf[1] = val[1];
        int month = atoi(buf);

        buf[0] = val[3]; buf[1] = val[4];
        int day = atoi(buf);

        buf[0] = val[6]; buf[1] = val[7];
        int year = atoi(buf);

        if (day < 1 || day > 31 || month < 1 || month > 12) {
            const char *err = "\033[31mInvalid date values.\033[0m\r\n";
            sys_req(WRITE, COM1, err, strlen(err));
            clock_help();
            return;
        }

        rtc_date_t d = { day, month, year };
        set_date(&d);
        print_date(&d);
    }
    else if (strcmp(args, "help") == 0) {
        clock_help();
    }
    else {
        const char *argMsg = "\033[31mInvalid argument. Please try again.\033[0m\r\n";
        sys_req(WRITE, COM1, argMsg, strlen(argMsg));
        clock_help();
    }
}

void print_time(rtc_time_t *time){
    char buffer[100];
    char num[4];
    
    //time->hour = (time->hour + 24 - 4) % 24;
    my_strcpy(buffer, "\r\nTime: ");

    if (time->hour < 10) my_strcat(buffer, "0");
    itoa(time->hour, num);
    my_strcat(buffer, num);
    my_strcat(buffer, ":");

    if (time->minute < 10) my_strcat(buffer, "0");
    itoa(time->minute, num);
    my_strcat(buffer, num);
    my_strcat(buffer, ":");

    if (time->second < 10) my_strcat(buffer, "0");
    itoa(time->second, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "\r\n\n");

    size_t len = strlen(buffer);

    sys_req(WRITE, COM1, buffer, len);
}

void print_date(const rtc_date_t *date){
    char buffer[100];
    char num[4];

    my_strcpy(buffer, "\r\nDate: ");

    if (date->month < 10) my_strcat(buffer, "0");
    itoa(date->month, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "/");

    if (date->day < 10) my_strcat(buffer, "0");    
    itoa(date->day, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "/");

    if (date->year < 10) my_strcat(buffer, "0");
    itoa(date->year, num);
    my_strcat(buffer, num);
    my_strcat(buffer, "\r\n\n");

    size_t len = strlen(buffer);

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
        "\r\n\033[33mclock\033[0m [\033[36mget\033[0m|\033[36mset\033[0m|\033[36mhelp\033[0m]  [\033[36mtime\033[0m <\033[36mHH:MM:SS\033[0m>|\033[36mdate\033[0m <\033[36mMM/DD/YY\033[0m>]\r\n"
        "  \033[33mclock\033[0m                        prints the current date and time.\r\n"
        "  \033[33mclock\033[0m \033[36mget\033[0m \033[36mtime\033[0m               prints the current time as: hour:minute:second.\r\n"
        "  \033[33mclock\033[0m \033[36mget\033[0m \033[36mdate\033[0m               prints the current date as: month/day/year.\r\n"
        "  \033[33mclock\033[0m \033[36mset\033[0m \033[36mtime\033[0m <\033[36mHH:MM:SS\033[0m>    sets the current time to: hour:minute:second.\r\n"
        "  \033[33mclock\033[0m \033[36mset\033[0m \033[36mdate\033[0m <\033[36mMM/DD/YY\033[0m>    sets the current date to: month/day/year.\r\n"
        "  \033[33mclock\033[0m \033[36mhelp\033[0m                   prints this message\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

void tz_correction(void){
    rtc_time_t t;
    rtc_date_t d;
    get_time(&t);
    get_date(&d);

    // Determine if subtracting 4 hours goes to previous day
    int new_hour = t.hour - 4;
    if (new_hour < 0) {
        new_hour += 24;

        // decrement day
        d.day--;

        // handle month/year wraparound
        if (d.day < 1) {
            d.month--;
            if (d.month < 1) {
                d.month = 12;
                d.year--;
            }

            // set day to last day of new month
            switch (d.month) {
                case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                    d.day = 31; break;
                case 4: case 6: case 9: case 11:
                    d.day = 30; break;
                case 2:
                    d.day = (d.year % 4 == 0) ? 29 : 28;
                    break;
            }
        }
    }

    t.hour = new_hour;

    // commit corrected time and date
    set_time(&t);
    set_date(&d);
}
