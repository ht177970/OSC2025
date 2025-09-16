# Lab1

## Part 1: Basic Initialization

1. edit a.S and linker.ld
2. add main.c with empty function
3. add makefile

## Part 2: UART Setup

1. copy uart.c and uart.h
2. edit main.c to use uart
3. fail and try edit Makefile to add arguments about compile
4. success on qemu but fail on real RV2
5. fix by removing uart_init() from main.c

## Part 3: Simple Shell

1. write shell logic in shell.c
2. edit main.c to call run_shell

## Part 4: System Information

1. write sbi logic in sbi.c and sbi.h
2. add info command(edit shell.c)
