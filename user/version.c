#include <string.h>
#include <sys_req.h>
#include "version.h"



void print_version(void)
{
    const char* version_message = "\r\nMacaroniOS \n\nVersion: R1.0\n";
    sys_req(WRITE, COM1, version_message, strlen(version_message));
}
