#ifndef MPX_SERIAL_H
#define MPX_SERIAL_H

#include <stddef.h>
#include <mpx/device.h>

/**
 @file mpx/serial.h
 @brief Kernel functions and constants for handling serial I/O
*/

/**
 * @brief Performs trimming and validation on a command before adding to history.
 * @param buffer The command string to clean
 * @return 0 on success, -1 if empty/invalid
 */
int history_errors(char *buffer);

/**
 * @brief Adds a command to the history buffer and performs shifting when the buffer is full.
 * @param buffer The command to add
 * @return 0 always
 */
int history_handler(char *buffer);

/**
 * @brief Initializes devices for user input and output
 * @param device A serial port to initialize (COM1, COM2, COM3, or COM4)
 * @return 0 on success, non-zero on failure
 */
int serial_init(device dev);

/**
 * @brief Writes a buffer to a serial port
 * @param device The serial port to output to
 * @param buffer A pointer to an array of characters to output
 * @param len The number of bytes to write
 * @return The number of bytes written
 */
int serial_out(device dev, const char *buffer, size_t len);

/**
 * @brief Reads a string from a serial port
 * @param device The serial port to read data from
 * @param buffer A buffer to write data into as it is read from the serial port
 * @param count The maximum number of bytes to read
 * @return The number of bytes read on success, a negative number on failure
 */
int serial_poll(device dev, char *buffer, size_t len);

int serial_open(device dev, int speed);
int serial_close(device dev);
int serial_write(device dev, char *buf, size_t len);
int serial_read(device dev, char *buf, size_t len);

#endif
