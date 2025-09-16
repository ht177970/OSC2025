#pragma once

#ifdef __QEMU__
#define UART_BASE 0x10000000UL
#define UART_RBR  (unsigned char *)(UART_BASE + 0x0)
#define UART_THR  (unsigned char *)(UART_BASE + 0x0)
#define UART_IER  (unsigned char *)(UART_BASE + 0x1)
#define UART_LSR  (unsigned char *)(UART_BASE + 0x5)
#else
#define UART_BASE 0xd4017000UL
#define UART_RBR  (unsigned int *)(UART_BASE + 0x0)
#define UART_THR  (unsigned int *)(UART_BASE + 0x0)
#define UART_IER  (unsigned int *)(UART_BASE + 0x4)
#define UART_LSR  (unsigned int *)(UART_BASE + 0x14)
#endif

void uart_init();
char uart_getc();
void uart_putc(char c);
void uart_puts(const char *s);
