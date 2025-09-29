#include "utils.h"
#include "uart.h"

int strcmp(char* a, char* b){
    while(*a && *b && (*a == *b))
        a++, b++;
    return (int)(*a-*b);
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
