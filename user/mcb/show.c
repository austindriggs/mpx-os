#include "mcb/show.h"
#include <mpx/vm.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>

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

static void print_dec(int num) {
    // setup variables for 32-bit int
    char buf[12];
    int i = 0, j, k;
    char temp;

    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        sys_req(WRITE, COM1, buf, 1);
        return;
    }

    while (num > 0 && i < 11) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    // reverse the buffer
    for (j = 0, k = i - 1; j < k; j++, k--) {
        temp = buf[j];
        buf[j] = buf[k];
        buf[k] = temp;
    }
    buf[i] = '\0';
    sys_req(WRITE, COM1, buf, i);
}

void show_mcb_help(void) {
    const char *helpMsg =
        "\r\n\033[33mshow mcb\033[0m [\033[36mallocated\033[0m|\033[36mfree\033[0m|<\033[36maddress\033[0m>|\033[36mhelp\033[0m]\r\n"
        "  \033[33mshow mcb\033[0m              shows info about all memory.\r\n"
        "  \033[33mshow mcb\033[0m \033[36mallocated\033[0m    shows info about all allocated memory.\r\n"
        "  \033[33mshow mcb\033[0m \033[36mfree\033[0m         shows info about all free memory.\r\n"
        "  \033[33mshow mcb\033[0m <\033[36maddress\033[0m>    shows info about the memory at the address given.\r\n"
        "  \033[33mshow mcb\033[0m \033[36mhelp\033[0m         prints this help message.\r\n"
        "\r\n";
    sys_req(WRITE, COM1, helpMsg, strlen(helpMsg));
}

//Show allocated function

void show_mcb_allocated(void){
    //setup variable
    struct mcb *cur;
    int mcb_size = 0;
    int mcb_addr = 0;

    //iterate through loop
    for (cur = mem_allocated_list.head; cur; cur = cur->next) {
        //assign variables
        mcb_addr=cur->startAddr;
        mcb_size=cur->size;

        //print address
        sys_req(WRITE, COM1, "\r\nAllocated Memory\n", strlen("\r\nAllocated Memory\n"));
        sys_req(WRITE, COM1, "Start Address: ", strlen("Start Address: "));
        print_hex(mcb_addr);

        //print size
        sys_req(WRITE, COM1, "\nSize: ", strlen("\nSize: "));
        print_dec(mcb_size);
        sys_req(WRITE, COM1, "\n\n", strlen("\n\n"));
    }

}

//Show free function

void show_mcb_free(void){
    //setup variable
    struct mcb *cur;
    int mcb_size = 0;
    int mcb_addr = 0;
    
    //iterate through loop
    for (cur = mem_free_list.head; cur; cur = cur->next) {
        //assign variables
        mcb_addr=cur->startAddr;
        mcb_size=cur->size;

        //print address
        sys_req(WRITE, COM1, "\r\nFree Memory\n", strlen("\r\nFree Memory\n"));
        sys_req(WRITE, COM1, "Start Address: ", strlen("Start Address: "));
        print_hex(mcb_addr);

        //print size
        sys_req(WRITE, COM1, "\nSize: ", strlen("\nSize: "));
        print_dec(mcb_size);
        sys_req(WRITE, COM1, "\n\n", strlen("\n\n"));
    }
}

//Show all function

void show_mcb_all(void){
    show_mcb_allocated();
    show_mcb_free();
}

int show_mcb_command(char *args) {
    // setup variables and check args
    int addr = 0;
    int i = 0;
    if (args == NULL) return -1;

    // help command
    if (strncmp(args, "help", 4) == 0) {
        show_mcb_help();
        return 0;
    }
    else if (strncmp(args, "allocated", 9) == 0) {
        show_mcb_allocated();
        return 0;
    }
    else if (strncmp(args, "free", 4) == 0) {
        show_mcb_free();
        return 0;
    }
    else if (args == NULL || *args=='\0' || (strcmp(args, "all")==0)){
        show_mcb_all();
        return 0;
    }

    // parse hex string from args
    if (args[0] == '0' && (args[1] == 'x' || args[1] == 'X')) i = 2;

    while (args[i] != '\0') {
        char c = args[i];
        int val = 0;
        if (c >= '0' && c <= '9') val = c - '0';
        else if (c >= 'A' && c <= 'F') val = 10 + (c - 'A');
        else if (c >= 'a' && c <= 'f') val = 10 + (c - 'a');
        else break; // invalid char
        addr = (addr << 4) | val;
        i++;
    }

    // setup more variables
    struct mcb *cur;
    int found = 0;

    // check allocated list
    for (cur = mem_allocated_list.head; cur; cur = cur->next) {
        if (cur->startAddr == addr) {
            const char *header_msg = "\r\nAllocated Block(s):\r\n";
            sys_req(WRITE, COM1, header_msg, strlen(header_msg));

            const char *addr_msg = "  Start address = .";
            sys_req(WRITE, COM1, addr_msg, strlen(addr_msg));
            print_hex(cur->startAddr);

            const char *size_msg = ", Size = ";
            sys_req(WRITE, COM1, size_msg, strlen(size_msg));
            print_dec(cur->size);

            sys_req(WRITE, COM1, "\r\n", 2);
            found = 1;
        }
    }

    sys_req(WRITE, COM1, "\r\n", 2);

    // check free list if not found
    for (cur = mem_free_list.head; cur; cur = cur->next) {
        if (cur->startAddr == addr) {
            const char *header_msg = "\r\nFree Block(s):\r\n";
            sys_req(WRITE, COM1, header_msg, strlen(header_msg));

            const char *addr_msg = "  Start address = ";
            sys_req(WRITE, COM1, addr_msg, strlen(addr_msg));
            print_hex(cur->startAddr);

            const char *size_msg = ", Size = .";
            sys_req(WRITE, COM1, size_msg, strlen(size_msg));
            print_dec(cur->size);

            sys_req(WRITE, COM1, "\r\n", 2);
            found = 1;
        }
    }

    if (!found) {
        const char *err = "Error: Block not found.\r\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return -1;
    }

    return addr;
}
