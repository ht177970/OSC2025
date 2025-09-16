#include "uart.h"
#include "shell.h"

int run_kernel(void){
//    uart_puts("Hello\n");
    run_shell();
    return 0;
}
