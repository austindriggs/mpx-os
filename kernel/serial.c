#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <mpx/interrupts.h>

////////// DEFINE VARIABLES ///////////////

#define HISTORY_SIZE 5
#define MAX_CMD_LEN 128

static char command_history[HISTORY_SIZE][MAX_CMD_LEN];
static int history_count = 0;
static int history_index = 0;

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

static int initialized[4] = { 0 };

////////// HISTORY FUNCTIONS ///////////////

int history_errors(char *buffer) {
	// ensure buffer is not empty
	if (buffer == NULL || strlen(buffer) == 0) {
		return -1;
	}
	
	// trim leading spaces
	size_t index = 0;
	while (buffer[index] == ' ' && index < strlen(buffer)) {
		index++;
	}
	if (index > 0) {
		size_t j = 0;
		while (buffer[index + j] != '\0' && (index + j) < strlen(buffer)) {
			buffer[j] = buffer[index + j];
			j++;
		}
		buffer[j] = '\0';
	}

	// trim trailing spaces
	size_t end = strlen(buffer);
	while (end > 0 && buffer[end - 1] == ' ') {
		end--;
	}
	buffer[end] = '\0';

	// ignore if buffer is now empty after trimming
	if (strlen(buffer) == 0) {
		return -1;
	}

	return 0;
}

int history_handler(char *buffer) {
	// handle input based on array size
	if (history_count < HISTORY_SIZE) {
		strncpy(command_history[history_count], buffer, MAX_CMD_LEN - 1);
		command_history[history_count][MAX_CMD_LEN - 1] = '\0';
		history_count++;
	} 
	
	// buffer is full
	else {
		for (int i = 1; i < HISTORY_SIZE; i++) {
			strncpy(command_history[i - 1], command_history[i], MAX_CMD_LEN - 1);
			command_history[i - 1][MAX_CMD_LEN - 1] = '\0';
		}
		strncpy(command_history[HISTORY_SIZE - 1], buffer, MAX_CMD_LEN - 1);
		command_history[HISTORY_SIZE - 1][MAX_CMD_LEN - 1] = '\0';
	}

	history_index = history_count;

	return 0;
}



////////// SERIAL FUNCTIONS ///////////////

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}


int serial_poll(device dev, char *buffer, size_t len)
{
	// This function must properly handle the following based off ASCII:
	// - alphanumerics (a-z, A-Z, 0-9) and regular special keys (/!#$)
	// - space, backspace, delete, arrow keys (up down left right)
	// - carriage returns (\r) and new lines (\n)

	// define some ASCII characters
	const char ESC_KEY = 27;
	const char BACKSPACE = 8;
	const char DELETE = 127;

	// format the serial terminal to look like a penguin
	sys_req(WRITE, COM1, "( ')> ", 6);

	// read the input while the buffer is not full
	int index = 0;
	char input;
	char specialKey;
	int endOfLine = 0;
	while (index < (int)len - 1) {
		input = inb(dev);

		// alphanumerics, spaces, and special keys
		if ((input >= 'a' && input <= 'z') ||
			(input >= 'A' && input <= 'Z') ||
			(input >= '0' && input <= '9') ||
			(input >= '!' && input <= '/') ||
			(input == ' ') || (input == ':')) {

			buffer[index] = input;
			outb(dev, input); // echo
			index++;
			endOfLine++;
		}

		// backspace and delete
		else if ((input == BACKSPACE || input == DELETE) && (index > 0)) {
			// remove the last character from the buffer
			index--;
			endOfLine--;
			buffer[index] = '\0';

			// erase the last character from the terminal
			outb(dev, '\b');
			outb(dev, ' ');
			outb(dev, '\b');
		}

		// CR and LF (user is done)
		else if (input == '\n' || input == '\r') {
			// end the line
			buffer[endOfLine] = '\0';
			outb(dev, input);

			// handle command history and reset to "empty line" position
			history_handler(buffer);
			history_index = history_count;  

			return endOfLine;
		}

		// handles multi-byte characters that use the escape character
		else if (input == ESC_KEY) {
			input = inb(dev);
			while (1) {
				specialKey = inb(dev);

				// command history error checking
				if (history_index < 0) history_index = 0;
				if (history_index > history_count) history_index = history_count;

				// UP arrow
				if (specialKey == 'A' && history_count > 0) {
					// load the last command
					if (history_index > 0 && history_index < HISTORY_SIZE) history_index--;

					// move the cursor to the end and clear the line
					while (index < endOfLine) {
						sys_req(WRITE, COM1, "\033[C", 3);
						index++;
					}
					for (int i = 0; i < endOfLine; i++) sys_req(WRITE, COM1, "\b \b", 3);

					// write the last command and set new index and endOfLine
					strncpy(buffer, command_history[history_index], MAX_CMD_LEN - 1);
					buffer[MAX_CMD_LEN - 1] = '\0';
					sys_req(WRITE, COM1, buffer, strlen(buffer));
					endOfLine = strlen(buffer);
					index = endOfLine;

					break;
				}

				// DOWN arrow
				else if (specialKey == 'B') {
					// default behavior for command history
					if (history_index < history_count - 1) {
						// load next command
						history_index++;

						// move the cursor to the end and clear the line
						while (index < endOfLine) {
							sys_req(WRITE, COM1, "\033[C", 3);
							index++;
						}
						for (int i = 0; i < endOfLine; i++) sys_req(WRITE, COM1, "\b \b", 3);

						// write the next command and set new index and endOfLine
						strncpy(buffer, command_history[history_index], MAX_CMD_LEN - 1);
						buffer[MAX_CMD_LEN - 1] = '\0';
						sys_req(WRITE, COM1, buffer, strlen(buffer));
						endOfLine = strlen(buffer);
						index = endOfLine;
					} 
					// clear the line when you get to the end of the history
					else if (history_index == history_count - 1) {
						// clear line (move to "new" command position)
						history_index = history_count;
						while (index < endOfLine) {
							sys_req(WRITE, COM1, "\033[C", 3);  // move cursor right
							index++;
						}
						for (int i = 0; i < endOfLine; i++) sys_req(WRITE, COM1, "\b \b", 3);

						// reset the buffer, index, and endOfLine
						buffer[0] = '\0';
						endOfLine = 0;
						index = 0;
					}

					break;
				}

				// RIGHT arrow (until you get to the end of the characters)
				else if (specialKey == 'C' && index < endOfLine) {
					sys_req(WRITE, COM1, "\033[C", 3);
					index++;
					break;
				}

				// LEFT arrow (until you get to the start of the characters)
				else if (specialKey == 'D' && index != 0) {
					sys_req(WRITE, COM1, "\033[D", 3);
					index--;
					break;
				}

				// DELETE key (ESC[3~)
				else if (specialKey == '3') {
					// ensure its the delete key
					while (input != '~'){
						input = inb(dev);
					}

					// delete until the last character
					if (index < (endOfLine-1)) {
						for (int i = index; i < endOfLine - 1; i++) {
							buffer[i] = buffer[i + 1];
							outb(dev, buffer[i]);
						}
						outb(dev, ' ');
						for (int i = index; i < endOfLine; i++){
							sys_req(WRITE, COM1, "\033[D", 3);
						}
						endOfLine--;
					}
					// delete the last character
					else if (index == (endOfLine-1)){
						buffer[index] = '\0';
						sys_req(WRITE, COM1, " \033[D", 4);
						endOfLine--;
					}
					else {
						// ignore sequence
					}
					break;
				}
			}
		} // end of multi byte chars
		
	}

	// returns -1 if buffer overflow or when there is an error
	buffer[index] = '\0';
	return -1;
}
