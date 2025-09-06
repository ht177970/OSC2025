SECTIONS
{
    . = 0x00200000;
    .text : { *(.text) }

    .bss : {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }
    . = ALIGN(16);
    . = . + 0x2000;
    __stack_top = .;
}
