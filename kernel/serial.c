#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>

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

	(void)buffer;

	// define the ASCII characters
	//const char ENTER_KEY = 13;
	const char ESC_KEY = 27;
	//const char UP_KEY = 'A';
	//const char DOWN_KEY = 'B';
	//const char LEFT_KEY = 'C';
	//const char RIGHT_KEY = 'D';
	//const char BACKSPACE = 8;
	//const char DELETE = 127;

	// format the serial terminal to look like a penguin
	outb(dev, '0');
	outb(dev, '>');
	outb(dev, ' ');

	// prep for input
	int index = 0;

	// read the input while the buffer is not full
	while (index < (int)len - 1) {
		char input = inb(dev);

		// alphanumerics, spaces, and special keys
		if ((input >= 'a' && input <= 'z') ||
		    (input >= 'A' && input <= 'Z') ||
		    (input >= '0' && input <= '9') ||
		    (input >= '!' && input <= '/') ||
		    (input == ' ')) {

			buffer[index] = input;
			outb(dev, input); // echo
			index++;
		}

		// arrow keys
		else if (input == ESC_KEY) {
			input = inb(dev);
		}

		// CR and LF (user is done)
		else if (input == '\n' || input == '\r') {
			buffer[index] = '\0';
			outb(dev, input);
			return index;
		}
	}

	// returns the number of bytes successfully read or -1
	return index;
}
