#include "uart.h"

int run_kernel(void){
    uart_puts("Hello\n");
    return 0;
}
