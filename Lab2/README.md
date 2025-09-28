# Lab2

## Part 1: UART bootloader

1. edit load address to 0x40200000 in linker.ld and kernel.its
2. write 'load' command to load kernel from UART.
3. fail to write addr.0x80000000, change to 0x00200000(origin kernel addr)
4. write script to send kernel from UART

## Part 2: Initial Ramdisk


