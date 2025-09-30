#include <stdint.h>
#include "uart.h"
#include "sbi.h"
#include "utils.h"
#include "initrd.h"

#define PROMPT "opi-rv2> "

unsigned int uart_base = UART_BASE;
char* model_name = "Undefined";

int readline(char* str){
    int pos = 0;

    while(1){
        char c = uart_getc();
        if(c == '\n'){
            uart_putc('\r');
            uart_putc('\n');
            break;
        }
        else if(c == 0x08 || c == 0x7f){//bs
            if(pos == 0)
                continue;
            uart_putc('\b');
            uart_putc(' ');
            uart_putc('\b');
            //replace it with space to do bs
            str[--pos] = '\0';
        }
        else{
            str[pos++] = c;
            str[pos] = '\0';
            uart_putc(c);
        }
    }
    while(pos > 0 && str[pos-1] == ' ')
        str[pos-1] = '\0', pos--;
    return pos;
}

void show_system_info(){
    struct sbiret sbi_version = sbi_ecall(0x10, 0x0, 0,0,0,0,0,0);
    struct sbiret imp_ID = sbi_ecall(0x10, 0x1, 0,0,0,0,0,0);
    struct sbiret imp_version = sbi_ecall(0x10, 0x2, 0,0,0,0,0,0);
    uart_puts("System information:\r\n");

    uart_puts("  OpenSBI version: ");
    ul minor = sbi_version.value & 0xFFFFFUL;
    ul major = (sbi_version.value >> 24) & 0x7FUL; 
    print64N(major);
    uart_putc('.');
    print64N(minor);
    uart_putc('\n');


    uart_puts("  implementation ID: ");
    print64N_h(imp_ID.value);
    uart_putc('\n');


    uart_puts("  implementation version: ");
    print64N_h(imp_version.value);
    uart_putc('\n');

    uart_puts("DTB information:\n");
    uart_puts("  UART base address: ");
    print64N_h(uart_base);
    uart_putc('\n');

    uart_puts("  Model name: ");
    uart_puts(model_name);
    uart_putc('\n');
}

void jump_bootloader(ul dtb){
    char* add = (char*)0x40200000;
    ((int(*)(ul, ul))add)(0, dtb);
}

void run_shell(uint64_t dtb){
    char buffer[1024];
    while(1){
        uart_puts(PROMPT);
        int pos = readline(buffer);
        
        int st = 0;
        while(st < pos && buffer[st] == ' ')
            st++;
        int fsp = 0;
        while(fsp < pos && buffer[fsp] != ' ')
            fsp++;

        if(pos == 0)
            continue;//empty line
        char* nbuf = buffer+st;
        if(strcmp(nbuf, "help") == 0){
            uart_puts("Available commands:\r\n");
            uart_puts("  help   - show all commands.\r\n");
            uart_puts("  hello  - print Hello world.\r\n");
            uart_puts("  info   - print system info.\r\n");
            //uart_puts("  reboot - reboot system.\r\n");
            uart_puts("  exit   - jump back to bootloader.\r\n");
            uart_puts("  ls     - list files.\r\n");
        }
        else if(strcmp(nbuf, "hello") == 0){
            uart_puts("Hello world.\r\n");
        }
        else if(strcmp(nbuf, "info") == 0){
            show_system_info();
        }
        /*else if(strcmp(nbuf, "reboot") == 0){
            sbi_system_reboot();
        }*/
        else if(strcmp(nbuf, "exit") == 0){
            uart_puts("Exit uart kernel.\n");
            jump_bootloader(dtb);
            break;
        }
        else if(strcmp(nbuf, "ls") == 0){
            initrd_list();
        }
        else{
            uart_puts("Unknown command: ");
            uart_puts(nbuf);
            uart_puts("\r\nUse help to get commands.\r\n");
        }

    }
}
