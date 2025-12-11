#include <mpx/serial.h>
#include <sys_req.h>
#include "mcb/allocate.h"
#include <mpx/vm.h>
#include <string.h>
#include <stdlib.h>

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

void allocate_help(void) {
    const char *helpMsg =
        "\r\n\033[33mallocate\033[0m [<\033[36msize\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33mallocate\033[0m <\033[36msize\033[0m>    allocates 1-50,000 bytes and prints its address in hex.\r\n"
        "  \033[33mallocate\033[0m \033[36mhelp\033[0m      prints this help message\r\n\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

int allocate_command(char *args) {
    // error checking and cleanup
    if (!args) return -1;
    while (*args == ' ') args++;

    // help command
    if (strncmp(args, "help", 4) == 0) {
        allocate_help();
        return 0;
    }

    // check args
    int size = atoi(args);
    if (size <= 0 || size > 50000) {
        const char *msg = "\033[31mError: invalid size.\033[0m\r\n";
        sys_req(WRITE, COM1, msg, strlen(msg));
        allocate_help();
        return -1;
    }

    // call kernel function
    void *addr = allocate_memory(size);
    if (!addr) {
        const char *err = "\033[31mAllocation failed in kernel function.\033[0m\r\n";
        sys_req(WRITE, COM1, err, strlen(err));
        allocate_help();
        return -1;
    }

    // successfully allocated
    const char *success_msg = "\033[32mSuccessfully allocated:\033[0m ";
    sys_req(WRITE, COM1, success_msg, strlen(success_msg));
    print_hex((int)addr);
    const char *success_addr = "\r\n";
    sys_req(WRITE, COM1, success_addr, strlen(success_addr));

    return 0;
}
