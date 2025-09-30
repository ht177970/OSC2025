#include <stdint.h>
#include "uart.h"
#include "sbi.h"

#define PROMPT "opi-rv2> "

unsigned int uart_base = UART_BASE;
char* model_name = "Undefined";

int strcmp(char* a, char* b){
    while(*a && (*a == *b))
        a++, b++;
    return (int)(*a-*b);
}

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

void print64N(ul x){
    if(x == 0){
        uart_putc('0');
        return;
    }
    char num[21];
    int len = 0;
    while(x){
        num[len++] = x%10+'0';
        x /= 10;
    }
    for(int i = len - 1;i >= 0;i--)
        uart_putc(num[i]);
}

void print64N_h(ul x){
    char num[15];
    for(int i = 0; i < 16;i++){
        int r = x % 16;
        if(r >= 10)
            num[15-i] = 'a'+(r-10);
        else
            num[15-i] = '0'+r;
        x >>= 4;
    }
    uart_puts("0x");
    int all0 = 1;
    for(int i = 0;i < 8;i++){
        if(num[i] != '0'){
            all0 = 0;
            break;
        }
    }
    for(int i = all0 ? 8 : 0;i < 16;i++)
        uart_putc(num[i]);
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

void load_kernelcpio_UART(uint64_t dtb){
    ul kernel_size = 0;
    
    uart_puts("Waiting for script of sending kernel...\r\n");

    for(int i = 0;i < 32;i++){
        int b = uart_getc()-'0';
        kernel_size <<= 1;
        kernel_size |= b;
    }

    uart_puts("Kernel size: ");
    print64N(kernel_size);
    uart_puts(" bytes\n");
    
    char* add = (char*)(0x00200000ul);
    for(ul i = 0;i < kernel_size;i++){
        add[i] = uart_getb();
        if(i % 512 == 0){
            print64N(i);
            uart_putc('/');
            print64N(kernel_size);
            uart_puts(" bytes received.\n");
        }
    }
    print64N(kernel_size);
    uart_putc('/');
    print64N(kernel_size);
    uart_puts(" bytes received.\n");

    ul cpio_size = 0;

    for(int i = 0;i < 32;i++){
        int b = uart_getc()-'0';
        cpio_size <<= 1;
        cpio_size |= b;
    }

    uart_puts("cpio size: ");
    print64N(cpio_size);
    uart_puts(" bytes\n");

    char* add2 = (char*)(0x21000000ul);
    for(ul i = 0;i < cpio_size;i++){
        add2[i] = uart_getb();
        if(i % 512 == 0){
            print64N(i);
            uart_putc('/');
            print64N(cpio_size);
            uart_puts(" bytes received.\n");
        }
    }

    print64N(cpio_size);
    uart_putc('/');
    print64N(cpio_size);
    uart_puts(" bytes received.\n");
    
    //((int(*)(ul, uint64_t))add)(0, dtb);
    register unsigned long in0 asm("a0") = 0;
    register unsigned long in1 asm("a1") = (unsigned long)dtb;
    register void *fn asm("t0") = (void*)add;
    register unsigned long out asm("a0");

    asm volatile (
        "jalr ra, %2, 0\n"
        : "=r"(out)                 /* output: ret in a0 */
        : "0"(in0), "r"(fn), "r"(in1) /* "0"(in0) ties output slot 0 to in0 register */
        : "ra", "memory"
    );
}


void load_kernel_UART(uint64_t dtb){
    ul kernel_size = 0;
    
    uart_puts("Waiting for script of sending kernel...\r\n");

    for(int i = 0;i < 32;i++){
        int b = uart_getc()-'0';
        kernel_size <<= 1;
        kernel_size |= b;
    }

    uart_puts("Kernel size: ");
    print64N(kernel_size);
    uart_puts(" bytes\n");
    
    char* add = (char*)(0x00200000ul);
    for(ul i = 0;i < kernel_size;i++){
        add[i] = uart_getb();
        if(kernel_size-i <= 100 || i % 128 == 0){
            print64N(i);
            uart_putc('/');
            print64N(kernel_size);
            uart_puts(" bytes received.\n");
        }
    }
    uart_puts("File received.\r\n");
    
    //((int(*)(ul, uint64_t))add)(0, dtb);
    register unsigned long in0 asm("a0") = 0;
    register unsigned long in1 asm("a1") = (unsigned long)dtb;
    register void *fn asm("t0") = (void*)add;
    register unsigned long out asm("a0");

    asm volatile (
        "jalr ra, %2, 0\n"
        : "=r"(out)                 /* output: ret in a0 */
        : "0"(in0), "r"(fn), "r"(in1) /* "0"(in0) ties output slot 0 to in0 register */
        : "ra", "memory"
    );
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
            uart_puts("  help    - show all commands.\r\n");
            uart_puts("  hello   - print Hello world.\r\n");
            uart_puts("  info    - print system info.\r\n");
            uart_puts("  reboot  - reboot system.\r\n");
            uart_puts("  load    - start UART loading kernel mode(both img and cpio).\r\n");
            uart_puts("  loadimg - start UART loading kernel mode(only img).\r\n");
        }
        else if(strcmp(nbuf, "hello") == 0){
            uart_puts("Hello world.\r\n");
        }
        else if(strcmp(nbuf, "info") == 0){
            show_system_info();
        }
        else if(strcmp(nbuf, "reboot") == 0){
            sbi_system_reboot();
        }
        else if(strcmp(nbuf, "load") == 0){
            load_kernelcpio_UART(dtb);
        }
        else if(strcmp(nbuf, "loadimg") == 0){
            load_kernel_UART(dtb);
        }
        else{
            uart_puts("Unknown command: ");
            uart_puts(nbuf);
            uart_puts("\r\nUse help to get commands.\r\n");
        }

    }
}
