#include "uart.h"

int run_kernel(void){
    uart_init();
    uart_puts("Hello\n");
    return 0;
}
