#include "uart.h"
#include "sbi.h"

#define PROMPT "opi-rv2> "

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

void print(int x){
    char num[10];
    int len = 0;
    while(x){
        num[len++] = x%10+'0';
        x /= 10;
    }
    for(int i = len - 1;i >= 0;i--)
        uart_putc(num[i]);
    uart_putc('\n');
}

void run_shell(){
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

        if(strcmp(buffer, "help") == 0){
            uart_puts("Available commands:\r\n");
            uart_puts("  help  - show all commands.\r\n");
            uart_puts("  hello - print Hello world.\r\n");
            uart_puts("  info  - print system info.\r\n");
        }
        else if(strcmp(buffer, "hello") == 0){
            uart_puts("Hello world.\r\n");
        }
        else if(strcmp(buffer, "info") == 0){
            struct sbiret info = sbi_ecall(0x10, 0x1, 0,0,0,0,0,0);
            print(info.value);
        }
        else{
            uart_puts("Unknown command: ");
            uart_puts(buffer);
            uart_puts("\r\nUse help to get commands.\r\n");
        }

    }
}
