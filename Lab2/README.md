# Lab2

## Part 1: UART bootloader

1. edit load address to 0x40200000 in linker.ld and kernel.its
2. write 'load' command to load kernel from UART.
3. fail to write addr.0x80000000, change to 0x00200000(origin kernel addr)
4. write script to send kernel from UART

## Part 2: Initial Ramdisk

1. From FreeBSD site copy cpio struct to initrd.h
2. write cpio walk logic to initrd.c
3. edit Makefile to create .cpio
4. edit bootloader to receive .cpio
5. by cpio walk, list all files(ls)
6. by cpio walk, cat file(cat)
