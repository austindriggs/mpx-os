#include "sys_req.h"
#include "string.h"

void yield(void){
    sys_req(IDLE);
}

void yield_command(const char* args){
    if (args == NULL || *args=='\0'){
        yield();
    }
}
