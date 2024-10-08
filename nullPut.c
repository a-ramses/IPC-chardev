#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "linux-5.10.16/arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc , char**argv){
    char * in = NULL;
    syscall(__NR_put,in,sizeof(char) * 6); 
}
