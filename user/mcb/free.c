#include <mpx/vm.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <stddef.h>
#include <stdint.h>
#include "mcb/free.h"
#include "string.h"

static void print_hex(int num) {
    // setup variables
    char hex_chars[] = "0123456789ABCDEF";
    char buf[11];   // 0x + 8 digits + '\0'
    int i;

    // create and print buffer
    buf[0] = '0';
    buf[1] = 'x';
    for (i = 0; i < 8; i++) {
        int shift = (7 - i) * 4;
        buf[2 + i] = hex_chars[(num >> shift) & 0xF];
    }
    buf[10] = '\0';

    sys_req(WRITE, COM1, buf, 10);
}

void free_help(void) {
    const char *helpMsg =
        "\r\n\033[33mfree\033[0m [<\033[36maddress\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33mfree\033[0m <\033[36maddress\033[0m>    frees the memory at the address given.\r\n"
        "  \033[33mfree\033[0m \033[36mhelp\033[0m         prints this help message.\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

int free_command(char *args)
{
    // error checking
    if (!args) {
        free_help();
        return -1;
    }

    // check args
    while (*args == ' ') args++;
    if (*args == '\0') {
        free_help();
        return -1;
    }

    // help command
    if (strncmp(args, "help", 4) == 0) {
        free_help();
        return 0;
    }

    // parse hex string (optional 0x prefix)
    int addr = 0;
    if (args[0] == '0' && (args[1] == 'x' || args[1] == 'X')) {
        args += 2;
    }
    while ((*args >= '0' && *args <= '9') || (*args >= 'a' && *args <= 'f') || (*args >= 'A' && *args <= 'F')) {
        int val = 0;
        if (*args >= '0' && *args <= '9') val = *args - '0';
        else if (*args >= 'a' && *args <= 'f') val = *args - 'a' + 10;
        else if (*args >= 'A' && *args <= 'F') val = *args - 'A' + 10;

        addr = (addr << 4) | val;
        args++;
    }

    // free memory
    if (free_memory((void *)addr) == 0) { 
        const char *msg = "\033[32mSuccessfully freed:\033[0m ";
        sys_req(WRITE, COM1, msg, strlen(msg));
        print_hex(addr);
        sys_req(WRITE, COM1, "\r\n", 2);
        return 0;
    } 
    else {
        const char *err = "\033[31mError: Invalid address.\033[0m\r\n";
        sys_req(WRITE, COM1, err, strlen(err));
        free_help();
        return -1;
    }
}
