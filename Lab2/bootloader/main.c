#include "uart.h"
#include "shell.h"
#include "dtb.h"

int run_kernel(uint64_t dtb){
    uart_puts("##############################\n");
    uart_puts("#      OSC2025 bootloader    #\n");
    uart_puts("##############################\n");
    uart_puts("");
    if(dtb){
        const char* model = fdt_get_model((const void*)(uintptr_t)dtb);
        uint64_t ub = fdt_get_uart_base((const void*)(uintptr_t)dtb);
        if(ub)
            uart_base = ub;
        else
            uart_base = 0;
        if (model)
            model_name = model;
    }
    else{
        uart_puts("No DTB pointer provided.\n");
    }
    run_shell(dtb);
    return 0;
}
